#ifndef MODBUSSEQUENCER_H
#define MODBUSSEQUENCER_H

#include <QObject>
#include <QModbusClient>
#include <QQueue>
#include <QTimer>
#include <QDateTime>
#include <QDebug>
#include <QModbusReply>
#include <QMutex>
#include <QMutexLocker>
#include <QAtomicInt>
#include <QThread>

#include "modbushelper.h"

class ModbusSequencer : public QObject
{
    Q_OBJECT
public:
    // 构造函数：传入外部Modbus客户端实例，初始化成员变量
    // 参数：client - 外部创建的Modbus客户端（如QModbusTcpClient/QModbusRtuSerialMaster）
    //       parent - 父对象，用于Qt内存管理
    explicit ModbusSequencer(QModbusClient *client, QObject *parent = 0)
        : QObject(parent), modbusClient(client), currentReply(nullptr),
          processingFlag(0), retryPending(false), retryDelay(1000),
          currentState(QModbusDevice::UnconnectedState)
    {
        // 连接Modbus设备错误信号（设备级错误，如连接失败、超时）
        connect(modbusClient, &QModbusClient::errorOccurred,
                this, &ModbusSequencer::handleModbusError,
                Qt::DirectConnection);

        // 连接Modbus连接状态变化信号，实时更新内部状态并通知外部
        connect(modbusClient, &QModbusClient::stateChanged,
                this, &ModbusSequencer::onStateChanged,
                Qt::DirectConnection);
    }

    // 析构函数：清理队列和未完成请求，避免内存泄漏
    ~ModbusSequencer()
    {
        clearQueue();
    }

    // 入队Modbus原始请求（线程安全）
    // 参数：serverId - Modbus从站地址（1~247）
    //       pduData - Modbus PDU数据（不含从站地址和CRC/LRC）
    //       functionCode - Modbus功能码（如0x03读保持寄存器、0x06写单个寄存器）
    void enqueueRawRequest(int serverId, const QByteArray &pduData, int functionCode)
    {
        QMutexLocker locker(&mutex); // 加锁保护队列操作

        ModbusTransaction trans;
        trans.serverId = serverId;
        trans.functionCode = functionCode;
        trans.requestData = pduData;
        trans.timestamp = QDateTime::currentDateTime();
        trans.retryCount = 0;       // 初始重试次数为0
        trans.maxRetries = 3;       // 最大重试次数（可根据需求调整）

        requestQueue.enqueue(trans); // 将请求加入队列

        triggerProcessing(); // 触发请求处理（非阻塞）
    }

    // 清空请求队列（线程安全）
    // 功能：清除所有待处理请求，终止当前未完成请求，重置状态标志
    void clearQueue()
    {
        QMutexLocker locker(&mutex);
        requestQueue.clear();       // 清空待处理队列
        cleanupCurrentReply();      // 清理当前未完成的请求
        processingFlag = 0;         // 重置处理标志
        retryPending = false;       // 重置重试标志
        retryDelay = 1000;          // 重置重试延迟（恢复为1秒）
    }

    // 获取当前Modbus连接状态（线程安全）
    // 返回值：QModbusDevice::State枚举（Disconnected/Connecting/Connected/Closing）
    QModbusDevice::State getConnectionState() const
    {
        QMutexLocker locker(&mutex);
        // 若客户端为空，返回"未连接"状态
        return modbusClient ? modbusClient->state() : QModbusDevice::UnconnectedState;
    }

    // 静态函数：将连接状态枚举转换为可读英文字符串
    // 参数：state - 连接状态枚举
    // 返回值：对应的英文描述（如"Connected"、"Disconnected"）
    static QString stateToString(QModbusDevice::State state)
    {
        switch (state) {
            case QModbusDevice::UnconnectedState: return "Disconnected";
            case QModbusDevice::ConnectingState: return "Connecting";
            case QModbusDevice::ConnectedState: return "Connected";
            case QModbusDevice::ClosingState: return "Disconnecting";
            default: return "Unknown state";
        }
    }

private slots:
    // 处理请求完成信号（QModbusReply::finished触发）
    void handleReplyFinished()
    {
        QModbusReply *reply = qobject_cast<QModbusReply*>(sender());
        if (!reply) return; // 若reply为空，直接返回

        // 保存请求结果（避免reply被delete后无法访问）
        bool isSuccess = (reply->error() == QModbusDevice::NoError);
        QModbusDataUnit result;
        if (isSuccess) {
            result = reply->result(); // 成功时获取返回数据
        } else {
            qWarning() << "Request failed:" << reply->errorString(); // 失败时打印错误信息
        }

        reply->deleteLater(); // 立即释放reply资源（Qt内存管理）

        {
            QMutexLocker locker(&mutex);
            // 确认当前reply是正在处理的请求（防止并发冲突）
            if (reply == currentReply) {
                currentReply = nullptr;
                // 只有请求成功时，才从队列移除该请求，并重置重试延迟
                if (isSuccess && !requestQueue.isEmpty()) {
                    requestQueue.dequeue();
                    retryDelay = 1000;
                }
            }
        }

        // 成功时处理返回数据（如解析寄存器值）
        if (isSuccess) {
            processSuccess(result);
        }

        triggerProcessing(); // 触发下一个请求处理
    }

    // 处理Modbus设备级错误（QModbusClient::errorOccurred触发）
    // 如：连接失败、通信超时、端口占用等
    void handleModbusError(QModbusDevice::Error error)
    {
        qWarning() << "Modbus error:" << error << modbusClient->errorString();

        QMutexLocker locker(&mutex);
        // 若存在未完成的请求，清理资源并标记需要重试
        if (currentReply) {
            cleanupCurrentReply();
            retryPending = true;

            // 连接错误时，采用指数退避策略（重试延迟翻倍，最大5秒）
            if (error == QModbusDevice::ConnectionError) {
                retryDelay = qMin(retryDelay * 2, 5000);
            }
        }
        if (!processingFlag)
            triggerProcessing(); // 触发下一个请求处理（可能是重试当前请求）
    }

    // 处理请求超时（1秒未收到响应时触发）
    void handleRequestTimeout()
    {
        QMutexLocker locker(&mutex);
        // 检查当前请求是否仍未完成（避免重复处理已完成的请求）
        if (currentReply && !currentReply->isFinished()) {
            qWarning() << "Request timeout for server:" << currentReply->serverAddress();
            cleanupCurrentReply(); // 清理超时请求
            retryPending = true;   // 标记需要重试
            retryDelay = qMin(retryDelay * 2, 5000); // 超时后重试延迟翻倍
        }

        triggerProcessing(); // 触发下一个请求处理
    }

    // 请求处理入口（原子操作防止重入）
    // 功能：确保同一时间只有一个线程执行请求处理逻辑
    void processNextRequest()
    {
        // testAndSetAcquire：原子操作，若processingFlag为0则设为1，返回true
        if (processingFlag.testAndSetAcquire(0, 1)) {
            processNextRequestImpl(); // 实际处理逻辑
            processingFlag = 0;       // 释放处理标志
        }
    }

    // 处理连接状态变化（QModbusClient::stateChanged触发）
    // 功能：更新内部状态、通知外部、连接恢复时触发请求处理
    void onStateChanged(QModbusDevice::State newState)
    {
        QMutexLocker locker(&mutex);
        currentState = newState;
        // 发送状态变化信号，外部可监听以更新UI或处理业务逻辑
        emit connectionStateChanged(newState, stateToString(newState));

        // 当连接从"非连接"变为"已连接"时，自动触发队列中的请求处理
        if (newState == QModbusDevice::ConnectedState) {
            triggerProcessing();
        }
    }

private:
    // 内部结构体：存储Modbus请求的完整信息
    struct ModbusTransaction
    {
        int serverId;               // Modbus从站地址
        QByteArray requestData;     // Modbus PDU数据（不含从站地址和校验）
        int functionCode;           // Modbus功能码
        QDateTime timestamp;        // 请求入队时间（用于调试和超时判断）
        int retryCount;             // 已重试次数
        int maxRetries;             // 最大重试次数
    };

    // 触发请求处理（非阻塞）
    // 逻辑：根据是否为重试请求，决定是否带延迟触发
    void triggerProcessing()
    {
        if (!processingFlag) { // 仅当当前无处理中的请求时触发
            // 重试请求带延迟（避免高频重试），正常请求立即处理
            QTimer::singleShot(retryPending ? retryDelay : 0, this, &ModbusSequencer::processNextRequest);
        }
    }

    // 清理当前未完成的请求
    // 功能：断开信号连接、释放reply资源、重置currentReply
    void cleanupCurrentReply()
    {
        if (currentReply) {
            // 断开所有信号连接（避免reply被delete后触发无效槽函数）
            disconnect(currentReply, nullptr, this, nullptr);
            currentReply->deleteLater(); // 延迟释放资源（确保当前事件循环结束）
            currentReply = nullptr;      // 重置指针，避免野指针
        }
    }

    // 请求处理核心逻辑（发送前检查连接状态）
    // 步骤：1. 检查连接状态 → 2. 处理重试 → 3. 构造请求 → 4. 发送请求
    void processNextRequestImpl()
    {
        ModbusTransaction trans;
        bool hasRequest = false;
        QModbusDevice::State currentConnState = QModbusDevice::UnconnectedState;

        // 1. 加锁获取请求信息和连接状态（缩小锁范围，减少阻塞）
        {
            QMutexLocker locker(&mutex);

            // 先检查连接状态：仅"已连接"时才继续处理请求
            currentConnState = modbusClient ? modbusClient->state() : QModbusDevice::UnconnectedState;
            if (currentConnState != QModbusDevice::ConnectedState) {
                qWarning() << "Cannot send request: " << stateToString(currentConnState)
                           << ", will retry when connection is restored";
                return; // 连接不正常，直接返回，不处理请求
            }

            // 处理重试请求：更新重试次数和时间戳
            if (retryPending && !requestQueue.isEmpty()) {
                ModbusTransaction &front = requestQueue.head();
                front.retryCount++;
                front.timestamp = QDateTime::currentDateTime();
                retryPending = false;
                qWarning() << "Retrying request (attempt" << front.retryCount << ")";
            }

            // 检查队列状态：若队列为空或存在未完成请求，直接返回
            if (requestQueue.isEmpty() || currentReply) {
                return;
            }

            // 检查最大重试次数：超过则放弃请求，处理下一个
            trans = requestQueue.head();
            if (trans.retryCount >= trans.maxRetries) {
                qCritical() << "Request failed after max retries for server" << trans.serverId;
                requestQueue.dequeue();
                triggerProcessing(); // 触发下一个请求处理
                return;
            }

            hasRequest = true; // 标记存在待处理请求
        }

        // 2. 仅当连接正常且有请求时，执行发送操作
        if (hasRequest && currentConnState == QModbusDevice::ConnectedState) {
            // 构造Modbus请求（无锁操作，减少锁持有时间）
            QModbusRequest request(
                static_cast<QModbusPdu::FunctionCode>(trans.functionCode),
                trans.requestData
            );

            QMutexLocker locker(&mutex);
            // 二次检查连接状态：防止加锁过程中连接断开（并发安全保障）
            if (modbusClient->state() == QModbusDevice::ConnectedState) {
                // 发送原始请求，获取reply对象
                currentReply = modbusClient->sendRawRequest(request, trans.serverId);
            } else {
                qWarning() << "Connection state changed before sending request: " << stateToString(modbusClient->state());
                retryPending = true;
                triggerProcessing();
                return;
            }

            // 配置请求的回调和超时
            if (currentReply) {
                // 连接请求完成信号（处理成功/失败）
                connect(currentReply, &QModbusReply::finished,
                        this, &ModbusSequencer::handleReplyFinished,
                        Qt::DirectConnection);
                // 连接请求错误信号（处理请求级错误，如功能码不支持）
                connect(currentReply, &QModbusReply::errorOccurred,
                        this, &ModbusSequencer::handleReplyError,
                        Qt::DirectConnection);

                // 设置请求超时（1秒，可根据需求调整）
                QTimer::singleShot(1000, this, &ModbusSequencer::handleRequestTimeout);
            } else {
                // 请求创建失败（如客户端未初始化）
                qWarning() << "Failed to create request for server:" << trans.serverId;
                locker.unlock(); // 提前解锁，避免阻塞
                retryPending = true;
                triggerProcessing();
            }
        }
    }

    // 处理成功的请求（解析返回数据）
    // 参数：result - Modbus返回的数据单元（包含寄存器值等）
    void processSuccess(const QModbusDataUnit &result)
    {
        // 示例：打印成功的请求数据（实际项目中可替换为业务逻辑，如存储到变量、更新UI）
        qDebug() << "Request succeeded. Values:" << result.values();
    }

    // 处理单个请求的错误（请求级错误，如功能码不支持、地址越界）
    void handleReplyError(QModbusDevice::Error error)
    {
        QModbusReply *reply = qobject_cast<QModbusReply*>(sender());
        if (reply) {
            qWarning() << "Request error:" << reply->errorString();
        }
    }

signals:
    // 连接状态变化信号（外部可监听）
    // 参数：state - 新的连接状态枚举；stateStr - 对应的英文描述
    void connectionStateChanged(QModbusDevice::State state, const QString &stateStr);

    // 请求因连接问题被延迟的信号（外部可监听以提示用户）
    // 参数：reason - 延迟原因（如"Disconnected"、"Connecting"）
    void requestDelayed(const QString &reason);

private:
    QModbusClient *modbusClient;          // 外部传入的Modbus客户端实例（不负责销毁）
    QModbusReply *currentReply;           // 当前正在处理的请求回复（由sendRawRequest创建）
    QQueue<ModbusTransaction> requestQueue; // 待处理的Modbus请求队列
    mutable QMutex mutex;                 // 保护共享资源的互斥锁（mutable允许const函数修改）
    QAtomicInt processingFlag;            // 防止请求处理重入的原子标志（线程安全）
    bool retryPending;                    // 是否需要重试当前请求的标志
    int retryDelay;                       // 重试延迟（毫秒），默认1秒，错误时翻倍
    QModbusDevice::State currentState;    // 当前连接状态缓存（减少重复调用modbusClient->state()）
};

#endif // MODBUSSEQUENCER_H

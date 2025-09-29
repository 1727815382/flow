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
    // ���캯���������ⲿModbus�ͻ���ʵ������ʼ����Ա����
    // ������client - �ⲿ������Modbus�ͻ��ˣ���QModbusTcpClient/QModbusRtuSerialMaster��
    //       parent - ����������Qt�ڴ����
    explicit ModbusSequencer(QModbusClient *client, QObject *parent = 0)
        : QObject(parent), modbusClient(client), currentReply(nullptr),
          processingFlag(0), retryPending(false), retryDelay(1000),
          currentState(QModbusDevice::UnconnectedState)
    {
        // ����Modbus�豸�����źţ��豸������������ʧ�ܡ���ʱ��
        connect(modbusClient, &QModbusClient::errorOccurred,
                this, &ModbusSequencer::handleModbusError,
                Qt::DirectConnection);

        // ����Modbus����״̬�仯�źţ�ʵʱ�����ڲ�״̬��֪ͨ�ⲿ
        connect(modbusClient, &QModbusClient::stateChanged,
                this, &ModbusSequencer::onStateChanged,
                Qt::DirectConnection);
    }

    // ����������������к�δ������󣬱����ڴ�й©
    ~ModbusSequencer()
    {
        clearQueue();
    }

    // ���Modbusԭʼ�����̰߳�ȫ��
    // ������serverId - Modbus��վ��ַ��1~247��
    //       pduData - Modbus PDU���ݣ�������վ��ַ��CRC/LRC��
    //       functionCode - Modbus�����루��0x03�����ּĴ�����0x06д�����Ĵ�����
    void enqueueRawRequest(int serverId, const QByteArray &pduData, int functionCode)
    {
        QMutexLocker locker(&mutex); // �����������в���

        ModbusTransaction trans;
        trans.serverId = serverId;
        trans.functionCode = functionCode;
        trans.requestData = pduData;
        trans.timestamp = QDateTime::currentDateTime();
        trans.retryCount = 0;       // ��ʼ���Դ���Ϊ0
        trans.maxRetries = 3;       // ������Դ������ɸ������������

        requestQueue.enqueue(trans); // ������������

        triggerProcessing(); // ������������������
    }

    // ���������У��̰߳�ȫ��
    // ���ܣ�������д�����������ֹ��ǰδ�����������״̬��־
    void clearQueue()
    {
        QMutexLocker locker(&mutex);
        requestQueue.clear();       // ��մ��������
        cleanupCurrentReply();      // ����ǰδ��ɵ�����
        processingFlag = 0;         // ���ô����־
        retryPending = false;       // �������Ա�־
        retryDelay = 1000;          // ���������ӳ٣��ָ�Ϊ1�룩
    }

    // ��ȡ��ǰModbus����״̬���̰߳�ȫ��
    // ����ֵ��QModbusDevice::Stateö�٣�Disconnected/Connecting/Connected/Closing��
    QModbusDevice::State getConnectionState() const
    {
        QMutexLocker locker(&mutex);
        // ���ͻ���Ϊ�գ�����"δ����"״̬
        return modbusClient ? modbusClient->state() : QModbusDevice::UnconnectedState;
    }

    // ��̬������������״̬ö��ת��Ϊ�ɶ�Ӣ���ַ���
    // ������state - ����״̬ö��
    // ����ֵ����Ӧ��Ӣ����������"Connected"��"Disconnected"��
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
    // ������������źţ�QModbusReply::finished������
    void handleReplyFinished()
    {
        QModbusReply *reply = qobject_cast<QModbusReply*>(sender());
        if (!reply) return; // ��replyΪ�գ�ֱ�ӷ���

        // ����������������reply��delete���޷����ʣ�
        bool isSuccess = (reply->error() == QModbusDevice::NoError);
        QModbusDataUnit result;
        if (isSuccess) {
            result = reply->result(); // �ɹ�ʱ��ȡ��������
        } else {
            qWarning() << "Request failed:" << reply->errorString(); // ʧ��ʱ��ӡ������Ϣ
        }

        reply->deleteLater(); // �����ͷ�reply��Դ��Qt�ڴ����

        {
            QMutexLocker locker(&mutex);
            // ȷ�ϵ�ǰreply�����ڴ�������󣨷�ֹ������ͻ��
            if (reply == currentReply) {
                currentReply = nullptr;
                // ֻ������ɹ�ʱ���ŴӶ����Ƴ������󣬲����������ӳ�
                if (isSuccess && !requestQueue.isEmpty()) {
                    requestQueue.dequeue();
                    retryDelay = 1000;
                }
            }
        }

        // �ɹ�ʱ���������ݣ�������Ĵ���ֵ��
        if (isSuccess) {
            processSuccess(result);
        }

        triggerProcessing(); // ������һ��������
    }

    // ����Modbus�豸������QModbusClient::errorOccurred������
    // �磺����ʧ�ܡ�ͨ�ų�ʱ���˿�ռ�õ�
    void handleModbusError(QModbusDevice::Error error)
    {
        qWarning() << "Modbus error:" << error << modbusClient->errorString();

        QMutexLocker locker(&mutex);
        // ������δ��ɵ�����������Դ�������Ҫ����
        if (currentReply) {
            cleanupCurrentReply();
            retryPending = true;

            // ���Ӵ���ʱ������ָ���˱ܲ��ԣ������ӳٷ��������5�룩
            if (error == QModbusDevice::ConnectionError) {
                retryDelay = qMin(retryDelay * 2, 5000);
            }
        }
        if (!processingFlag)
            triggerProcessing(); // ������һ�����������������Ե�ǰ����
    }

    // ��������ʱ��1��δ�յ���Ӧʱ������
    void handleRequestTimeout()
    {
        QMutexLocker locker(&mutex);
        // ��鵱ǰ�����Ƿ���δ��ɣ������ظ���������ɵ�����
        if (currentReply && !currentReply->isFinished()) {
            qWarning() << "Request timeout for server:" << currentReply->serverAddress();
            cleanupCurrentReply(); // ����ʱ����
            retryPending = true;   // �����Ҫ����
            retryDelay = qMin(retryDelay * 2, 5000); // ��ʱ�������ӳٷ���
        }

        triggerProcessing(); // ������һ��������
    }

    // ��������ڣ�ԭ�Ӳ�����ֹ���룩
    // ���ܣ�ȷ��ͬһʱ��ֻ��һ���߳�ִ���������߼�
    void processNextRequest()
    {
        // testAndSetAcquire��ԭ�Ӳ�������processingFlagΪ0����Ϊ1������true
        if (processingFlag.testAndSetAcquire(0, 1)) {
            processNextRequestImpl(); // ʵ�ʴ����߼�
            processingFlag = 0;       // �ͷŴ����־
        }
    }

    // ��������״̬�仯��QModbusClient::stateChanged������
    // ���ܣ������ڲ�״̬��֪ͨ�ⲿ�����ӻָ�ʱ����������
    void onStateChanged(QModbusDevice::State newState)
    {
        QMutexLocker locker(&mutex);
        currentState = newState;
        // ����״̬�仯�źţ��ⲿ�ɼ����Ը���UI����ҵ���߼�
        emit connectionStateChanged(newState, stateToString(newState));

        // �����Ӵ�"������"��Ϊ"������"ʱ���Զ����������е�������
        if (newState == QModbusDevice::ConnectedState) {
            triggerProcessing();
        }
    }

private:
    // �ڲ��ṹ�壺�洢Modbus�����������Ϣ
    struct ModbusTransaction
    {
        int serverId;               // Modbus��վ��ַ
        QByteArray requestData;     // Modbus PDU���ݣ�������վ��ַ��У�飩
        int functionCode;           // Modbus������
        QDateTime timestamp;        // �������ʱ�䣨���ڵ��Ժͳ�ʱ�жϣ�
        int retryCount;             // �����Դ���
        int maxRetries;             // ������Դ���
    };

    // ������������������
    // �߼��������Ƿ�Ϊ�������󣬾����Ƿ���ӳٴ���
    void triggerProcessing()
    {
        if (!processingFlag) { // ������ǰ�޴����е�����ʱ����
            // ����������ӳ٣������Ƶ���ԣ�������������������
            QTimer::singleShot(retryPending ? retryDelay : 0, this, &ModbusSequencer::processNextRequest);
        }
    }

    // ����ǰδ��ɵ�����
    // ���ܣ��Ͽ��ź����ӡ��ͷ�reply��Դ������currentReply
    void cleanupCurrentReply()
    {
        if (currentReply) {
            // �Ͽ������ź����ӣ�����reply��delete�󴥷���Ч�ۺ�����
            disconnect(currentReply, nullptr, this, nullptr);
            currentReply->deleteLater(); // �ӳ��ͷ���Դ��ȷ����ǰ�¼�ѭ��������
            currentReply = nullptr;      // ����ָ�룬����Ұָ��
        }
    }

    // ����������߼�������ǰ�������״̬��
    // ���裺1. �������״̬ �� 2. �������� �� 3. �������� �� 4. ��������
    void processNextRequestImpl()
    {
        ModbusTransaction trans;
        bool hasRequest = false;
        QModbusDevice::State currentConnState = QModbusDevice::UnconnectedState;

        // 1. ������ȡ������Ϣ������״̬����С����Χ������������
        {
            QMutexLocker locker(&mutex);

            // �ȼ������״̬����"������"ʱ�ż�����������
            currentConnState = modbusClient ? modbusClient->state() : QModbusDevice::UnconnectedState;
            if (currentConnState != QModbusDevice::ConnectedState) {
                qWarning() << "Cannot send request: " << stateToString(currentConnState)
                           << ", will retry when connection is restored";
                return; // ���Ӳ�������ֱ�ӷ��أ�����������
            }

            // �����������󣺸������Դ�����ʱ���
            if (retryPending && !requestQueue.isEmpty()) {
                ModbusTransaction &front = requestQueue.head();
                front.retryCount++;
                front.timestamp = QDateTime::currentDateTime();
                retryPending = false;
                qWarning() << "Retrying request (attempt" << front.retryCount << ")";
            }

            // ������״̬��������Ϊ�ջ����δ�������ֱ�ӷ���
            if (requestQueue.isEmpty() || currentReply) {
                return;
            }

            // ���������Դ�����������������󣬴�����һ��
            trans = requestQueue.head();
            if (trans.retryCount >= trans.maxRetries) {
                qCritical() << "Request failed after max retries for server" << trans.serverId;
                requestQueue.dequeue();
                triggerProcessing(); // ������һ��������
                return;
            }

            hasRequest = true; // ��Ǵ��ڴ���������
        }

        // 2. ��������������������ʱ��ִ�з��Ͳ���
        if (hasRequest && currentConnState == QModbusDevice::ConnectedState) {
            // ����Modbus������������������������ʱ�䣩
            QModbusRequest request(
                static_cast<QModbusPdu::FunctionCode>(trans.functionCode),
                trans.requestData
            );

            QMutexLocker locker(&mutex);
            // ���μ������״̬����ֹ�������������ӶϿ���������ȫ���ϣ�
            if (modbusClient->state() == QModbusDevice::ConnectedState) {
                // ����ԭʼ���󣬻�ȡreply����
                currentReply = modbusClient->sendRawRequest(request, trans.serverId);
            } else {
                qWarning() << "Connection state changed before sending request: " << stateToString(modbusClient->state());
                retryPending = true;
                triggerProcessing();
                return;
            }

            // ��������Ļص��ͳ�ʱ
            if (currentReply) {
                // ������������źţ�����ɹ�/ʧ�ܣ�
                connect(currentReply, &QModbusReply::finished,
                        this, &ModbusSequencer::handleReplyFinished,
                        Qt::DirectConnection);
                // ������������źţ��������󼶴����繦���벻֧�֣�
                connect(currentReply, &QModbusReply::errorOccurred,
                        this, &ModbusSequencer::handleReplyError,
                        Qt::DirectConnection);

                // ��������ʱ��1�룬�ɸ������������
                QTimer::singleShot(1000, this, &ModbusSequencer::handleRequestTimeout);
            } else {
                // ���󴴽�ʧ�ܣ���ͻ���δ��ʼ����
                qWarning() << "Failed to create request for server:" << trans.serverId;
                locker.unlock(); // ��ǰ��������������
                retryPending = true;
                triggerProcessing();
            }
        }
    }

    // ����ɹ������󣨽����������ݣ�
    // ������result - Modbus���ص����ݵ�Ԫ�������Ĵ���ֵ�ȣ�
    void processSuccess(const QModbusDataUnit &result)
    {
        // ʾ������ӡ�ɹ����������ݣ�ʵ����Ŀ�п��滻Ϊҵ���߼�����洢������������UI��
        qDebug() << "Request succeeded. Values:" << result.values();
    }

    // ����������Ĵ������󼶴����繦���벻֧�֡���ַԽ�磩
    void handleReplyError(QModbusDevice::Error error)
    {
        QModbusReply *reply = qobject_cast<QModbusReply*>(sender());
        if (reply) {
            qWarning() << "Request error:" << reply->errorString();
        }
    }

signals:
    // ����״̬�仯�źţ��ⲿ�ɼ�����
    // ������state - �µ�����״̬ö�٣�stateStr - ��Ӧ��Ӣ������
    void connectionStateChanged(QModbusDevice::State state, const QString &stateStr);

    // �������������ⱻ�ӳٵ��źţ��ⲿ�ɼ�������ʾ�û���
    // ������reason - �ӳ�ԭ����"Disconnected"��"Connecting"��
    void requestDelayed(const QString &reason);

private:
    QModbusClient *modbusClient;          // �ⲿ�����Modbus�ͻ���ʵ�������������٣�
    QModbusReply *currentReply;           // ��ǰ���ڴ��������ظ�����sendRawRequest������
    QQueue<ModbusTransaction> requestQueue; // �������Modbus�������
    mutable QMutex mutex;                 // ����������Դ�Ļ�������mutable����const�����޸ģ�
    QAtomicInt processingFlag;            // ��ֹ�����������ԭ�ӱ�־���̰߳�ȫ��
    bool retryPending;                    // �Ƿ���Ҫ���Ե�ǰ����ı�־
    int retryDelay;                       // �����ӳ٣����룩��Ĭ��1�룬����ʱ����
    QModbusDevice::State currentState;    // ��ǰ����״̬���棨�����ظ�����modbusClient->state()��
};

#endif // MODBUSSEQUENCER_H

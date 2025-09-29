#ifndef SEQUENCEQUEUE_H
#define SEQUENCEQUEUE_H

#include <QModbusClient>
#include <QMutexLocker>
#include <QQueue>
#include <QDateTime>

struct ModbusTransaction
{
    int serverId;
    QByteArray requestData;
    int functionCode;
    QDateTime timestamp;
    int retryCount;
    int maxRetries;
};
// 线程安全的 Modbus 事务队列
class ThreadSafeTransactionQueue
{
public:
    void enqueue(const ModbusTransaction &trans)
    {
        QMutexLocker locker(&mutex);
        queue.enqueue(trans);
    }

    bool dequeue(ModbusTransaction &trans)
    {
        QMutexLocker locker(&mutex);
        if (queue.isEmpty()) return false;
        trans = queue.dequeue();
        return true;
    }

    bool head(ModbusTransaction &trans)
    {
        QMutexLocker locker(&mutex);
        if (queue.isEmpty()) return false;
        trans = queue.head();
        return true;
    }

    bool updateHead(const ModbusTransaction &trans)
    {
        QMutexLocker locker(&mutex);
        if (queue.isEmpty()) return false;
        queue.head() = trans;
        return true;
    }

    bool isEmpty()
    {
        QMutexLocker locker(&mutex);
        return queue.isEmpty();
    }

    void clear()
    {
        QMutexLocker locker(&mutex);
        queue.clear();
    }

private:
    QQueue<ModbusTransaction> queue;
    QMutex mutex;
};

// 线程安全的 Modbus 回复包装器
class ThreadSafeReply : public QObject
{
    Q_OBJECT
public:
    ThreadSafeReply(QModbusReply *reply, QObject *parent = nullptr)
        : QObject(parent), reply(reply), isFinish(false)
    {
        connect(reply, &QModbusReply::finished, this, &ThreadSafeReply::handleFinished);
    }

    ~ThreadSafeReply()
    {
        if (reply) {
            reply->deleteLater();
        }
    }

    bool isFinished()
    {
        QMutexLocker locker(&mutex);
        return isFinish;
    }

    QModbusDevice::Error error()
    {
        QMutexLocker locker(&mutex);
        return reply ? reply->error() : QModbusDevice::UnknownError;
    }

    QString errorString()
    {
        QMutexLocker locker(&mutex);
        return reply ? reply->errorString() : QString();
    }

    QModbusDataUnit result()
    {
        QMutexLocker locker(&mutex);
        return reply ? reply->result() : QModbusDataUnit();
    }

    void abort()
    {
        QMutexLocker locker(&mutex);
        if (reply && !reply->isFinished()) {
            reply->deleteLater();
        }
    }

signals:
    void finished();

private slots:
    void handleFinished()
    {
        QMutexLocker locker(&mutex);
        isFinish = true;
        emit this->finished();
    }

private:
    QModbusReply *reply;
    bool isFinish;
    QMutex mutex;
};

#endif // SEQUENCEQUEUE_H

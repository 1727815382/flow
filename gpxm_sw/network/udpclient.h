#ifndef UDPCLIENT_H
#define UDPCLIENT_H


#include <QCoreApplication>
#include <QUdpSocket>
#include <QHostAddress>
#include <QDebug>
#include <QTimer>

class UdpClient : public QObject
{
    Q_OBJECT
public:
    explicit UdpClient(QObject *parent = nullptr) : QObject(parent)
    {
        // 创建socket并绑定本地地址
        socket = new QUdpSocket(this);

        // 绑定本地地址和端口
        if (!socket->bind(QHostAddress("192.168.1.200"), 8001))
        {
            qDebug() << "udp error:" << socket->errorString();
        }
        else
        {
            qDebug() << "udp client success: 192.168.1.201:8001";
            qDebug() << "udp server success: 192.168.1.1:4001";
        }

        // 连接接收信号
        connect(socket, &QUdpSocket::readyRead, this, &UdpClient::receiveData);
    }

    // 发送数据到服务器
    void send(char* data, int len)
    {
        // 准备数据
//        QByteArray datagram = message.toUtf8();

        // 发送到服务器
        qint64 bytesSent = socket->writeDatagram(
            data,
            len,
            QHostAddress("192.168.1.10"),
            4001
        );

        if (bytesSent == -1)
        {
            qDebug() << "send faild:" << socket->errorString();
        }
        else
        {
//            qDebug() << "发送成功(" << bytesSent << "字节):" << message;
        }
    }

signals:

    void signUdpRecvd (QByteArray data);

private slots:
    // 接收数据
    void receiveData()
    {
        while (socket->hasPendingDatagrams())
        {
            QByteArray datagram;
            datagram.resize(socket->pendingDatagramSize());
            QHostAddress senderAddress;
            quint16 senderPort;

            // 读取数据
            qint64 bytesRead = socket->readDatagram(
                datagram.data(),
                datagram.size(),
                &senderAddress,
                &senderPort
            );

            if (bytesRead == -1)
            {
                qDebug() << "接收失败:" << socket->errorString();
            }
            else
            {
                QString message = QString::fromLocal8Bit(datagram);
                qDebug().nospace() << "接收["
                    << senderAddress.toString() << ":"
                    << senderPort << "]: "
                    << message;

                emit signUdpRecvd(datagram);
            }
        }
    }

private:
    QUdpSocket *socket;
};
#endif // UDPCLIENT_H

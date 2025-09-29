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
        // ����socket���󶨱��ص�ַ
        socket = new QUdpSocket(this);

        // �󶨱��ص�ַ�Ͷ˿�
        if (!socket->bind(QHostAddress("192.168.1.200"), 8001))
        {
            qDebug() << "udp error:" << socket->errorString();
        }
        else
        {
            qDebug() << "udp client success: 192.168.1.201:8001";
            qDebug() << "udp server success: 192.168.1.1:4001";
        }

        // ���ӽ����ź�
        connect(socket, &QUdpSocket::readyRead, this, &UdpClient::receiveData);
    }

    // �������ݵ�������
    void send(char* data, int len)
    {
        // ׼������
//        QByteArray datagram = message.toUtf8();

        // ���͵�������
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
//            qDebug() << "���ͳɹ�(" << bytesSent << "�ֽ�):" << message;
        }
    }

signals:

    void signUdpRecvd (QByteArray data);

private slots:
    // ��������
    void receiveData()
    {
        while (socket->hasPendingDatagrams())
        {
            QByteArray datagram;
            datagram.resize(socket->pendingDatagramSize());
            QHostAddress senderAddress;
            quint16 senderPort;

            // ��ȡ����
            qint64 bytesRead = socket->readDatagram(
                datagram.data(),
                datagram.size(),
                &senderAddress,
                &senderPort
            );

            if (bytesRead == -1)
            {
                qDebug() << "����ʧ��:" << socket->errorString();
            }
            else
            {
                QString message = QString::fromLocal8Bit(datagram);
                qDebug().nospace() << "����["
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

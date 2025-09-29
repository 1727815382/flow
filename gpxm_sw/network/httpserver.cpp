#include "httpserver.h"

#include <QtNetwork/QTcpServer>
#include <QtNetwork/QTcpSocket>
#include <QString>
#include <QByteArray>
#include <QTextStream>
#include <QDebug>


// �����µ����ӵ���ʱ������ô˺���
void HttpServer::incomingConnection(qintptr handle)
{
    // ����һ���µ��׽����������������
    QTcpSocket *socket = new QTcpSocket(this);
    if (!socket->setSocketDescriptor(handle))
    {
        qDebug() << "Failed to set socket descriptor";
        delete socket;
        return;
    }

    // �����źţ����׽����пɶ�����ʱ����readRequest�ۺ���
    connect(socket, &QTcpSocket::readyRead, this, [socket, this]()
    {
        readRequest(socket);
    });
    // �����źţ����׽��ֶϿ�����ʱɾ���׽��ֶ���
    connect(socket, &QTcpSocket::disconnected, socket, &QTcpSocket::deleteLater);
}

void HttpServer::readRequest(QTcpSocket *socket)
{
    // ��ȡ�ͻ��˷��͵���������
    QByteArray requestData = socket->readAll();
    // ����HTTP����
    QString request = QString::fromUtf8(requestData);
    qDebug() << "Request:" << request;

    // ����Ƿ�ΪPOST����
    if (request.startsWith("POST"))
    {
        // ��ȡPOST����
        QString postData = extractPostData(request);
        qDebug() << "POST Data:" << postData;
        emit signMessageSent(postData);

        // ������Ӧ
        QString response = "HTTP/1.1 200 OK\r\n";
        response += "Content-Type: application/octet-stream; charset=UTF-8\r\n";
        response += "\r\n";
        response += "<html><body><h1>POST request received successfully!</h1></body></html>";

        // ������Ӧ���ͻ���
        socket->write(response.toUtf8());
    }
    else
    {
        // ��POST�������Ӧ
        QString response = "HTTP/1.1 405 Method Not Allowed\r\n";
        response += "Content-Type: application/octet-stream; charset=UTF-8\r\n";
        response += "\r\n";
        response += "<html><body><h1>405 Method Not Allowed</h1></body></html>";
        socket->write(response.toUtf8());
    }

    // �ر��׽���
    socket->disconnectFromHost();
}

// ��ȡPOST�����е�����
QString HttpServer::extractPostData(const QString &request)
{
    int bodyIndex = request.indexOf("\r\n\r\n");
    if (bodyIndex != -1)
    {
        // ��ȡ�����岿��
        return request.mid(bodyIndex  + 4);
    }
    return "";
}


#include "httpserver.h"

#include <QtNetwork/QTcpServer>
#include <QtNetwork/QTcpSocket>
#include <QString>
#include <QByteArray>
#include <QTextStream>
#include <QDebug>


// 当有新的连接到来时，会调用此函数
void HttpServer::incomingConnection(qintptr handle)
{
    // 创建一个新的套接字来处理这个连接
    QTcpSocket *socket = new QTcpSocket(this);
    if (!socket->setSocketDescriptor(handle))
    {
        qDebug() << "Failed to set socket descriptor";
        delete socket;
        return;
    }

    // 连接信号，当套接字有可读数据时调用readRequest槽函数
    connect(socket, &QTcpSocket::readyRead, this, [socket, this]()
    {
        readRequest(socket);
    });
    // 连接信号，当套接字断开连接时删除套接字对象
    connect(socket, &QTcpSocket::disconnected, socket, &QTcpSocket::deleteLater);
}

void HttpServer::readRequest(QTcpSocket *socket)
{
    // 读取客户端发送的所有数据
    QByteArray requestData = socket->readAll();
    // 解析HTTP请求
    QString request = QString::fromUtf8(requestData);
    qDebug() << "Request:" << request;

    // 检查是否为POST请求
    if (request.startsWith("POST"))
    {
        // 提取POST数据
        QString postData = extractPostData(request);
        qDebug() << "POST Data:" << postData;
        emit signMessageSent(postData);

        // 构建响应
        QString response = "HTTP/1.1 200 OK\r\n";
        response += "Content-Type: application/octet-stream; charset=UTF-8\r\n";
        response += "\r\n";
        response += "<html><body><h1>POST request received successfully!</h1></body></html>";

        // 发送响应给客户端
        socket->write(response.toUtf8());
    }
    else
    {
        // 非POST请求的响应
        QString response = "HTTP/1.1 405 Method Not Allowed\r\n";
        response += "Content-Type: application/octet-stream; charset=UTF-8\r\n";
        response += "\r\n";
        response += "<html><body><h1>405 Method Not Allowed</h1></body></html>";
        socket->write(response.toUtf8());
    }

    // 关闭套接字
    socket->disconnectFromHost();
}

// 提取POST请求中的数据
QString HttpServer::extractPostData(const QString &request)
{
    int bodyIndex = request.indexOf("\r\n\r\n");
    if (bodyIndex != -1)
    {
        // 获取请求体部分
        return request.mid(bodyIndex  + 4);
    }
    return "";
}


#ifndef HTTPSERVER_H
#define HTTPSERVER_H

#include <QtNetwork/QTcpServer>
#include <QtNetwork/QTcpSocket>

// 自定义的HTTP服务器类，继承自QTcpServer
class HttpServer : public QTcpServer
{
    Q_OBJECT
public:
    HttpServer(QObject *parent = nullptr) : QTcpServer(parent) {}

protected:
    // 当有新的连接到来时，会调用此函数
    void incomingConnection(qintptr handle) override;

signals:
    // 自定义信号
    void signMessageSent(const QString &message);

private slots:

    void readRequest(QTcpSocket *socket);

private:
    // 提取POST请求中的数据
    QString extractPostData(const QString &request);
};
#endif // HTTPSERVER_H

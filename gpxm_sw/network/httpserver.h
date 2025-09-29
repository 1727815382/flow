#ifndef HTTPSERVER_H
#define HTTPSERVER_H

#include <QtNetwork/QTcpServer>
#include <QtNetwork/QTcpSocket>

// �Զ����HTTP�������࣬�̳���QTcpServer
class HttpServer : public QTcpServer
{
    Q_OBJECT
public:
    HttpServer(QObject *parent = nullptr) : QTcpServer(parent) {}

protected:
    // �����µ����ӵ���ʱ������ô˺���
    void incomingConnection(qintptr handle) override;

signals:
    // �Զ����ź�
    void signMessageSent(const QString &message);

private slots:

    void readRequest(QTcpSocket *socket);

private:
    // ��ȡPOST�����е�����
    QString extractPostData(const QString &request);
};
#endif // HTTPSERVER_H

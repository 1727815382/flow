// modules/net/udp_client_task.hpp
#pragma once
#include <QObject>
#include <QHostAddress>
#include <QUdpSocket>
#include <QJsonObject>
#include "core/message_bus.hpp"
#include "core/message.hpp"

// 若你们有 app1.0 自带 udp_task，可在编译选项里 -DAPP10_HAS_UDP_TASK
#ifdef APP10_HAS_UDP_TASK
#include "core/udp_task.hpp"
class UdpClientTask : public UdpTask {
    Q_OBJECT
public:
    using UdpTask::UdpTask; // 直接复用基类能力
    UdpClientTask(){ MessageBus::instance().subscribe([this](Message& m){ onMessage(m); }); }
private:
    void onMessage(Message& m){
        const auto& t=m.header.type; const auto p=m.payload;
        if(t=="udp.bind"){ bind(p.value("port").toInt(), QHostAddress(p.value("addr").toString("0.0.0.0"))); publish("udp.bound", p); }
        else if(t=="udp.send"){ send(p.value("data").toString().toUtf8(), QHostAddress(p.value("addr").toString()), quint16(p.value("port").toInt())); }
        else if(t=="udp.close"){ close(); publish("udp.closed", {}); }
    }
    void onDatagram(const QByteArray& d, const QHostAddress& a, quint16 port) override {
        publish("udp.recv", QJsonObject{{"from", a.toString()}, {"port", int(port)}, {"data", QString::fromUtf8(d)}} );
    }
    void publish(const std::string& t, const QJsonObject& j){ Message m; m.header.type=t; m.payload=j; MessageBus::instance().post(m); }
};
#else
class UdpClientTask : public QObject {
    Q_OBJECT
public:
    explicit UdpClientTask(QObject* parent=nullptr): QObject(parent), _sock(new QUdpSocket(this)){
        MessageBus::instance().subscribe([this](Message& m){ onMessage(m); });
        connect(_sock, &QUdpSocket::readyRead, this, [this]{
            while(_sock->hasPendingDatagrams()){
                QHostAddress a; quint16 p=0; QByteArray d; d.resize(int(_sock->pendingDatagramSize()));
                _sock->readDatagram(d.data(), d.size(), &a, &p);
                publish("udp.recv", QJsonObject{{"from", a.toString()}, {"port", int(p)}, {"data", QString::fromUtf8(d)}});
            }
        });
    }
private:
    void onMessage(Message& m){
        const auto& t=m.header.type; const auto p=m.payload;
        if(t=="udp.bind"){
            QHostAddress addr(p.value("addr").toString("0.0.0.0")); quint16 port=p.value("port").toInt();
            bool ok=_sock->bind(addr, port, QUdpSocket::ShareAddress|QUdpSocket::ReuseAddressHint);
            publish(ok? "udp.bound":"udp.error", QJsonObject{{"addr", addr.toString()}, {"port", int(port)}} );
        }else if(t=="udp.send"){
            auto data = p.value("data").toString().toUtf8();
            QHostAddress to(p.value("addr").toString()); quint16 port=p.value("port").toInt();
            _sock->writeDatagram(data, to, port);
        }else if(t=="udp.close"){
            _sock->close(); publish("udp.closed", {});
        }
    }
    void publish(const std::string& t, const QJsonObject& j){ Message m; m.header.type=t; m.payload=j; MessageBus::instance().post(m); }

private:
    QUdpSocket* _sock{};
};
#endif

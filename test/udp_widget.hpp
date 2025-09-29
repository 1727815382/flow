#pragma once
#include <QWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QHBoxLayout>
#include <QLabel>
#include "core/message_bus.hpp"

class UdpWidget : public QWidget {
    Q_OBJECT
public:
    UdpWidget(QWidget* p=nullptr):QWidget(p){
        auto* lay=new QVBoxLayout(this);
        auto* l1=new QHBoxLayout; _bindAddr=new QLineEdit("0.0.0.0"); _bindPort=new QLineEdit("9000"); auto* bBind=new QPushButton("绑定");
        l1->addWidget(_bindAddr); l1->addWidget(_bindPort); l1->addWidget(bBind);
        auto* l2=new QHBoxLayout; _to=new QLineEdit("127.0.0.1"); _toPort=new QLineEdit("9000"); _hex=new QLineEdit("010203"); auto* bSend=new QPushButton("发送");
        l2->addWidget(_to); l2->addWidget(_toPort); l2->addWidget(_hex); l2->addWidget(bSend);
        _log=new QLabel(this); _log->setWordWrap(true);
        lay->addLayout(l1); lay->addLayout(l2); lay->addWidget(_log);

        connect(bBind,&QPushButton::clicked,this,[this]{
            Message m; m.header.type="udp.bind";
            m.payload=QJsonObject{{"addr",_bindAddr->text()},{"port",_bindPort->text().toInt()}};
            MessageBus::instance().post(m);
        });
        connect(bSend,&QPushButton::clicked,this,[this]{
            Message m; m.header.type="udp.send";
            m.payload=QJsonObject{{"addr",_to->text()},{"port",_toPort->text().toInt()},{"hex",_hex->text()}};
            MessageBus::instance().post(m);
        });

        MessageBus::instance().subscribe([this](Message& m){
            if(m.header.type=="udp.received"){
                _log->setText(QString("收:%1:%2 %3")
                              .arg(m.payload.value("from").toString())
                              .arg(m.payload.value("port").toInt())
                              .arg(m.payload.value("hex").toString()));
            }
        });
    }
private:
    QLineEdit *_bindAddr{},*_bindPort{},*_to{},*_toPort{},*_hex{}; QLabel* _log{};
};

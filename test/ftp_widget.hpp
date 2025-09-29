#pragma once
#include <QWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QHBoxLayout>
#include <QLabel>
#include "core/message_bus.hpp"

class FtpWidget : public QWidget {
    Q_OBJECT
public:
    FtpWidget(QWidget* p=nullptr):QWidget(p){
        auto* lay=new QHBoxLayout(this);
        _url=new QLineEdit(this); _url->setPlaceholderText("ftp://user:pwd@host:21/path/file");
        _file=new QLineEdit(this); _file->setPlaceholderText("本地文件路径");
        auto* put=new QPushButton("上传",this);
        _status=new QLabel(this);
        lay->addWidget(_url); lay->addWidget(_file); lay->addWidget(put); lay->addWidget(_status);
        connect(put,&QPushButton::clicked,this,[this]{ doPut(); });
        MessageBus::instance().subscribe([this](Message& m){
            if(m.header.type=="ftp.done") _status->setText("完成");
            if(m.header.type=="ftp.error") _status->setText("错误");
        });
    }
private:
    QLineEdit *_url{}, *_file{}; QLabel* _status{};
    void doPut(){
        Message m; m.header.type="ftp.put";
        m.payload = QJsonObject{{"url",_url->text()},{"file",_file->text()}};
        MessageBus::instance().post(m);
    }
};

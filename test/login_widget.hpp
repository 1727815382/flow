#pragma once
#include <QWidget>
#include <QVBoxLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>

class LoginWidget : public QWidget {
    Q_OBJECT
public:
    explicit LoginWidget(QWidget* p=nullptr):QWidget(p){
        auto* lay=new QVBoxLayout(this);
        auto* title=new QLabel("GPXM 登录",this);
        auto* user =new QLineEdit(this); user->setPlaceholderText("用户名");
        auto* pass =new QLineEdit(this); pass->setPlaceholderText("密码"); pass->setEchoMode(QLineEdit::Password);
        auto* btn  =new QPushButton("登录",this);
        lay->addWidget(title); lay->addWidget(user); lay->addWidget(pass); lay->addWidget(btn); lay->addStretch();
        connect(btn,&QPushButton::clicked,this,[this]{ emit loggedIn(); });
    }
signals: void loggedIn();
};

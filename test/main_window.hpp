#pragma once
#include <QMainWindow>
#include <QStackedWidget>
#include <QToolBar>
#include "login_widget.hpp"
#include "work_widget.hpp"
#include "system_config_widget.hpp"
#include "ftp_widget.hpp"
#include "udp_widget.hpp"
#include "measure_widget.hpp"

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit MainWindow(QWidget* p=nullptr):QMainWindow(p){
        _stack=new QStackedWidget(this);
        setCentralWidget(_stack);
        auto* tb=addToolBar("nav");
        auto mk=[&](const QString& t,int idx){ auto* a=tb->addAction(t); connect(a,&QAction::triggered,this,[this,idx]{ _stack->setCurrentIndex(idx); }); };

        _login=new LoginWidget(this);
        _work =new WorkWidget(this);
        _sys  =new SystemConfigWidget(this);
        _ftp  =new FtpWidget(this);
        _udp  =new UdpWidget(this);
        _meas =new MeasureWidget(this);

        _stack->addWidget(_login); //0
        _stack->addWidget(_work);  //1
        _stack->addWidget(_sys);   //2
        _stack->addWidget(_ftp);   //3
        _stack->addWidget(_udp);   //4
        _stack->addWidget(_meas);  //5

        mk("工作",1); mk("系统",2); mk("FTP",3); mk("UDP",4); mk("测量",5);

        connect(_login,&LoginWidget::loggedIn,this,[this]{ _stack->setCurrentIndex(1); });
    }
private:
    QStackedWidget* _stack{};
    LoginWidget *_login{}; WorkWidget *_work{}; SystemConfigWidget* _sys{};
    FtpWidget* _ftp{}; UdpWidget* _udp{}; MeasureWidget* _meas{};
};

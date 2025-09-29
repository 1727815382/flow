#pragma once
#include <QWidget>
#include <QTableWidget>
#include <QPushButton>
#include <QFileDialog>
#include "core/message_bus.hpp"

class MeasureWidget : public QWidget {
    Q_OBJECT
public:
    MeasureWidget(QWidget* p=nullptr):QWidget(p){
        auto* lay=new QVBoxLayout(this);
        _tbl=new QTableWidget(this); _tbl->setColumnCount(4);
        _tbl->setHorizontalHeaderLabels({"ID","里程","轨距偏差","调整"});
        auto* bar=new QHBoxLayout();
        auto* bAdd=new QPushButton("添加"); auto* bChk=new QPushButton("校验");
        auto* bSav=new QPushButton("保存CSV"); auto* bLd=new QPushButton("打开CSV");
        bar->addWidget(bAdd); bar->addWidget(bChk); bar->addWidget(bSav); bar->addWidget(bLd);
        lay->addLayout(bar); lay->addWidget(_tbl);

        connect(bAdd,&QPushButton::clicked,this,[this]{
            Message m; m.header.type="measure.add";
            m.payload=QJsonObject{{"id",QString::number(_tbl->rowCount()+1)},{"mile","0"},{"gauge","0"}};
            MessageBus::instance().post(m);
        });
        connect(bChk,&QPushButton::clicked,this,[]{ Message m; m.header.type="measure.check"; MessageBus::instance().post(m); });
        connect(bSav,&QPushButton::clicked,this,[this]{
            const auto p=QFileDialog::getSaveFileName(this,"保存",".","*.csv");
            if(p.isEmpty()) return; Message m; m.header.type="measure.file.save"; m.payload=QJsonObject{{"path",p}}; MessageBus::instance().post(m);
        });
        connect(bLd,&QPushButton::clicked,this,[this]{
            const auto p=QFileDialog::getOpenFileName(this,"打开",".","*.csv");
            if(p.isEmpty()) return; Message m; m.header.type="measure.file.load"; m.payload=QJsonObject{{"path",p}}; MessageBus::instance().post(m);
        });

        MessageBus::instance().subscribe([this](Message& m){
            if(m.header.type=="measure.dataset"){
                auto a=m.payload.value("rows").toArray(); _tbl->setRowCount(a.size());
                for(int i=0;i<a.size();++i){ auto o=a[i].toObject();
                    setItem(i,0,o.value("id").toString());
                    setItem(i,1,o.value("mile").toString());
                    setItem(i,2,o.value("gauge").toString());
                    setItem(i,3,o.value("adj").toString());
                }
            }
        });

        Message mm; mm.header.type="measure.request"; MessageBus::instance().post(mm);
    }
private:
    QTableWidget* _tbl{};
    void setItem(int r,int c,const QString& s){
        if(!_tbl->item(r,c)) _tbl->setItem(r,c,new QTableWidgetItem);
        _tbl->item(r,c)->setText(s);
    }
};

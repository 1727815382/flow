#pragma once
#include <QWidget>
#include <QTableWidget>
#include <QHBoxLayout>
#include <QPushButton>
#include <QLabel>
#include "core/message_bus.hpp"

class WorkWidget : public QWidget {
    Q_OBJECT
public:
    explicit WorkWidget(QWidget* p=nullptr):QWidget(p){
        auto* lay=new QVBoxLayout(this);
        _tbl = new QTableWidget(this);
        _tbl->setColumnCount(8);
        _tbl->setHorizontalHeaderLabels({"PLC","PLC-ID","MOTOR","MOTOR-ID","RUN","ALM","HOMED","LEG"});
        _log = new QLabel(this); _log->setWordWrap(true);
        auto* bar=new QHBoxLayout();
        auto addBtn=[&](const QString& text, const QString& op){
            auto* b=new QPushButton(text,this);
            connect(b,&QPushButton::clicked,this,[this,op]{ sendCmd(op); });
            bar->addWidget(b);
        };
        addBtn("正转","fwd"); addBtn("反转","rev"); addBtn("停止","stop");
        addBtn("回零","home"); addBtn("压实","leg_press"); addBtn("回收","leg_retract");
        lay->addLayout(bar); lay->addWidget(_tbl); lay->addWidget(_log);

        MessageBus::instance().subscribe([this](Message& m){
            if(m.header.type=="plc.snapshot") onSnapshot(m.payload);
            else if(m.header.type=="motor.job.status") onJob(m.payload);
            else if(m.header.type=="ui.controls.lock")  setEnabled(false);
            else if(m.header.type=="ui.controls.unlock")setEnabled(true);
        });
    }

private:
    QTableWidget* _tbl{}; QLabel* _log{};
    void onSnapshot(const QJsonObject& p){
        const auto rows=p.value("rows").toArray();
        _tbl->setRowCount(rows.size());
        for(int i=0;i<rows.size();++i){
            auto o=rows[i].toObject();
            setItem(i,0,QString::number(o.value("plc").toInt()));
            setItem(i,1,o.value("plc_id").toString());
            setItem(i,2,QString::number(o.value("motor").toInt()));
            setItem(i,3,o.value("motor_id").toString());
            setItem(i,4,o.value("run").toString());
            setItem(i,5,o.value("alarm").toString());
            setItem(i,6,o.value("homed").toBool()? "Y":"N");
            setItem(i,7,o.value("leg_retracted").toBool()? "Y":"N");
        }
    }
    void setItem(int r,int c,const QString& s){
        if(!_tbl->item(r,c)) _tbl->setItem(r,c,new QTableWidgetItem);
        _tbl->item(r,c)->setText(s);
    }
    void onJob(const QJsonObject& p){
        const auto phase=p.value("phase").toString();
        const auto op=p.value("op").toString();
        _log->setText(QString("作业[%1]：%2").arg(op,phase));
    }
    void sendCmd(const QString& op){
        // 选中行 → 目标
        QList<int> rows; for(auto* it:_tbl->selectedItems()) rows<<it->row();
        rows.removeDuplicates();
        QJsonArray targets;
        for(int r: rows){
            int plc = _tbl->item(r,0)->text().toInt()-1;
            int mot = _tbl->item(r,2)->text().toInt()-1;
            targets.push_back(QJsonObject{{"plc_index",plc},{"motor_index",mot}});
        }
        if(targets.isEmpty()) return;
        Message m; m.header.type="motor.ctrl";
        m.payload = QJsonObject{{"op", op},{"scope","single"},{"targets",targets}};
        MessageBus::instance().post(m);
    }
};

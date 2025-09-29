// multi_plc_widget.hpp
#pragma once
#include <QWidget>
#include <QTableWidget>
#include <QGroupBox>
#include <QSpinBox>
#include <QLineEdit>
#include <QPushButton>
#include <QGridLayout>
#include <QHeaderView>
#include <QJsonArray>
#include "ui_bus.hpp"

class MultiPlcWidget : public QWidget {
    Q_OBJECT
public:
    explicit MultiPlcWidget(QWidget* parent=nullptr): QWidget(parent){
        auto* root = new QVBoxLayout(this);

        // 状态表（20 PLC * 12 电机 = 240 行）
        _tbl = new QTableWidget; _tbl->setColumnCount(10);
        _tbl->setHorizontalHeaderLabels({QStringLiteral("PLC#"), QStringLiteral("PLC ID"), QStringLiteral("电机#"), QStringLiteral("电机ID"),
                                         QStringLiteral("PLC状态"), QStringLiteral("报警"), QStringLiteral("模式"), QStringLiteral("运行"),
                                         QStringLiteral("电机状态"), QStringLiteral("转速/位置")});
        _tbl->horizontalHeader()->setStretchLastSection(true);
        _tbl->verticalHeader()->setVisible(false);
        _tbl->setEditTriggers(QAbstractItemView::NoEditTriggers);
        root->addWidget(_tbl, 7);

        // 控制台
        auto* gb = new QGroupBox(QStringLiteral("控制台"));
        auto* g = new QGridLayout(gb);
        _plcIdx = new QSpinBox; _plcIdx->setRange(1, 20);
        _motorIdx = new QSpinBox; _motorIdx->setRange(1, 12);
        _plcId  = new QLineEdit("1");
        _motorId= new QLineEdit("1");
        _cmd    = new QLineEdit("1");
        _speed  = new QLineEdit("1500");
        _pos    = new QLineEdit("1000");

        add(g,0,0,QStringLiteral("PLC编号"), _plcIdx);
        add(g,0,2,QStringLiteral("PLC ID"),   _plcId);
        _btnPlcCtrl = new QPushButton(QStringLiteral("PLC控制"));
        _btnEnable  = new QPushButton(QStringLiteral("使能"));
        g->addWidget(_btnPlcCtrl,0,4); g->addWidget(_btnEnable,0,5);

        add(g,1,0,QStringLiteral("电机编号"), _motorIdx);
        add(g,1,2,QStringLiteral("电机ID"),   _motorId);
        add(g,1,4,QStringLiteral("指令码"),   _cmd);

        add(g,2,0,QStringLiteral("目标转速"), _speed);
        add(g,2,2,QStringLiteral("目标位置"), _pos);
        _btnMotorCtrl = new QPushButton(QStringLiteral("电机控制"));
        g->addWidget(_btnMotorCtrl,2,4);

        // 快捷控制
        _btnFwd=new QPushButton(QStringLiteral("正转"));
        _btnRev=new QPushButton(QStringLiteral("反转"));
        _btnZero=new QPushButton(QStringLiteral("回0"));
        _btnStop=new QPushButton(QStringLiteral("停止"));
        _btnUrg =new QPushButton(QStringLiteral("急停"));
        g->addWidget(_btnFwd,3,0); g->addWidget(_btnRev,3,1);
        g->addWidget(_btnZero,3,2); g->addWidget(_btnStop,3,3);
        g->addWidget(_btnUrg,3,4);
        root->addWidget(gb, 3);

        // 交互：仅发消息
        connect(_btnPlcCtrl, &QPushButton::clicked, this, [this]{
            send_ui_message("plc.ctrl", QJsonObject{
                {"plc_index", _plcIdx->value()-1},{"plc_id", _plcId->text()},
            });
        });
        connect(_btnEnable, &QPushButton::clicked, this, [this]{
            send_ui_message("plc.enable", QJsonObject{{"plc_index", _plcIdx->value()-1},{"plc_id", _plcId->text()}});
        });
        connect(_btnMotorCtrl, &QPushButton::clicked, this, [this]{
            send_ui_message("motor.ctrl", QJsonObject{
                {"plc_index", _plcIdx->value()-1},
                {"motor_index", _motorIdx->value()-1},
                {"motor_id", _motorId->text()},
                {"cmd", _cmd->text()},
                {"speed", _speed->text()},
                {"position", _pos->text()}
            });
        });
        connect(_btnFwd,  &QPushButton::clicked, this, [this]{ postQuick(1);  });
        connect(_btnRev,  &QPushButton::clicked, this, [this]{ postQuick(2);  });
        connect(_btnZero, &QPushButton::clicked, this, [this]{ postQuick(3);  });
        connect(_btnStop, &QPushButton::clicked, this, [this]{ postQuick(4);  });
        connect(_btnUrg,  &QPushButton::clicked, this, [this]{ postQuick(99); });
    }

public slots:
    // 任务侧把（所有PLC+电机）的最新快照推给UI
    void applyPlcSnapshot(const QJsonObject& s){
        // 期望结构：{ "rows":[ {plc, plc_id, motor, motor_id, plc_status, alarm, mode, run, motor_status, rpm, pos}, ... ] }
        auto rows = s.value("rows").toArray();
        _tbl->setRowCount(rows.size());
        for(int i=0;i<rows.size();++i){
            auto o = rows[i].toObject();
            setCell(i,0,o["plc"        ].toVariant().toString());
            setCell(i,1,o["plc_id"     ].toVariant().toString());
            setCell(i,2,o["motor"      ].toVariant().toString());
            setCell(i,3,o["motor_id"   ].toVariant().toString());
            setCell(i,4,o["plc_status" ].toVariant().toString());
            setCell(i,5,o["alarm"      ].toVariant().toString());
            setCell(i,6,o["mode"       ].toVariant().toString());
            setCell(i,7,o["run"        ].toVariant().toString());
            setCell(i,8,o["motor_status"].toVariant().toString());
            setCell(i,9,QString("R:%1  P:%2")
                          .arg(o["rpm"].toVariant().toString(),
                               o["pos"].toVariant().toString()));
        }
    }

private:
    template<typename W>
    void add(QGridLayout* g,int r,int c,const QString& name,W* w){
        g->addWidget(new QLabel(name), r, c); g->addWidget(w, r, c+1);
    }
    void setCell(int r,int c,const QString& s){
        if(!_tbl->item(r,c)) _tbl->setItem(r,c,new QTableWidgetItem);
        _tbl->item(r,c)->setText(s); _tbl->item(r,c)->setTextAlignment(Qt::AlignCenter);
    }
    void postQuick(int cmd){
        send_ui_message("motor.ctrl.quick", QJsonObject{
            {"plc_index", _plcIdx->value()-1},
            {"motor_index", _motorIdx->value()-1},
            {"motor_id", _motorId->text()},
            {"quick_cmd", cmd},
            {"speed", _speed->text()},
            {"position", _pos->text()}
        });
    }

private:
    QTableWidget* _tbl{};
    QSpinBox *_plcIdx{}, *_motorIdx{};
    QLineEdit *_plcId{}, *_motorId{}, *_cmd{}, *_speed{}, *_pos{};
    QPushButton *_btnPlcCtrl{}, *_btnEnable{}, *_btnMotorCtrl{};
    QPushButton *_btnFwd{}, *_btnRev{}, *_btnZero{}, *_btnStop{}, *_btnUrg{};
};

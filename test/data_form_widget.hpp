#pragma once
#include <QWidget>
#include <QTableWidget>
#include <QGroupBox>
#include <QLabel>
#include <QPushButton>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QHeaderView>
#include "ui_bus.hpp"

class DataFormWidget : public QWidget {
    Q_OBJECT
public:
    explicit DataFormWidget(QWidget* parent=nullptr): QWidget(parent){
        auto* root = new QVBoxLayout(this);

        // —— 顶部状态条 ——
        auto* gbTop = new QGroupBox(QStringLiteral("系统状态"));
        auto* gt = new QGridLayout(gbTop);
        _labPlc = new QLabel(colBad(QStringLiteral("PLC：离线")));
        _labNet = new QLabel(colBad(QStringLiteral("网络：未连接")));
        _labErr = new QLabel(QStringLiteral("错误码：--"));
        _labHint= new QLabel(QStringLiteral("提示：--"));
        gt->addWidget(_labPlc,0,0); gt->addWidget(_labNet,0,1);
        gt->addWidget(_labErr,1,0,1,2);
        gt->addWidget(_labHint,2,0,1,2);

        // —— 快捷操作（对当前选中行） ——
        auto* gbOps = new QGroupBox(QStringLiteral("快捷操作（作用于选中对象）"));
        auto* go = new QHBoxLayout(gbOps);
        _btnHome  = new QPushButton(QStringLiteral("回零"));
        _btnZero  = new QPushButton(QStringLiteral("设置参考点"));
        _btnPress = new QPushButton(QStringLiteral("支腿压实"));
        _btnReset = new QPushButton(QStringLiteral("复位报警"));
        _btnStop  = new QPushButton(QStringLiteral("停止"));
        _btnEStop = new QPushButton(QStringLiteral("急停"));
        go->addWidget(_btnHome); go->addWidget(_btnZero); go->addWidget(_btnPress);
        go->addWidget(_btnReset); go->addWidget(_btnStop); go->addWidget(_btnEStop);

        // —— 明细表（PLC×电机） ——
        _tbl = new QTableWidget; _tbl->setColumnCount(11);
        _tbl->setHorizontalHeaderLabels({QStringLiteral("轨排#"),QStringLiteral("PLC ID"),QStringLiteral("电机#"),QStringLiteral("电机ID"),
                                         QStringLiteral("运行"),QStringLiteral("模式"),QStringLiteral("报警"),
                                         QStringLiteral("位置"),QStringLiteral("速度"),QStringLiteral("转矩"),QStringLiteral("温度")});
        _tbl->horizontalHeader()->setStretchLastSection(true);
        _tbl->verticalHeader()->setVisible(false);
        _tbl->setEditTriggers(QAbstractItemView::NoEditTriggers);
        _tbl->setSelectionBehavior(QAbstractItemView::SelectRows);

        root->addWidget(gbTop);
        root->addWidget(gbOps);
        root->addWidget(_tbl, 1);

        // —— 交互：选中行上下文发布到总线 ——
        connect(_tbl, &QTableWidget::itemSelectionChanged, this, [this]{
            auto [plcIdx, motorIdx, plcId, motorId] = currentContext();
            if(plcIdx<0) return;
            send_ui_message("context.select", QJsonObject{
                {"plc_index", plcIdx}, {"motor_index", motorIdx},
                {"plc_id", plcId}, {"motor_id", motorId}
            });
        });

        // —— 交互：快捷动作均只发消息 —— 
        connect(_btnHome,  &QPushButton::clicked, this, [this]{ postQuick("home");  });
        connect(_btnZero,  &QPushButton::clicked, this, [this]{ postQuick("set_zero"); });
        connect(_btnPress, &QPushButton::clicked, this, [this]{ postQuick("press_leg"); });
        connect(_btnReset, &QPushButton::clicked, this, [this]{ postQuick("reset_alarm"); });
        connect(_btnStop,  &QPushButton::clicked, this, [this]{ postQuick("stop"); });
        connect(_btnEStop, &QPushButton::clicked, this, [this]{ postQuick("estop"); });
    }

public slots:
    // 任务侧周期广播系统快照（含顶栏+明细）
    // 期望结构：{
    //   "sys":{"plc_online":true,"net_online":true,"err":"E000","hint":"..."},
    //   "rows":[{ "plc":1,"plc_id":"X","motor":1,"motor_id":"M",
    //             "run":"ON","mode":"CSP","alarm":"-", "pos":"100", "spd":"1500", "tor":"3.2", "temp":"45" },...]
    // }
    void applySnapshot(const QJsonObject& snap){
        // 顶部状态
        auto sys = snap.value("sys").toObject();
        setOnlineLabel(_labPlc, sys.value("plc_online").toBool(false), QStringLiteral("PLC"));
        setOnlineLabel(_labNet, sys.value("net_online").toBool(false), QStringLiteral("网络"));
        _labErr->setText(QStringLiteral("错误码：") + sys.value("err").toString("--"));
        _labHint->setText(QStringLiteral("提示：") + sys.value("hint").toString("--"));

        // 明细
        auto rows = snap.value("rows").toArray();
        _tbl->setRowCount(rows.size());
        for(int i=0;i<rows.size();++i){
            auto o = rows[i].toObject();
            set(i,0,o["plc"]);      set(i,1,o["plc_id"]);
            set(i,2,o["motor"]);    set(i,3,o["motor_id"]);
            set(i,4,o["run"]);      set(i,5,o["mode"]);
            set(i,6,o["alarm"]);    set(i,7,o["pos"]);
            set(i,8,o["spd"]);      set(i,9,o["tor"]);
            set(i,10,o["temp"]);
        }
    }

private:
    QString colGood(const QString& s){ return QString("<span style='color:#0c0;'>%1</span>").arg(s); }
    QString colBad (const QString& s){ return QString("<span style='color:#f60;'>%1</span>").arg(s); }
    void setOnlineLabel(QLabel* lab, bool on, const QString& name){
        lab->setText(on ? colGood(name+QStringLiteral("：在线")) : colBad(name+QStringLiteral("：离线")));
    }
    void set(int r,int c,const QJsonValue& v){
        if(!_tbl->item(r,c)) _tbl->setItem(r,c,new QTableWidgetItem);
        auto s = v.toVariant().toString();
        _tbl->item(r,c)->setText(s); _tbl->item(r,c)->setTextAlignment(Qt::AlignCenter);
    }
    std::tuple<int,int,QString,QString> currentContext() const {
        int row = _tbl->currentRow();
        if(row<0) return {-1,-1,{},{}};
        auto plcIdx   = _tbl->item(row,0)? _tbl->item(row,0)->text().toInt()-1 : -1;
        auto motorIdx = _tbl->item(row,2)? _tbl->item(row,2)->text().toInt()-1 : -1;
        auto plcId    = _tbl->item(row,1)? _tbl->item(row,1)->text() : QString{};
        auto motorId  = _tbl->item(row,3)? _tbl->item(row,3)->text() : QString{};
        return {plcIdx, motorIdx, plcId, motorId};
    }
    void postQuick(const QString& action){
        auto [plcIdx, motorIdx, plcId, motorId] = currentContext();
        if(plcIdx<0) return;
        send_ui_message("motor.quick", QJsonObject{
            {"action", action},
            {"plc_index", plcIdx}, {"motor_index", motorIdx},
            {"plc_id", plcId}, {"motor_id", motorId}
        });
    }

private:
    QLabel *_labPlc{}, *_labNet{}, *_labErr{}, *_labHint{};
    QTableWidget* _tbl{};
    QPushButton *_btnHome{}, *_btnZero{}, *_btnPress{}, *_btnReset{}, *_btnStop{}, *_btnEStop{};
};

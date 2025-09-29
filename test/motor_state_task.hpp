#pragma once
#include <QTimer>
#include <QHash>
#include <QPair>
#include <QJsonArray>
#include "core/taskbase.hpp"
#include "core/message_bus.hpp"
#include "core/message.hpp"
#include "plc_device.hpp"
#include "motor_device.hpp"

class MotorStateTask : public TaskBase {
    Q_OBJECT
public:
    explicit MotorStateTask(QObject* parent=nullptr): TaskBase(parent){
        _tmr = new QTimer(this);
        connect(_tmr, &QTimer::timeout, this, &MotorStateTask::pollOnce);
        _tmr->start(200); // 5Hz 轮询
    }
    void init() override {}
    void exec() override {}
    void stop() override { _tmr->stop(); }

    void addPlc(int plc_index, const QString& plc_id, PlcDevice* plc){
        _plcs.insert(plc_index, plc);
        _plcId[plc_index] = plc_id;
        connect(plc, &PlcDevice::responseDecoded, this, [this, plc_index](const PlcResponse& r, const QJsonObject& j){
            _plcJson[plc_index] = j;
        });
    }
    void addMotor(int plc_index, int motor_index, const QString& motor_id, MotorDevice* md){
        _motors.insert({plc_index,motor_index}, md);
        _motorId[{plc_index,motor_index}] = motor_id;
        connect(md, &MotorDevice::responseDecoded, this, [this, plc_index, motor_index](const MotorResponse& r, const QJsonObject& j){
            _motorJson[{plc_index,motor_index}] = j;
        });
    }

private:
    QTimer* _tmr{};
    QHash<int, PlcDevice*> _plcs;
    QHash<QPair<int,int>, MotorDevice*> _motors;
    QHash<int, QString> _plcId;
    QHash<QPair<int,int>, QString> _motorId;

    QHash<int, QJsonObject> _plcJson;
    QHash<QPair<int,int>, QJsonObject> _motorJson;

    void pollOnce(){
        // 触发一次读取（底层完成后会回调 onReadHoldingRegistersDone → 填充 _plcJson/_motorJson）
        for(auto it=_plcs.begin(); it!=_plcs.end(); ++it){
            it.value()->readResponse(/*localIndex*/it.key());
        }
        for(auto it=_motors.begin(); it!=_motors.end(); ++it){
            const auto plcIdx = it.key().first;
            const auto motIdx = it.key().second;
            it.value()->readResponse(/*localIndex*/motIdx);
        }
        // 汇总并广播
        publishSnapshot();
    }

    void publishSnapshot(){
        // 顶部系统状态（以第一个PLC为准，可按需汇聚）
        QJsonObject sys{
            {"plc_online", true},          // 若需要，可依据 _plcJson 是否新鲜来判断
            {"net_online", true},
            {"err", "E000"},
            {"hint", "运行正常"}
        };
        QJsonArray rows;
        for(auto it=_motors.begin(); it!=_motors.end(); ++it){
            const int plcIdx = it.key().first;
            const int motIdx = it.key().second;
            const QString plc_id = _plcId.value(plcIdx, QString("PLC-%1").arg(plcIdx+1));
            const QString motor_id = _motorId.value({plcIdx,motIdx}, QString("M%1").arg(motIdx+1));

            const auto m = _motorJson.value({plcIdx,motIdx}, QJsonObject{});
            const QString run = m.value("run").toString("IDLE");
            const QString alarm = m.value("alarm").toBool(false) ? QStringLiteral("ALM") : QStringLiteral("-");
            rows.push_back(QJsonObject{
                {"plc", plcIdx+1}, {"plc_id", plc_id},
                {"motor", motIdx+1}, {"motor_id", motor_id},
                {"run", run},
                {"mode", "CSP"},
                {"alarm", alarm},
                {"pos",  m.value("pos").toString()},
                {"spd",  m.value("rpm").toString()},
                {"tor",  QString::number(m.value("moment").toInt())},
                {"temp", ""},   // 协议暂无温度
                {"homed", m.value("homed").toBool(false)},
                {"leg_retracted", m.value("leg_retracted").toBool(false)},
                {"busy", m.value("busy").toBool(false)},
                {"rpm",  m.value("rpm").toString()}
            });
        }
        Message msg; msg.header.type="plc.snapshot";
        msg.payload = QJsonObject{{"sys", sys}, {"rows", rows}};
        MessageBus::instance().post(msg);
    }
};

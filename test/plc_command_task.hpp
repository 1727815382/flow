#pragma once
#include <QHash>
#include <QPair>
#include "core/taskbase.hpp"
#include "core/message_bus.hpp"
#include "core/message.hpp"
#include "motor_common.hpp"
#include "motor_device.hpp"
#include "plc_device.hpp"

class PlcCommandTask : public TaskBase {
    Q_OBJECT
public:
    explicit PlcCommandTask(QObject* parent=nullptr): TaskBase(parent){
        MessageBus::instance().subscribe([this](Message& m){ onMessage(m); });
    }
    void init() override {}
    void exec() override {}
    void stop() override {}

    // 在启动阶段注册设备
    void addPlcDevice(int plc_index, PlcDevice* dev){ _plcs.insert(plc_index, dev); }
    void addMotorDevice(int plc_index, int motor_index, MotorDevice* dev){ _motors.insert({plc_index,motor_index}, dev); }

private:
    QHash<int, PlcDevice*> _plcs;
    QHash<QPair<int,int>, MotorDevice*> _motors;

    void onMessage(Message& m){
        if(m.header.type!="motor.op.request") return;
        const auto p=m.payload;
        const int plcIdx=p.value("plc_index").toInt(-1);
        const int motIdx=p.value("motor_index").toInt(-1);
        const QString op=p.value("op").toString();
        MotorDevice* md = _motors.value({plcIdx,motIdx}, nullptr);
        if(!md) return;

        // 映射高层op到协议不变的方法
        if(op==gpxm::OP_STOP)        md->stop();
        else if(op==gpxm::OP_FWD)    md->fwd(p.value("params").toObject().value("rpm").toInt(1200));
        else if(op==gpxm::OP_REV)    md->rev(p.value("params").toObject().value("rpm").toInt(1200));
        else if(op==gpxm::OP_HOME)   md->home();
        else if(op==gpxm::OP_LEG_PRESS)   md->legPress();
        else if(op==gpxm::OP_LEG_RETRACT) md->legRetract();
        else if(op==gpxm::OP_JOG_H1 || op==gpxm::OP_JOG_H2) md->jogUp();
        else if(op==gpxm::OP_JOG_DOWN)    md->jogDown();
        else if(op==gpxm::OP_GOTO_POS)    md->gotoPos(p.value("params").toObject().value("pos").toInt(0));
    }
};

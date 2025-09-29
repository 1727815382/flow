#pragma once
#include <QObject>
#include <QSettings>
#include <QVector>
#include <QHash>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonDocument>
#include <functional>
#include <QDebug>

#include "plc_device.hpp"
#include "motor_device.hpp"
#include "motor_common.hpp"
#include "plc_command_task.hpp"
#include "motor_state_task.hpp"
#include "net/modbus_bridge.hpp"

class DeviceConfigLoaderLegacy : public QObject {
    Q_OBJECT
public:
    struct AddrDefaults {
        int plc_ctrl_addr  = 4000, plc_ctrl_regs  = 20;
        int plc_resp_addr  = 4000, plc_resp_regs  = 20;
        int mot_ctrl_addr  = 2020, mot_ctrl_regs  = 20;
        int mot_resp_addr  = 3020, mot_resp_regs  = 18;
        int motor_default_rpm   = 1200;
        int motor_jog_rpm       = 300;
        int motor_jog_pulse_ms  = 120;
        int motor_goto_rpm      = 800;
        int motor_press_rpm     = 400;
        int motor_retract_rpm   = 400;
    };
    using UnitMapFn = std::function<int(int,int,int,const QString&)>;

    explicit DeviceConfigLoaderLegacy(const QString& iniPath,
                                      AddrDefaults addr = {},
                                      QObject* parent=nullptr)
        : QObject(parent), _settings(iniPath, QSettings::IniFormat), _addr(addr) {
        _mapMotorUnitId = [](int plcUid, int, int secIdx, const QString&){ return plcUid*10 + secIdx; };
    }
    void setMotorUnitIdMapper(UnitMapFn fn){ if(fn) _mapMotorUnitId = std::move(fn); }

    QJsonObject build(PlcCommandTask* cmdTask, MotorStateTask* stateTask, ModbusBridge* bridge){
        Q_ASSERT(bridge);
        _settings.beginGroup("PLCDevices");

        int size = _settings.value("size", 0).toInt();
        QStringList groups = _settings.childGroups();
        if(groups.isEmpty() && size>0) for(int i=1;i<=size;++i) groups << QString::number(i);

        QJsonArray plcList; int plcCount=0;
        for(const QString& grp : groups){
            bool ok=false; int grpNum=grp.toInt(&ok);
            if(!ok) continue;
            const int plcIndex=grpNum-1;
            _settings.beginGroup(grp);

            const QString plc_name = _settings.value("name", QString("plc%1").arg(plcIndex)).toString();
            const int plc_unitId   = _settings.value("ip", plcIndex+1).toInt();

            auto* plc = new PlcDevice(plc_unitId,
                                      _addr.plc_ctrl_addr, _addr.plc_ctrl_regs,
                                      _addr.plc_resp_addr, _addr.plc_resp_regs,
                                      this);
            plc->writeRegs = [bridge](int u,int a,const QVector<quint16>& r){ return bridge->writeMultiple(u,a,r); };
            plc->readRegs  = [bridge](int u,int a,int c){ return bridge->readHolding(u,a,c); };
            QObject::connect(bridge, &ModbusBridge::readDone, plc, &PlcDevice::onReadHoldingRegistersDone);

            if(cmdTask)   cmdTask->addPlcDevice(plcIndex, plc);
            if(stateTask) stateTask->addPlc(plcIndex, plc_name, plc);

            QJsonArray motors;
            for(int s=1; s<=4; ++s){
                const QString key = QString("section%1").arg(s);
                const QString secStr = _settings.value(key).toString().trimmed();
                if(secStr.isEmpty() || secStr=="000") continue;

                const int motor_unitId = _mapMotorUnitId(plc_unitId, plcIndex, s, secStr);

                auto* md = new MotorDevice(motor_unitId,
                                           _addr.mot_ctrl_addr, _addr.mot_ctrl_regs,
                                           _addr.mot_resp_addr, _addr.mot_resp_regs,
                                           this);
                md->writeRegs = [bridge](int u,int a,const QVector<quint16>& r){ return bridge->writeMultiple(u,a,r); };
                md->readRegs  = [bridge](int u,int a,int c){ return bridge->readHolding(u,a,c); };
                QObject::connect(bridge, &ModbusBridge::readDone, md, &MotorDevice::onReadHoldingRegistersDone);

                md->_defaultRpm        = _addr.motor_default_rpm;
                md->_jogRpm            = _addr.motor_jog_rpm;
                md->_jogPulseMs        = _addr.motor_jog_pulse_ms;
                md->_gotoRpm           = _addr.motor_goto_rpm;
                md->_defaultPressRpm   = _addr.motor_press_rpm;
                md->_defaultRetractRpm = _addr.motor_retract_rpm;

                const int motorIndex = s-1;
                const QString motor_id = secStr;

                if(cmdTask)   cmdTask->addMotorDevice(plcIndex, motorIndex, md);
                if(stateTask) stateTask->addMotor(plcIndex, motorIndex, motor_id, md);

                motors.push_back(QJsonObject{
                    {"index", motorIndex}, {"display_id", motor_id},
                    {"unit_id", motor_unitId},
                    {"ctrl_addr", _addr.mot_ctrl_addr}, {"ctrl_regs", _addr.mot_ctrl_regs},
                    {"resp_addr", _addr.mot_resp_addr}, {"resp_regs", _addr.mot_resp_regs}
                });
            }

            plcList.push_back(QJsonObject{
                {"plc_index", plcIndex}, {"name", plc_name},
                {"unit_id", plc_unitId},
                {"ctrl_addr", _addr.plc_ctrl_addr}, {"ctrl_regs", _addr.plc_ctrl_regs},
                {"resp_addr", _addr.plc_resp_addr}, {"resp_regs", _addr.plc_resp_regs},
                {"motors", motors}
            });

            _settings.endGroup(); ++plcCount;
        }
        _settings.endGroup();
        return QJsonObject{{"plc_count", plcCount},{"plcs", plcList}};
    }

private:
    QSettings _settings;
    AddrDefaults _addr;
    UnitMapFn _mapMotorUnitId;
};

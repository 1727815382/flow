#pragma once
#include <QObject>
#include <QVector>
#include <QJsonObject>
#include <QDateTime>
#include <QTimer>
#include <QtGlobal>

// ================= 协议结构：不改动 =================
#pragma pack(push,1)
struct MotorResponse {
    short _id;
    short _status;       // 0空闲 1正转 2反转 4归零成功? 5参考点成功 6收腿位置成功 100+ 故障
    short _positionH;
    short _positionL;
    short _speedH;
    short _speedL;
    short _moment;       // 压力
    short _setingStatus; // 4参考点成功 7收腿位置成功
    int   _index;        // 本地字段
    short _bat[10];
};
struct MotorCtrl {
    short _id;            // 电机ID
    short _urgenStop;     // 1 停止
    short _cmd;           // 0空闲/1正转/2反转/3停止/4归0
    short _targetPosH;    // *0.01 pulse
    short _targetPosL;
    short _targetSpeedH;  // 0~2000 rpm
    short _targetSpeedL;
    short _zero;          // 1: 0点设置
    short _zeroNumH;      // 0点值
    short _zeroNumL;
    short _bat[10];
};
#pragma pack(pop)

// ================ 编解码工具 ================
namespace motor_codec {

inline QVector<quint16> encodeMotorCtrl(const MotorCtrl& x) {
    QVector<quint16> r;
    r << quint16(x._id) << quint16(x._urgenStop) << quint16(x._cmd);
    r << quint16(x._targetPosH) << quint16(x._targetPosL);
    r << quint16(x._targetSpeedH) << quint16(x._targetSpeedL);
    r << quint16(x._zero) << quint16(x._zeroNumH) << quint16(x._zeroNumL);
    for(int i=0;i<10;i++) r << quint16(x._bat[i]);
    return r;
}

inline bool decodeMotorResponse(const QVector<quint16>& regs, MotorResponse& out, int localIndex=-1) {
    if (regs.size() < 18) return false;
    int i=0;
    out._id        = qint16(regs[i++]);
    out._status    = qint16(regs[i++]);
    out._positionH = qint16(regs[i++]);
    out._positionL = qint16(regs[i++]);
    out._speedH    = qint16(regs[i++]);
    out._speedL    = qint16(regs[i++]);
    out._moment    = qint16(regs[i++]);
    out._setingStatus = qint16(regs[i++]);
    out._index     = localIndex;
    for(int j=0;j<10;j++) out._bat[j] = (i<regs.size()? qint16(regs[i++]):0);
    return true;
}

inline qint32 merge32(short hi, short lo){
    return (qint32(qint16(hi))<<16) | quint16(lo);
}

// 统一 JSON（供 UI/任务使用）
inline QJsonObject toJson(const MotorResponse& r){
    const qint32 pos = merge32(r._positionH, r._positionL);
    const qint32 spd = merge32(r._speedH,    r._speedL);

    QString run = "IDLE"; bool busy=false, homed=false, legRetracted=false, alarm=false;
    if      (r._status==1) { run="FWD"; busy=true; }
    else if (r._status==2) { run="REV"; busy=true; }
    else if (r._status==4) { run="WORK"; homed=true; }    // 回零成功/到位（如协议如此）
    else if (r._status>=100){ alarm=true; }

    if (r._setingStatus==4) homed = true;
    if (r._setingStatus==7) legRetracted = true;

    return QJsonObject{
        {"motor_id", r._id},
        {"status_raw", r._status},
        {"run", run},
        {"busy", busy},
        {"alarm", alarm},
        {"homed", homed},
        {"leg_retracted", legRetracted},
        {"pos", QString::number(pos)},
        {"rpm", QString::number(spd)},
        {"moment", r._moment}
    };
}

} // namespace motor_codec

// ================ 电机设备类（协议不变；提供友好API） ================
class MotorDevice : public QObject {
    Q_OBJECT
public:
    explicit MotorDevice(int unitId,
                         int ctrlAddr, int ctrlRegs,
                         int respAddr, int respRegs,
                         QObject* parent=nullptr)
        : QObject(parent),
          _unitId(unitId),
          _addrCtrl(ctrlAddr), _lenCtrl(ctrlRegs),
          _addrResp(respAddr), _lenResp(respRegs) {}

    // 高层友好 API（内部仍写 MotorCtrl，不改协议）
    bool stop()       { MotorCtrl c=base(); c._cmd=3; return writeCtrl(c); }
    bool fwd(int rpm) { MotorCtrl c=base(); c._cmd=1; setSpeed(c,rpm); return writeCtrl(c); }
    bool rev(int rpm) { MotorCtrl c=base(); c._cmd=2; setSpeed(c,rpm); return writeCtrl(c); }
    bool home()       { MotorCtrl c=base(); c._cmd=4; return writeCtrl(c); }

    bool legPress()   { MotorCtrl c=base(); c._cmd=1; setSpeed(c,_defaultPressRpm); return writeCtrl(c); }
    bool legRetract() { MotorCtrl c=base(); c._cmd=2; setSpeed(c,_defaultRetractRpm); return writeCtrl(c); }

    bool jogUp()      { return pulseCmd(1, _jogPulseMs, _jogRpm); }
    bool jogDown()    { return pulseCmd(2, _jogPulseMs, _jogRpm); }

    bool gotoPos(int pulse01) {
        MotorCtrl c=base(); c._cmd=1; setSpeed(c,_gotoRpm);
        const qint32 v=pulse01; c._targetPosH=qint16((v>>16)&0xFFFF); c._targetPosL=qint16(v&0xFFFF);
        return writeCtrl(c);
    }

    // 原生读/写
    bool writeCtrl(const MotorCtrl& mc){
        // 设备层冗余忙闲拦截（上层控制任务也会做）
        if (_lastBusy && !(mc._cmd==3 || mc._cmd==4)) {
            emit ioError("motor.busy", "motor not idle");
            return false;
        }
        return writeRegs(_unitId, _addrCtrl, motor_codec::encodeMotorCtrl(mc));
    }

    bool readResponse(int localIndex=-1){
        _pendingIndex = localIndex;
        return readRegs(_unitId, _addrResp, _lenResp);
    }

    // 由集成方注入 Modbus 读写
    std::function<bool(int,int,const QVector<quint16>&)> writeRegs =
        [](int, int, const QVector<quint16>&){ return false; };
    std::function<bool(int,int,int)> readRegs =
        [](int, int, int){ return false; };

signals:
    void responseDecoded(const MotorResponse& r, QJsonObject json);
    void ioError(const QString& where, const QString& msg);

public slots:
    void onReadHoldingRegistersDone(int unitId, int addr, const QVector<quint16>& regs){
        if (unitId!=_unitId || addr!=_addrResp) return;
        MotorResponse r{};
        if(!motor_codec::decodeMotorResponse(regs, r, _pendingIndex)) {
            emit ioError("motor.decode", "decodeMotorResponse failed (size)");
            return;
        }
        const auto json = motor_codec::toJson(r);
        _lastBusy = json.value("busy").toBool(false);
        emit responseDecoded(r, json);
    }

private:
    MotorCtrl base() const {
        MotorCtrl c{}; c._id=_motorId; c._urgenStop=0; c._cmd=0;
        c._targetPosH=0; c._targetPosL=0; setSpeed(c,_defaultRpm);
        c._zero=0; c._zeroNumH=0; c._zeroNumL=0; for(short &b: c._bat) b=0;
        return c;
    }
    static void setSpeed(MotorCtrl& c, int rpm){
        const quint32 v=quint32(rpm);
        c._targetSpeedH=qint16((v>>16)&0xFFFF);
        c._targetSpeedL=qint16(v&0xFFFF);
    }
    // 点动脉冲：写入→延迟→停止（若协议需要沿触发，可在此改为写1→写0）
    bool pulseCmd(short cmd, int pulseMs, int rpm){
        MotorCtrl c=base(); c._cmd=cmd; setSpeed(c,rpm);
        if(!writeCtrl(c)) return false;
        QTimer::singleShot(pulseMs, this, [this]{ MotorCtrl s=base(); s._cmd=3; writeCtrl(s); });
        return true;
    }

private:
    int _unitId;
    int _addrCtrl, _lenCtrl;
    int _addrResp, _lenResp;

    bool _lastBusy=false;
    int _pendingIndex{-1};

public:
    // 可配置
    short _motorId = 1;
    int _defaultRpm = 1200;
    int _jogRpm     = 300;
    int _jogPulseMs = 120;
    int _gotoRpm    = 800;
    int _defaultPressRpm   = 400;
    int _defaultRetractRpm = 400;
};

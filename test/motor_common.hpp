#pragma once
#include <QObject>
#include <QHash>
#include <QDateTime>
#include <QJsonObject>
#include <QJsonArray>

namespace gpxm {

enum class RunState : int { Idle=0, Forward=1, Reverse=2, Working=3 };

static inline const char* OP_FWD         = "fwd";
static inline const char* OP_REV         = "rev";
static inline const char* OP_STOP        = "stop";
static inline const char* OP_HOME        = "home";
static inline const char* OP_LEG_PRESS   = "leg_press";
static inline const char* OP_LEG_RETRACT = "leg_retract";
static inline const char* OP_JOG_H1      = "jog_h1";
static inline const char* OP_JOG_H2      = "jog_h2";
static inline const char* OP_JOG_DOWN    = "jog_down";
static inline const char* OP_GOTO_POS    = "goto_pos";

struct MotorKey {
    int plc_index=-1;   // 0-based
    int motor_index=-1; // 0-based
    QString plc_id;
    QString motor_id;
    bool operator==(const MotorKey& o) const { return plc_index==o.plc_index && motor_index==o.motor_index; }
};
inline uint qHash(const MotorKey& k, uint seed=0){ return ::qHash((k.plc_index<<16) ^ k.motor_index, seed); }

struct MotorStatus {
    RunState run = RunState::Idle;
    bool alarm=false;
    bool homed=false;
    bool leg_retracted=false;
    bool busyFlag=false;
    QDateTime lastUpdate;
};

struct Job {
    QString job_id;
    QString op;
    QString scope;               // "single"|"section"|"track"
    QList<MotorKey> targets;
    QJsonObject params;
    bool exclusive=false;        // home/leg_press/leg_retract
    int timeout_ms=30000;        // 默认30s
    QDateTime start_time;
};

inline QString stateToText(RunState s){
    switch(s){
        case RunState::Idle: return QObject::tr("空闲");
        case RunState::Forward: return QObject::tr("正转");
        case RunState::Reverse: return QObject::tr("反转");
        case RunState::Working: return QObject::tr("工作中");
    } return QObject::tr("未知");
}

} // namespace gpxm

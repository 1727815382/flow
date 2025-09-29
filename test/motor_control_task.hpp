#pragma once
#include <QUuid>
#include <QTimer>
#include <QSet>
#include <QMap>
#include "core/taskbase.hpp"
#include "core/message_bus.hpp"
#include "core/message.hpp"
#include "motor_common.hpp"

namespace gpxm {

inline void publish_bus(const std::string& type, const QJsonObject& payload){
    Message m; m.header.type=type; m.payload=payload; MessageBus::instance().post(m);
}
inline void send_motor_op_request(const MotorKey& key, const QString& op,
                                  const QJsonObject& params, const QString& job_id){
    publish_bus("motor.op.request", QJsonObject{
        {"plc_index", key.plc_index},{"motor_index", key.motor_index},
        {"plc_id", key.plc_id},{"motor_id", key.motor_id},
        {"op", op},{"params", params},{"job_id", job_id}
    });
}
inline void ui_lock_controls(const QString& scope, const QList<MotorKey>& keys,
                             const QString& reason, const QString& job_id){
    QJsonArray arr; for(const auto& k: keys)
        arr.push_back(QJsonObject{{"plc_index",k.plc_index},{"motor_index",k.motor_index}});
    publish_bus("ui.controls.lock", QJsonObject{{"scope",scope},{"reason",reason},{"job_id",job_id},{"targets",arr}});
}
inline void ui_unlock_controls(const QString& job_id){
    publish_bus("ui.controls.unlock", QJsonObject{{"job_id",job_id}});
}
inline void job_status(const QString& phase, const Job& job, const QString& msg={}){
    QJsonArray arr; for(const auto& k: job.targets)
        arr.push_back(QJsonObject{{"plc_index",k.plc_index},{"motor_index",k.motor_index},{"plc_id",k.plc_id},{"motor_id",k.motor_id}});
    publish_bus("motor.job.status", QJsonObject{
        {"phase",phase},{"job_id",job.job_id},{"op",job.op},{"scope",job.scope},{"message",msg},{"targets",arr}
    });
}

class MotorControlTask : public TaskBase {
    Q_OBJECT
public:
    explicit MotorControlTask(QObject* parent=nullptr) : TaskBase(parent){
        MessageBus::instance().subscribe([this](Message& m){ onMessage(m); });
        _tick = new QTimer(this);
        connect(_tick, &QTimer::timeout, this, &MotorControlTask::onTick);
        _tick->start(100);
    }
    void init() override {}
    void exec() override {}
    void stop() override { _queue.clear(); _running.clear(); _jobCursor.clear(); _exclusiveLocks.clear(); }

private:
    QHash<MotorKey, MotorStatus> _status;
    QList<Job> _queue;
    QHash<QString, Job> _running;
    QSet<MotorKey> _exclusiveLocks;
    QHash<QString,int> _jobCursor;
    QTimer* _tick{};

    static QString new_job_id(){ return QUuid::createUuid().toString(QUuid::WithoutBraces); }

    void onMessage(Message& m){
        const auto& t=m.header.type; const auto p=m.payload;

        if(t=="plc.snapshot"){ apply_snapshot(p); return; }

        if(t=="motor.ctrl" || t=="motor.ctrl.quick" || t=="motor.home" || t=="motor.leg.press" || t=="motor.leg.retract"){
            Job j; j.job_id=new_job_id();
            j.op = p.value("op").toString();
            if(t=="motor.home")        j.op = OP_HOME;
            if(t=="motor.leg.press")   j.op = OP_LEG_PRESS;
            if(t=="motor.leg.retract") j.op = OP_LEG_RETRACT;

            j.scope   = p.value("scope").toString("single");
            j.params  = p.value("params").toObject();
            j.exclusive = (j.op==OP_HOME || j.op==OP_LEG_PRESS || j.op==OP_LEG_RETRACT);
            j.timeout_ms = p.value("timeout_ms").toInt(default_timeout(j.op));

            for(const auto& v: p.value("targets").toArray()){
                auto o=v.toObject();
                MotorKey k; k.plc_index=o.value("plc_index").toInt(-1); k.motor_index=o.value("motor_index").toInt(-1);
                k.plc_id=o.value("plc_id").toString(); k.motor_id=o.value("motor_id").toString();
                if(k.plc_index>=0 && k.motor_index>=0) j.targets.push_back(k);
            }
            submit(j); return;
        }

        if(t=="motor.job.stop"){
            const QString id=p.value("job_id").toString();
            if(_running.contains(id)) finish_job(id, false, tr("用户中止"));
            return;
        }
    }

    void apply_snapshot(const QJsonObject& snap){
        for(const auto& v: snap.value("rows").toArray()){
            auto o=v.toObject();
            MotorKey k; k.plc_index=o.value("plc").toInt()-1; k.motor_index=o.value("motor").toInt()-1;
            k.plc_id=o.value("plc_id").toString(); k.motor_id=o.value("motor_id").toString();

            MotorStatus s; s.lastUpdate=QDateTime::currentDateTime();
            const QString run=o.value("run").toString();
            if(run=="FWD"||run=="ON") s.run=RunState::Forward;
            else if(run=="REV")      s.run=RunState::Reverse;
            else if(run=="WORK")     s.run=RunState::Working;
            else                     s.run=RunState::Idle;

            s.alarm         = o.value("alarm").toString()!="-"
                           && !o.value("alarm").toString().isEmpty();
            s.homed         = o.value("homed").toBool(false);
            s.leg_retracted = o.value("leg_retracted").toBool(false);
            s.busyFlag      = o.value("busy").toBool(false);

            _status[k]=s;
        }
        evaluate_jobs();
    }

    void submit(Job& j){
        for(const auto& k: j.targets){
            if(j.exclusive && _exclusiveLocks.contains(k)){
                job_status("rejected", j, tr("目标被独占锁占用")); return;
            }
            const auto st=_status.value(k, MotorStatus{});
            const bool busy=(st.run!=RunState::Idle)||st.busyFlag;
            if(busy && j.op!=OP_STOP){
                job_status("rejected", j, tr("目标不空闲：%1").arg(stateToText(st.run))); return;
            }
        }
        if(j.exclusive){ for(const auto& k: j.targets) _exclusiveLocks.insert(k);
            ui_lock_controls(j.scope, j.targets, tr("独占动作执行中"), j.job_id);
        }
        j.start_time=QDateTime::currentDateTime();
        _queue.push_back(j);
        job_status("queued", j);
        try_start_next();
    }

    void try_start_next(){
        for(int i=0;i<_queue.size();){
            Job j=_queue.at(i);
            if(can_start(j)){ _queue.removeAt(i); start_job(j); } else ++i;
        }
    }
    bool can_start(const Job& j){
        for(const auto& k: j.targets){
            const auto st=_status.value(k, MotorStatus{});
            const bool busy=(st.run!=RunState::Idle)||st.busyFlag;
            if(busy && j.op!=OP_STOP) return false;
        } return true;
    }

    void start_job(const Job& j){
        _running.insert(j.job_id, j);
        job_status("started", j);
        const bool sequential=j.exclusive;
        if(sequential){ _jobCursor[j.job_id]=0; send_op_for_target(j,0); }
        else { for(int idx=0; idx<j.targets.size(); ++idx) send_op_for_target(j,idx); }
    }
    void send_op_for_target(const Job& j, int idx){
        if(idx<0||idx>=j.targets.size()) return;
        send_motor_op_request(j.targets[idx], j.op, j.params, j.job_id);
    }

    void evaluate_jobs(){
        const auto now=QDateTime::currentDateTime();
        QList<QString> toFinish;
        for(auto it=_running.begin(); it!=_running.end(); ++it){
            auto& j=it.value();
            if(j.start_time.msecsTo(now) > j.timeout_ms){ toFinish<<j.job_id; continue; }

            if(j.exclusive){
                int idx=_jobCursor.value(j.job_id,0);
                if(idx<j.targets.size()){
                    if(is_done(j.op, j.targets[idx])){ ++idx; _jobCursor[j.job_id]=idx;
                        if(idx<j.targets.size()) send_op_for_target(j, idx);
                        else toFinish<<j.job_id;
                    }else if(is_fault(j.targets[idx])){ finish_job(j.job_id,false,tr("目标发生故障")); }
                    else job_status("progress", j, tr("执行中"));
                }else toFinish<<j.job_id;
            }else{
                if(is_continuous(j.op)){ job_status("progress", j, tr("运行中")); }
                else{
                    bool allDone=true, anyFault=false;
                    for(const auto& k: j.targets){
                        if(is_fault(k)) { anyFault=true; break; }
                        if(!is_done(j.op,k)) allDone=false;
                    }
                    if(anyFault) finish_job(j.job_id,false,tr("发生故障"));
                    else if(allDone) toFinish<<j.job_id;
                    else job_status("progress", j, tr("执行中"));
                }
            }
        }
        for(const auto& id: toFinish) finish_job(id, true, tr("完成"));
    }

    bool is_done(const QString& op, const MotorKey& k) const {
        const auto s=_status.value(k, MotorStatus{});
        if(op==OP_HOME)        return s.homed;
        if(op==OP_LEG_RETRACT) return s.leg_retracted;
        if(op==OP_LEG_PRESS)   return (s.run==RunState::Idle && !s.alarm); // 若现场有专用“压实完成位”，替换为该位判断
        if(op==OP_JOG_H1 || op==OP_JOG_H2 || op==OP_JOG_DOWN) return (s.run==RunState::Idle);
        if(op==OP_GOTO_POS)    return (s.run==RunState::Idle);
        if(op==OP_STOP)        return (s.run==RunState::Idle);
        if(op==OP_FWD || op==OP_REV) return false; // 连续动作需 stop/异常来结束
        return false;
    }
    bool is_fault(const MotorKey& k) const { return _status.value(k, MotorStatus{}).alarm; }
    static bool is_continuous(const QString& op){ return (op==OP_FWD || op==OP_REV); }
    static int  default_timeout(const QString& op){
        if(op==OP_HOME) return 60000;
        if(op==OP_LEG_PRESS || op==OP_LEG_RETRACT) return 30000;
        if(op==OP_GOTO_POS) return 45000;
        return 20000;
    }

    void finish_job(const QString& id, bool ok, const QString& msg){
        if(!_running.contains(id)) return;
        auto j=_running.take(id);
        if(j.exclusive){ for(const auto& k: j.targets) _exclusiveLocks.remove(k); ui_unlock_controls(j.job_id); }
        job_status(ok? "finished":"error", j, msg);
    }

    void onTick(){ try_start_next(); evaluate_jobs(); }
};

} // namespace gpxm

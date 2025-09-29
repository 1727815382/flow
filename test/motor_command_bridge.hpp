#pragma once
#include <QJsonObject>
#include <QJsonArray>
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

// 独占动作时让 UI 禁用相关控件
inline void ui_lock_controls(const QString& scope, const QList<MotorKey>& keys,
                             const QString& reason, const QString& job_id){
    QJsonArray arr; for(const auto& k: keys)
        arr.push_back(QJsonObject{{"plc_index",k.plc_index},{"motor_index",k.motor_index}});
    publish_bus("ui.controls.lock", QJsonObject{{"scope",scope},{"reason",reason},{"job_id",job_id},{"targets",arr}});
}
inline void ui_unlock_controls(const QString& job_id){
    publish_bus("ui.controls.unlock", QJsonObject{{"job_id",job_id}});
}

// 广播作业状态（闭环）
inline void job_status(const QString& phase, const Job& job, const QString& msg={}){
    QJsonArray arr; for(const auto& k: job.targets)
        arr.push_back(QJsonObject{{"plc_index",k.plc_index},{"motor_index",k.motor_index},{"plc_id",k.plc_id},{"motor_id",k.motor_id}});
    publish_bus("motor.job.status", QJsonObject{
        {"phase",phase},{"job_id",job.job_id},{"op",job.op},{"scope",job.scope},{"message",msg},{"targets",arr}
    });
}

} // namespace gpxm

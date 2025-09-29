#pragma once
#include <string>
#include <stdint.h>
#include <QJsonObject>
#include <QJsonDocument>

/** 消息优先级（与 1.0 对齐） */
enum MsgPrio { PRIO_HIGH=0, PRIO_NORM=1, PRIO_LOW=2 };

/**
 * MsgHeader：在 1.0 的基础上做“超集”设计：
 *  - 保留 1.0 字段：id/type/version/from/to/topic/correlation_id/ts_epoch_ms/priority
 *  - 额外可选字段（默认不影响旧逻辑）：trace_id/seq/ack_required/durable/meta
 */
// core/message.hpp
struct MsgHeader {
    // —— 1.0 保留 —— //
    std::string id;
    std::string type;
    std::string version;
    std::string from;
    std::string to;
    std::string topic;

    // 新增：ask/应答用—应答时把结果回发到哪个任务
    std::string reply_to;   // <—— 加上这行

    std::string correlation_id;
    long long   ts_epoch_ms;
    int         priority;

    // —— 可选扩展 —— //
    std::string trace_id;
    uint64_t    seq = 0;
    bool        ack_required = false;
    bool        durable = false;
    QJsonObject meta;

    MsgHeader(): version("1.0"), ts_epoch_ms(0), priority(PRIO_NORM) {}
};


struct Message {
    MsgHeader   header;
    QJsonObject payload;
};

/** 小工具：把 payload 变为紧凑 JSON 字节 */
inline QByteArray toJsonBytes(const QJsonObject& o){
    return QJsonDocument(o).toJson(QJsonDocument::Compact);
}

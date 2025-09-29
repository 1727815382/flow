#include "modules/metrics_service/metrics_task.hpp"
#include <chrono>
#include "core/message_bus.hpp"
#include <QJsonArray>

static long long now_ms(){
    return (long long)std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::system_clock::now().time_since_epoch()).count();
}
long long MetricsTask::nowMs(){ return now_ms(); }

void MetricsTask::flush(){
    lastFlushMs_ = nowMs();
    QJsonArray arr;
    for (std::map<std::string, Rec>::iterator it=rec_.begin(); it!=rec_.end(); ++it){
        QJsonObject o{
            {"id", QString::fromStdString(it->first)},
            {"qsize", it->second.qsize},
            {"status", it->second.status},
            {"lastHb", (qint64)it->second.lastHb},
            {"hbCnt", it->second.hbCnt},
            {"errCnt", it->second.errCnt}
        };
        arr.push_back(o);
    }
    Message out; out.header.type="metrics.summary"; out.header.topic="metrics.summary"; out.header.from=id();
    out.payload = QJsonObject{{"items", arr}, {"ts",(qint64)lastFlushMs_}};
    sendToBus(std::move(out));
}

void MetricsTask::onMessage(Message& m){
    long long now = nowMs();
    if (m.header.type == "supervisor.status"){
        Rec& r = rec_[m.header.from];
        r.qsize = m.payload.value("queue").toInt();
        r.status = m.payload.value("status").toInt();
        r.lastHb = now;
        r.hbCnt++;
    } else if (m.header.type == "supervisor.exception"){
        rec_[m.header.from].errCnt++;
    } else if (m.header.type == "metrics.flush"){
        flush();
    }
    if (now - lastFlushMs_ >= 3000) flush();
}

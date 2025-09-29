// core/supervisor.hpp
#pragma once
#include "task_base.hpp"
#include <map>

class Supervisor : public TaskBase {
public:
    explicit Supervisor(MessageBus* bus): TaskBase("supervisor", bus){}
protected:
    void onStart() override { status_ = TS_RUNNING; }
    void onMessage(Message& m) override {
        if (m.header.type == "supervisor.status") {
            auto tid   = m.header.from;
            auto phase = m.payload.value("phase").toString();
            int qsize  = m.payload.value("queue").toInt();
            int st     = m.payload.value("status").toInt();
            tasks_[tid].qsize = qsize;
            tasks_[tid].status = st;
            tasks_[tid].last_ms = nowMs();
            // 可按需发布到 UI
        } else if (m.header.type == "supervisor.exception") {
            auto tid = m.header.from;
            auto where = m.payload.value("where").toString();
            auto what  = m.payload.value("what").toString();
            tasks_[tid].errors++;
            // 写日志
            Message log; log.header.type="log.write"; log.header.to="log"; log.header.from=id();
            log.payload = QJsonObject{{"level","error"},{"tag","supervisor"},
                                      {"msg", QString("%1: %2").arg(where, what)}};
            sendToBus(std::move(log));
        }
    }
private:
    struct Rec { int qsize=0; int status=0; long long last_ms=0; int errors=0; };
    std::map<std::string, Rec> tasks_;
    static long long nowMs(){
        return (long long)std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::system_clock::now().time_since_epoch()).count();
    }
};

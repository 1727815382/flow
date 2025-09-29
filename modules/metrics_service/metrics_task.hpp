#pragma once
#include "core/task_base.hpp"
#include <map>

class MetricsTask : public TaskBase {
public:
    explicit MetricsTask(MessageBus* bus)
        : TaskBase("metrics", bus), lastFlushMs_(0) {}

protected:
    void onStart(){ status_ = TS_RUNNING; }
    void onMessage(Message& m);

private:
    struct Rec { int qsize=0; int status=0; long long lastHb=0; int hbCnt=0; int errCnt=0; };
    std::map<std::string, Rec> rec_;
    long long lastFlushMs_;

    static long long nowMs();
    void flush();
};

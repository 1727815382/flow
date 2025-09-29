// core/scheduler_task.hpp
#pragma once
#include "core/task_base.hpp"
#include <queue>
#include <map>
#include <QJsonArray>
#include <QVariant>

class SchedulerTask : public TaskBase {
public:
    SchedulerTask(MessageBus* bus): TaskBase("scheduler", bus){}
protected:
    void onStart();
    void onMessage(Message& m);
    int  heartbeatIntervalMs() const { return 100; }
private:
    struct Job {
        std::string id;
        long long next_ms;
        QJsonObject msg;
        long long period_ms;
        bool fixed_rate;
        bool paused;
        bool catch_up;
        int  jitter_ms;
        int  left_runs;
        int  priority;
        bool operator<(const Job& o) const { return next_ms > o.next_ms; }
    };
    std::priority_queue<Job> heap_;
    std::map<std::string, Job> jobs_;

    static long long nowMs();
    static long long todayMs(int hh, int mm, int ss);
    void pushJob(const Job& j);

    void addOnce(const QJsonObject& o);
    void addDelay(const QJsonObject& o);
    void addInterval(const QJsonObject& o);
    void addFixedRate(const QJsonObject& o);
    void addDaily(const QJsonObject& o);
    void addWeekly(const QJsonObject& o);
    void addChain(const QJsonObject& o);
    void cancel(const QJsonObject& o);
    void pauseJob(const QJsonObject& o);
    void resumeJob(const QJsonObject& o);

    long long jittered(long long base, int jitter_ms);
    void sendJob(const Job& j);
    void scheduleNext(Job j, long long fired_ms);
    void tick();
};

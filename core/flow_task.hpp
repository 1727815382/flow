// core/flow_task.hpp
#pragma once
#include "core/task_base.hpp"
#include <map>
#include <vector>
#include <set>
#include <QJsonArray>

class FlowTask : public TaskBase {
public:
    FlowTask(const std::string& id, MessageBus* bus): TaskBase(id,bus), step_(""), stepStart_(0), retryCount_(0){}
protected:
    void onStart(){ status_ = TS_RUNNING; }
    void onMessage(Message& m);
private:
    struct WaitCond {
        std::vector<std::string> anyOfTypes;
        std::vector<std::string> allOfTypes;
    };
    struct StepDef {
        std::string name;
        int timeout_ms;
        int max_retry;
        int onEnterDelayMs;
        QJsonArray onEnterMsgs;
        QJsonArray onExitMsgs;
        WaitCond   wait;
        std::map<std::string, std::string> nextByEvent;
        std::string nextDefault;
        std::string nextOnTimeout;
        std::string nextOnFail;
    };
    std::map<std::string, StepDef> steps_;
    std::string step_;
    long long stepStart_;
    int retryCount_;
    std::set<std::string> fulfilled_;
    std::string lastEventType_;

    static long long nowMs();
    void loadFromJson(const QJsonObject& o);
    void gotoStep(const std::string& name);
    void fireMsgs(const QJsonArray& arr);
    void handleEvent(Message& m);
    void evaluate();
};

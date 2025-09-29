#pragma once
#include "core/task_base.hpp"
#include "ui/ui_bridge.hpp"

class UiAdapterTask : public TaskBase {
public:
    UiAdapterTask(MessageBus* bus, UiBridge* bridge);
protected:
    void onStart();
    void onMessage(Message& m);
private:
    UiBridge* bridge_;
};

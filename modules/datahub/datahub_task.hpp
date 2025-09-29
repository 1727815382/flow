#pragma once
#include "core/task_base.hpp"
#include <QReadWriteLock>
#include <QMap>
#include <QJsonValue>
#include <QJsonArray>

class DataHubTask : public TaskBase {
public:
    explicit DataHubTask(MessageBus* bus) : TaskBase("datahub", bus) {}

protected:
    void onStart() override {
        status_ = TS_RUNNING;
        bus_->subscribe(id(), "datahub.set");
        bus_->subscribe(id(), "datahub.bulk");
        bus_->subscribe(id(), "datahub.get");
        bus_->subscribe(id(), "datahub.get_many");
        bus_->subscribe(id(), "datahub.cas");
        bus_->subscribe(id(), "datahub.snapshot");
    }
    void onStop() override { status_ = TS_STOPPED; }

    void onMessage(Message& m) override;

private:
    QReadWriteLock rw_;
    QMap<QString, QJsonValue> store_;
};

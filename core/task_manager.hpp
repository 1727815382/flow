#pragma once
#include <map>
#include <memory>
#include <string>
#include <QLibrary>
#include <QJsonObject>
#include "core/task_base.hpp"
#include "core/message_bus.hpp"
#include "core/plugin_api.hpp"

/**
 * TaskManager：动态装卸任务（支持插件 .dll/.so）
 * - task.add    {id, lib, args}
 * - task.remove {id}
 * - task.start  {id}
 * - task.stop   {id}
 * - task.prio   {id, prio: low|normal|high|rt}
 * - task.list   -> task.list.reply
 */
class TaskManager : public TaskBase {
public:
    explicit TaskManager(MessageBus* bus): TaskBase("taskmgr", bus) {}

protected:
    void onStart() override {
        bus_->subscribe(id(), "task.add");
        bus_->subscribe(id(), "task.remove");
        bus_->subscribe(id(), "task.start");
        bus_->subscribe(id(), "task.stop");
        bus_->subscribe(id(), "task.prio");
        bus_->subscribe(id(), "task.list");
    }
    void onStop() override {}

    void onMessage(Message& m) override;

private:
    void doAdd(const QJsonObject& o);
    void doRemove(const QJsonObject& o);
    void doStart(const QJsonObject& o);
    void doStop (const QJsonObject& o);
    void doPrio (const QJsonObject& o);
    void replyList(const std::string& to, const std::string& cid);

    std::map<std::string, std::shared_ptr<ITask> > tasks_;
    std::map<std::string, QLibrary*> plugins_;
};

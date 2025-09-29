// core/task_manager.cpp
#include "core/task_manager.hpp"
#include "core/message_bus.hpp"
#include "core/plugin_api.hpp"
#include <QFileInfo>
#include <QJsonArray>

static std::string strOr(const QJsonObject& o, const char* k, const char* defv){
    if (o.contains(k)) return o.value(k).toString().toStdString();
    return std::string(defv);
}

void TaskManager::onMessage(Message& m){
    const std::string& t = m.header.type;
    if (t=="task.add") { doAdd(m.payload); }
    else if (t=="task.remove") { doRemove(m.payload); }
    else if (t=="task.start") { doStart(m.payload); }
    else if (t=="task.stop") { doStop(m.payload); }
    else if (t=="task.prio") { doPrio(m.payload); }
    else if (t=="task.list") { replyList(m.header.from, m.header.correlation_id); }
}

void TaskManager::doAdd(const QJsonObject& o){
    QString id = o.value("id").toString();
    if (id.isEmpty()){ reportException("task.add","id empty"); return; }
    if (tasks_.find(id.toStdString())!=tasks_.end()){ reportException("task.add","id exists"); return; }

    QString libpath = o.value("lib").toString();
    QJsonObject args = o.value("args").toObject();

    std::shared_ptr<ITask> t;
    if (!libpath.isEmpty()){
        QLibrary* lib = new QLibrary(libpath);
        if(!lib->load()){ reportException("task.add","load fail"); delete lib; return; }
        CreateTaskFn cf=(CreateTaskFn)lib->resolve("create_task");
        DestroyTaskFn df=(DestroyTaskFn)lib->resolve("destroy_task");
        if(!cf||!df){ reportException("task.add","symbol missing"); lib->unload(); delete lib; return; }
        ITask* raw = cf(bus_, id.toStdString().c_str(), &args);
        if (!raw){ reportException("task.add","create null"); lib->unload(); delete lib; return; }
        t = std::shared_ptr<ITask>(raw, df);
        plugins_[id.toStdString()] = lib;
    } else {
        reportException("task.add","no lib provided"); return;
    }
    tasks_[id.toStdString()] = t;
    bus_->registerTask(t);
}

void TaskManager::doRemove(const QJsonObject& o){
    std::string id = strOr(o, "id", "");
    std::map<std::string, std::shared_ptr<ITask> >::iterator it=tasks_.find(id);
    if (it==tasks_.end()) return;
    it->second->stop();
    bus_->unregisterTask(id);
    tasks_.erase(it);
    std::map<std::string, QLibrary*>::iterator lt = plugins_.find(id);
    if (lt!=plugins_.end()){
        QLibrary* lib = lt->second;
        if (lib){ lib->unload(); delete lib; }
        plugins_.erase(lt);
    }
}

void TaskManager::doStart(const QJsonObject& o){
    std::string id = strOr(o,"id","");
    std::map<std::string, std::shared_ptr<ITask> >::iterator it=tasks_.find(id);
    if(it!=tasks_.end()) it->second->start();
}
void TaskManager::doStop(const QJsonObject& o){
    std::string id = strOr(o,"id","");
    std::map<std::string, std::shared_ptr<ITask> >::iterator it=tasks_.find(id);
    if(it!=tasks_.end()) it->second->stop();
}
void TaskManager::doPrio(const QJsonObject& o){
    std::string id = strOr(o,"id","");
    std::string pr = strOr(o,"prio","normal");
    std::map<std::string, std::shared_ptr<ITask> >::iterator it=tasks_.find(id);
    if(it==tasks_.end()) return;
    TaskBase* tb = dynamic_cast<TaskBase*>(it->second.get());
    if(!tb) return;
    ThreadPrio p=TP_NORMAL;
    if (pr=="high") p=TP_HIGH; else if(pr=="low") p=TP_LOW; else if(pr=="rt") p=TP_REALTIME; else p=TP_NORMAL;
    tb->setThreadPriority(p);
}
void TaskManager::replyList(const std::string& to, const std::string& cid){
    QJsonArray arr;
    for (std::map<std::string, std::shared_ptr<ITask> >::iterator it=tasks_.begin(); it!=tasks_.end(); ++it){
        QJsonObject o; o.insert("id", QString::fromStdString(it->first));
        arr.push_back(o);
    }
    if (!to.empty()){
        Message r; r.header.type="task.list.reply"; r.header.to=to; r.header.from=id(); r.header.correlation_id=cid;
        QJsonObject o; o.insert("tasks", arr);
        r.payload=o;
        sendToBus(std::move(r));
    }
}

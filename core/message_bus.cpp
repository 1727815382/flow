// core/message_bus.cpp
#include "core/message_bus.hpp"

void MessageBus::registerTask(const std::shared_ptr<ITask>& t){
    if (!t) return;
    std::lock_guard<std::mutex> lk(m_tasks_);
    tasks_[t->id()] = t;
}

void MessageBus::unregisterTask(const std::string& id){
    {
        std::lock_guard<std::mutex> lk(m_tasks_);
        tasks_.erase(id);
    }
    // 同时清掉订阅
    std::lock_guard<std::mutex> lk2(m_subs_);
    for (auto& kv : subs_){
        kv.second.erase(id);
    }
}

void MessageBus::subscribe(const std::string& taskId, const std::string& topic){
    std::lock_guard<std::mutex> lk(m_subs_);
    subs_[topic].insert(taskId);
}
void MessageBus::unsubscribe(const std::string& taskId, const std::string& topic){
    std::lock_guard<std::mutex> lk(m_subs_);
    auto it = subs_.find(topic);
    if (it!=subs_.end()) it->second.erase(taskId);
}

void MessageBus::sendTo(const std::string& to, const Message& m){
    std::shared_ptr<ITask> dst;
    {
        std::lock_guard<std::mutex> lk(m_tasks_);
        auto it = tasks_.find(to);
        if (it!=tasks_.end()) dst = it->second.lock();
    }
    if (dst) dst->post(m);
}
void MessageBus::sendTo(const std::string& to, Message&& m){
    std::shared_ptr<ITask> dst;
    {
        std::lock_guard<std::mutex> lk(m_tasks_);
        auto it = tasks_.find(to);
        if (it!=tasks_.end()) dst = it->second.lock();
    }
    if (dst) dst->post(std::move(m));
}

void MessageBus::publish(const std::string& topic, const Message& m){
    std::vector<std::shared_ptr<ITask>> targets;
    {
        std::lock_guard<std::mutex> lk1(m_subs_);
        auto it = subs_.find(topic);
        if (it==subs_.end()) return;
        std::lock_guard<std::mutex> lk2(m_tasks_);
        for (auto& id : it->second){
            auto w = tasks_.find(id);
            if (w!=tasks_.end()){
                if (auto sp = w->second.lock()) targets.push_back(sp);
            }
        }
    }
    for (auto& t : targets) t->post(m);
}

void MessageBus::publish(const std::string& topic, Message&& m){
    std::vector<std::shared_ptr<ITask>> targets;
    {
        std::lock_guard<std::mutex> lk1(m_subs_);
        auto it = subs_.find(topic);
        if (it==subs_.end()) return;
        std::lock_guard<std::mutex> lk2(m_tasks_);
        for (auto& id : it->second){
            auto w = tasks_.find(id);
            if (w!=tasks_.end()){
                if (auto sp = w->second.lock()) targets.push_back(sp);
            }
        }
    }
    for (auto& t : targets) t->post(m);
}

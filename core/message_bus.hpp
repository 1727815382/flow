// core/message_bus.hpp
#pragma once
#include <unordered_map>
#include <unordered_set>
#include <mutex>
#include <memory>
#include <vector>
#include "core/itask.hpp"
#include "core/message.hpp"

class MessageBus {
public:
    MessageBus() {}

    void registerTask(const std::shared_ptr<ITask>& t);
    void unregisterTask(const std::string& id);

    void subscribe(const std::string& taskId, const std::string& topic);
    void unsubscribe(const std::string& taskId, const std::string& topic);

    void sendTo(const std::string& to, const Message& m);
    void sendTo(const std::string& to, Message&& m);

    void publish(const std::string& topic, const Message& m);
    void publish(const std::string& topic, Message&& m);

private:
    std::mutex m_tasks_;
    std::unordered_map<std::string, std::weak_ptr<ITask>> tasks_;

    std::mutex m_subs_;
    // topic -> set(taskId)
    std::unordered_map<std::string, std::unordered_set<std::string>> subs_;
};

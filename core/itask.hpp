#pragma once
#include <string>
#include "core/message.hpp"

class ITask {
public:
    virtual ~ITask(){}
    virtual const std::string& id() const = 0;
    virtual void start() = 0;
    virtual void stop() = 0;
    virtual void post(const Message& m) = 0;
    virtual void post(Message&& m) = 0;
};

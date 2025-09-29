#pragma once
#include <deque>
#include <mutex>
#include <condition_variable>
#include "core/message.hpp"

class Mailbox {
public:
    Mailbox(): closed_(false), limit_(10000), dropped_(0) {}

    void   setLimit(size_t n){ limit_ = n; }
    size_t dropped() const { return dropped_; }
    size_t size() const { std::lock_guard<std::mutex> lk(m_); return q_.size(); }

    bool push(Message&& m){
        std::unique_lock<std::mutex> lk(m_);
        if (closed_) return false;
        if (q_.size() >= limit_) { ++dropped_; return false; }
        q_.push_back(std::move(m));
        cv_.notify_one();
        return true;
    }

    bool pop(Message& out, int timeout_ms){
        std::unique_lock<std::mutex> lk(m_);
        if (timeout_ms < 0) cv_.wait(lk, [&]{ return closed_ || !q_.empty(); });
        else if (!cv_.wait_for(lk, std::chrono::milliseconds(timeout_ms), [&]{ return closed_ || !q_.empty(); }))
            return false; // timeout
        if (closed_ && q_.empty()) return false;
        out = std::move(q_.front()); q_.pop_front();
        return true;
    }

    void close(){ std::lock_guard<std::mutex> lk(m_); closed_ = true; cv_.notify_all(); }

private:
    mutable std::mutex m_;
    std::condition_variable cv_;
    std::deque<Message> q_;
    bool closed_;
    size_t limit_;
    size_t dropped_;
};

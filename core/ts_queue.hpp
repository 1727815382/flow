#pragma once
#include <mutex>
#include <condition_variable>
#include <deque>

template<typename T>
class TSQueue {
public:
    TSQueue():closed_(false){}
    void push(const T& v){
        { std::lock_guard<std::mutex> lk(m_); if(closed_) return; q_.push_back(v); }
        cv_.notify_one();
    }
    void push(T&& v){
        { std::lock_guard<std::mutex> lk(m_); if(closed_) return; q_.push_back(std::move(v)); }
        cv_.notify_one();
    }
    bool pop_blocking(T& out){
        std::unique_lock<std::mutex> lk(m_);
        cv_.wait(lk, [this]{ return closed_ || !q_.empty(); });
        if(q_.empty()) return false;
        out = std::move(q_.front()); q_.pop_front();
        return true;
    }
    void close(){ { std::lock_guard<std::mutex> lk(m_); closed_=true; } cv_.notify_all(); }
    size_t size() const { std::lock_guard<std::mutex> lk(m_); return q_.size(); }
private:
    mutable std::mutex m_;
    std::condition_variable cv_;
    std::deque<T> q_;
    bool closed_;
};

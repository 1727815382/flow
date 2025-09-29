#pragma once
#include <deque>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include "core/message.hpp"

/**
 * TSPrioQueue：多生产者 / 单消费者 MPSC 三档优先级队列，支持背压策略
 * - 容量：max_size_（0 表示无限）
 * - 策略：DropNewest / DropOldest / BlockWithTimeout
 */
class TSPrioQueue {
public:
    enum class DropPolicy { DropNewest, DropOldest, BlockWithTimeout };

    explicit TSPrioQueue(size_t max_size = 0,
                         DropPolicy policy = DropPolicy::DropNewest,
                         int block_timeout_ms = 100)
        : closed_(false), max_size_(max_size), policy_(policy), block_timeout_ms_(block_timeout_ms)
    {}

    void push(const Message& m){ push_impl(m); }
    void push(Message&& m){ push_impl(std::move(m)); }

    bool pop_blocking(Message& out){
        std::unique_lock<std::mutex> lk(mu_);
        cv_.wait(lk, [this]{ return closed_ || !empty_nolock(); });
        if (closed_ && empty_nolock()) return false;
        for (int i=0;i<3;++i){
            if (!qs_[i].empty()){
                out = std::move(qs_[i].front());
                qs_[i].pop_front();
                return true;
            }
        }
        return false;
    }

    void close(){
        std::unique_lock<std::mutex> lk(mu_);
        closed_ = true;
        cv_.notify_all();
    }

    size_t size() const {
        std::unique_lock<std::mutex> lk(mu_);
        return qs_[0].size() + qs_[1].size() + qs_[2].size();
    }

private:
    template<typename T>
    void push_impl(T&& m){
        std::unique_lock<std::mutex> lk(mu_);
        if (closed_) return;

        auto& q = q_by_pri(m);
        // 背压：若设置了容量，检查总大小
        if (max_size_>0){
            size_t total = qs_[0].size()+qs_[1].size()+qs_[2].size();
            if (total >= max_size_){
                if (policy_ == DropPolicy::DropNewest){
                    // 丢当前（新）消息
                    return;
                } else if (policy_ == DropPolicy::DropOldest){
                    // 丢最旧（低优先级起）
                    for (int i=2;i>=0;--i){
                        if (!qs_[i].empty()){ qs_[i].pop_front(); break; }
                    }
                } else { // BlockWithTimeout
                    if (!cv_space_.wait_for(lk, std::chrono::milliseconds(block_timeout_ms_), [this]{
                        return closed_ || (qs_[0].size()+qs_[1].size()+qs_[2].size()) < max_size_;
                    })){
                        // 超时仍满：丢新
                        return;
                    }
                    if (closed_) return;
                }
            }
        }
        q.push_back(std::forward<T>(m));
        cv_.notify_one();
    }

    static int pri(const Message& m){
        int p = m.header.priority;
        if (p<=PRIO_HIGH) return 0;
        if (p>=PRIO_LOW ) return 2;
        return 1;
    }
    std::deque<Message>& q_by_pri(const Message& m){ return qs_[pri(m)]; }
    bool empty_nolock() const { return qs_[0].empty() && qs_[1].empty() && qs_[2].empty(); }

    mutable std::mutex mu_;
    std::condition_variable cv_;
    std::condition_variable cv_space_;
    std::deque<Message> qs_[3];
    bool closed_;
    size_t max_size_;
    DropPolicy policy_;
    int block_timeout_ms_;
};

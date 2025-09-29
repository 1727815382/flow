#pragma once
#include <atomic>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <unordered_map>
#include <functional>
#include <chrono>
#include <string>

#include "core/itask.hpp"
#include "core/message.hpp"
#include "core/ts_prio_queue.hpp"
#include "core/message_bus.hpp"

// -------------------- 任务与线程枚举 --------------------
enum TaskStatus  { TS_INIT, TS_RUNNING, TS_IDLE, TS_ERROR, TS_STOPPED, TS_WAITING, TS_SCHEDULED };
enum ThreadPrio  { TP_LOW, TP_NORMAL, TP_HIGH, TP_REALTIME };

// 队列背压策略
enum BackpressurePolicy { BP_DROP_NEWEST, BP_BLOCK_TIMEOUT };

// -------------------- TaskBase --------------------
class TaskBase : public ITask {
public:
    explicit TaskBase(const std::string& id, MessageBus* bus);
    virtual ~TaskBase();

    // 基本信息
    const std::string& id() const { return id_; }
    TaskStatus status() const { return status_; }

    // 生命周期（由外部/MessageBus 调用）
    void start() override;
    void stop()  override;

    // 入队（由 MessageBus 调用；带背压）
    void post(const Message& m) override;
    void post(Message&& m) override;

    // 线程优先级（可选）
    void setThreadPriority(ThreadPrio p);

    // 背压配置：容量上限与策略（默认：容量4096，BLOCK_TIMEOUT，1秒）
    void setQueueBackpressure(size_t max_size,
                              BackpressurePolicy policy = BP_BLOCK_TIMEOUT,
                              int block_timeout_ms = 1000);

    // ======= ask/应答（同步等待）=======
    // 发送请求并等待应答，按 correlation_id 匹配；超时返回 false
    bool ask(const std::string& to,
             const std::string& type,
             const QJsonObject& payload,
             int timeout_ms,
             Message* reply_out);

    // 回复：把 correlation_id 原样带回，回到请求方
    void reply(const Message& req, const std::string& type, const QJsonObject& payload);

    // ======= 统一定时器 =======
    // 设置定时任务：delay_ms 后触发一次；若 repeat>1 且 interval_ms>0，则重复触发 repeat 次（repeat<0 表示无限）
    // 回调最终在 Worker 线程执行（通过内部 __timer.fire 消息）
    size_t setTimer(int delay_ms,
                    int repeat = 1,
                    int interval_ms = 0,
                    std::function<void()> fn = std::function<void()>());

    bool cancelTimer(size_t timer_id);

protected:
    // —— 派生类可重写 —— //
    virtual void onStart() {}          // Worker 启动前
    virtual void onStop()  {}          // Worker 停止后
    virtual void onMessage(Message& m) = 0;   // 业务消息入口
    virtual int  heartbeatIntervalMs() const { return 2000; }

    // 发到总线：优先使用 header.to；否则用 header.topic 发布
    void sendToBus(Message&& m);

    // 状态与异常上报
    void reportStatus(const char* phase, const char* err = "");
    void reportException(const std::string& where, const std::string& what);

    // 纯 C++11 时钟（steady：单调；system：墙钟）
    static inline long long tick_ms() {
        using namespace std::chrono;
        return (long long)duration_cast<milliseconds>(steady_clock::now().time_since_epoch()).count();
    }
    static inline long long wall_ms() {
        using namespace std::chrono;
        return (long long)duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
    }

protected:
    MessageBus* bus_;
    TaskStatus  status_;
    TSPrioQueue inbox_;

private:
    // ============ Worker/Heartbeat ============
    void workerLoop();
    void heartbeatLoop();

    // ============ 定时器 ============
    struct TimerItem {
        size_t id;
        long long due_ms;
        int repeat;             // <0 无限；=1 只触发1次；>1 触发repeat次
        int interval_ms;        // 重复间隔（>0 有效）
        bool cancelled;
        bool operator<(const TimerItem& rhs) const {
            // std::priority_queue 是大顶堆，反转让最早的在 top()
            return due_ms > rhs.due_ms;
        }
    };
    void timerLoop();
    void scheduleTimerUnsafe(const TimerItem& it);
    void notifyTimerThread();

    // ============ ask/应答 ============
    struct PendingReq {
        std::mutex m;
        std::condition_variable cv;
        bool done = false;
        Message reply;
        long long deadline_ms = 0;
        bool cancelled = false;
    };
    bool interceptAskReply_(Message& m);    // 在 workerLoop 中优先拦截应答
    std::string nextCorrelationId_();

    // ============ 背压 ============
    bool pushWithBackpressure_(Message&& m);

    // ============ 平台线程优先级 ============
    static void setNativePriority_(std::thread& th, ThreadPrio p);

private:
    // 标识、线程、运行状态
    std::string id_;
    std::atomic<bool> running_;
    std::thread worker_;
    std::thread heartbeat_;
    ThreadPrio threadPrio_;

    // 统一定时器线程与结构
    std::atomic<bool> timers_running_;
    std::thread timer_thread_;
    std::mutex timers_mu_;
    std::condition_variable timers_cv_;
    std::priority_queue<TimerItem> timers_;
    std::unordered_map<size_t, std::function<void()>> timer_callbacks_;
    std::unordered_map<size_t, TimerItem> timers_index_;
    std::atomic<size_t> timer_id_seq_;

    // ask/应答
    std::mutex pending_mu_;
    std::unordered_map<std::string, std::shared_ptr<PendingReq>> pendings_;
    std::atomic<uint64_t> corr_seq_;

    // 背压配置
    std::atomic<size_t>         q_max_;
    std::atomic<int>            q_block_timeout_ms_;
    std::atomic<int>            q_policy_; // BackpressurePolicy

    // 统计（可作为指标来源）
    std::atomic<uint64_t> msg_in_;
    std::atomic<uint64_t> msg_drop_;
};

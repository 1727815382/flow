#include "core/task_base.hpp"
#include <cassert>
#include <QVariant>
#ifdef _WIN32
  #include <windows.h>
  #include <processthreadsapi.h>
#else
  #include <pthread.h>
  #include <sched.h>
#endif

// ============ 构造/析构 ============
TaskBase::TaskBase(const std::string& id, MessageBus* bus)
    : bus_(bus),
      status_(TS_INIT),
      running_(false),
      threadPrio_(TP_NORMAL),
      timers_running_(false),
      timer_id_seq_(1),
      corr_seq_(1),
      q_max_(4096),
      q_block_timeout_ms_(1000),
      q_policy_((int)BP_BLOCK_TIMEOUT),
      msg_in_(0),
      msg_drop_(0),
      id_(id) {}

TaskBase::~TaskBase() {
    stop(); // 允许外部漏调 stop() 时也能安全回收
}

// ============ 生命周期 ============
void TaskBase::start() {
    bool expected = false;
    if (!running_.compare_exchange_strong(expected, true)) return;

    status_ = TS_RUNNING;
    onStart();

    // Worker
    worker_ = std::thread(&TaskBase::workerLoop, this);
    setNativePriority_(worker_, threadPrio_);

    // Heartbeat
    heartbeat_ = std::thread(&TaskBase::heartbeatLoop, this);

    // Timers
    timers_running_ = true;
    timer_thread_ = std::thread(&TaskBase::timerLoop, this);

    reportStatus("started", "");
}

void TaskBase::stop() {
    if (!running_.exchange(false)) return;

    // 结束队列
    inbox_.close();

    // 停定时器
    {
        std::lock_guard<std::mutex> lk(timers_mu_);
        timers_running_ = false;
        while (!timers_.empty()) timers_.pop();
        timers_index_.clear();
        timer_callbacks_.clear();
    }
    timers_cv_.notify_all();

    // 清理 ask/应答等待者
    {
        std::lock_guard<std::mutex> lk(pending_mu_);
        for (auto& kv : pendings_) {
            auto p = kv.second;
            {
                std::lock_guard<std::mutex> lk2(p->m);
                p->cancelled = true;
                p->done = true;
            }
            p->cv.notify_all();
        }
        pendings_.clear();
    }

    if (worker_.joinable())    worker_.join();
    if (heartbeat_.joinable()) heartbeat_.join();
    if (timer_thread_.joinable()) timer_thread_.join();

    status_ = TS_STOPPED;
    onStop();
    reportStatus("stopped", "");
}

// ============ post（带背压） ============
void TaskBase::post(const Message& m)  { Message c = m; pushWithBackpressure_(std::move(c)); }
void TaskBase::post(Message&& m)       { pushWithBackpressure_(std::move(m)); }

bool TaskBase::pushWithBackpressure_(Message&& m) {
    // 尽量用 TSPrioQueue::size() 做简单判定（已带互斥）
    const size_t cur = inbox_.size();
    const size_t cap = q_max_.load();
    if (cap > 0 && cur >= cap) {
        if ((BackpressurePolicy)q_policy_.load() == BP_BLOCK_TIMEOUT) {
            // 简单阻塞等待：轮询 + sleep（避免引入复杂接口）
            const long long start = tick_ms();
            const int to_ms = q_block_timeout_ms_.load();
            while (inbox_.size() >= cap && (tick_ms() - start) < to_ms) {
                std::this_thread::sleep_for(std::chrono::milliseconds(1));
            }
            if (inbox_.size() >= cap) {
                // 超时，丢弃最新（记录 drop）
                msg_drop_.fetch_add(1);
                reportException("backpressure", "queue full; drop newest");
                return false;
            }
        } else { // BP_DROP_NEWEST
            msg_drop_.fetch_add(1);
            // 丢弃但打点
            reportException("backpressure", "queue full; drop newest");
            return false;
        }
    }
    inbox_.push(std::move(m));
    msg_in_.fetch_add(1);
    return true;
}

void TaskBase::setQueueBackpressure(size_t max_size,
                                    BackpressurePolicy policy,
                                    int block_timeout_ms) {
    q_max_.store(max_size);
    q_policy_.store((int)policy);
    q_block_timeout_ms_.store(block_timeout_ms > 0 ? block_timeout_ms : 1);
}

// ============ 发送到总线 ============
void TaskBase::sendToBus(Message&& m) {
    if (!bus_) return;
    if (!m.header.to.empty()) {
        bus_->sendTo(m.header.to, std::move(m));
    } else if (!m.header.topic.empty()) {
        bus_->publish(m.header.topic, std::move(m));
    } else {
        // 做个容错：如果都没填，把 to=supervisor+type=supervisor.exception
        Message s;
        s.header.type = "supervisor.exception";
        s.header.from = id_;
        s.header.to   = "supervisor";
        QJsonObject o;
        o.insert("where", QString("sendToBus"));
        o.insert("what",  QString("empty to/topic"));
        s.payload = o;
        bus_->sendTo("supervisor", std::move(s));
    }
}

// ============ 状态/异常 ============
void TaskBase::reportStatus(const char* phase, const char* err) {
    if (!bus_) return;
    Message s;
    s.header.type = "supervisor.status";
    s.header.from = id_;
    s.header.to   = "supervisor";
    s.header.ts_epoch_ms = wall_ms();

    QJsonObject o;
    o.insert("phase", QString::fromUtf8(phase ? phase : ""));
    o.insert("queue", (int)inbox_.size());
    o.insert("status", (int)status_);
    o.insert("error", QString::fromUtf8(err ? err : ""));
    o.insert("msg_in",  (double)msg_in_.load());
    o.insert("msg_drop",(double)msg_drop_.load());
    s.payload = o;

    bus_->sendTo("supervisor", std::move(s));
}

void TaskBase::reportException(const std::string& where, const std::string& what) {
    status_ = TS_ERROR;
    if (!bus_) return;
    Message s;
    s.header.type = "supervisor.exception";
    s.header.from = id_;
    s.header.to   = "supervisor";
    s.header.ts_epoch_ms = wall_ms();

    QJsonObject o;
    o.insert("where", QString::fromStdString(where));
    o.insert("what",  QString::fromStdString(what));
    s.payload = o;

    bus_->sendTo("supervisor", std::move(s));
}

// ============ Worker/Heartbeat ============
void TaskBase::workerLoop() {
    try {
        Message m;
        while (running_) {
            if (!inbox_.pop_blocking(m)) break;

            // 1) 拦截 ask/应答
            if (interceptAskReply_(m)) {
                continue;
            }

            // 2) 拦截内部定时器触发（__timer.fire）
            if (m.header.type == "__timer.fire") {
                // payload: {"id": <timer_id>}
                const size_t tid = (size_t)m.payload.value("id").toVariant().toULongLong();
                std::function<void()> fn;
                {
                    std::lock_guard<std::mutex> lk(timers_mu_);
                    auto it = timer_callbacks_.find(tid);
                    if (it != timer_callbacks_.end()) fn = it->second;
                }
                if (fn) {
                    try { fn(); }
                    catch (const std::exception& e) { reportException("__timer.fire", e.what()); }
                    catch (...) { reportException("__timer.fire", "unknown"); }
                }
                continue;
            }

            // 3) 常规业务
            try {
                onMessage(m);
            } catch (const std::exception& e) {
                reportException("onMessage", e.what());
            } catch (...) {
                reportException("onMessage", "unknown");
            }
        }
    } catch (...) {
        reportException("workerLoop", "fatal");
    }
}

void TaskBase::heartbeatLoop() {
    const int iv = std::max(1, heartbeatIntervalMs());
    while (running_) {
        reportStatus("heartbeat", "");
        std::this_thread::sleep_for(std::chrono::milliseconds(iv));
    }
}

// ============ 定时器线程 ============
void TaskBase::timerLoop() {
    std::unique_lock<std::mutex> lk(timers_mu_);
    while (timers_running_) {
        if (timers_.empty()) {
            timers_cv_.wait(lk, [this]{ return !timers_running_ || !timers_.empty(); });
            if (!timers_running_) break;
        } else {
            auto next = timers_.top();
            const long long now = tick_ms();
            if (next.cancelled) {
                timers_.pop();
                timers_index_.erase(next.id);
                continue;
            }
            if (next.due_ms > now) {
                const auto wait_ms = std::max<long long>(1, next.due_ms - now);
                timers_cv_.wait_for(lk, std::chrono::milliseconds(wait_ms));
                continue; // 重新判断
            }

            // 到期：发内部消息到 Worker 执行回调
            timers_.pop();
            lk.unlock();
            {
                Message fire;
                fire.header.type = "__timer.fire";
                fire.header.to   = id_;
                QJsonObject p; p.insert("id", (double)next.id);
                fire.payload = p;
                post(std::move(fire)); // 走本任务队列
            }
            lk.lock();

            if (next.cancelled) {
                timers_index_.erase(next.id);
                continue;
            }

            // 重复调度
            if (next.repeat < 0 || next.repeat > 1) {
                if (next.repeat > 1) next.repeat -= 1;
                next.due_ms = tick_ms() + std::max(1, next.interval_ms);
                timers_index_[next.id] = next;
                timers_.push(next);
            } else {
                timers_index_.erase(next.id);
            }
        }
    }
}

void TaskBase::scheduleTimerUnsafe(const TimerItem& it) {
    timers_.push(it);
    timers_index_[it.id] = it;
}

void TaskBase::notifyTimerThread() {
    timers_cv_.notify_all();
}

size_t TaskBase::setTimer(int delay_ms, int repeat, int interval_ms, std::function<void()> fn) {
    if (delay_ms < 0) delay_ms = 0;
    if (repeat == 0)  repeat = 1;
    if (repeat != 1 && interval_ms <= 0) interval_ms = std::max(1, delay_ms);

    const size_t tid = (size_t)timer_id_seq_.fetch_add(1);
    {
        std::lock_guard<std::mutex> lk(timers_mu_);
        TimerItem it;
        it.id = tid;
        it.due_ms = tick_ms() + delay_ms;
        it.repeat = repeat;
        it.interval_ms = interval_ms;
        it.cancelled = false;
        timer_callbacks_[tid] = std::move(fn);
        scheduleTimerUnsafe(it);
    }
    notifyTimerThread();
    return tid;
}

bool TaskBase::cancelTimer(size_t timer_id) {
    std::lock_guard<std::mutex> lk(timers_mu_);
    auto it = timers_index_.find(timer_id);
    if (it == timers_index_.end()) return false;
    TimerItem &node = it->second;
    node.cancelled = true;
    // 不立即从优先队列移除（lazy removal），在 timerLoop 取出时会跳过
    timer_callbacks_.erase(timer_id);
    notifyTimerThread();
    return true;
}

// ============ ask/应答 ============
std::string TaskBase::nextCorrelationId_() {
    uint64_t seq = corr_seq_.fetch_add(1);
    return id_ + "-" + std::to_string(seq);
}

bool TaskBase::ask(const std::string& to,
                   const std::string& type,
                   const QJsonObject& payload,
                   int timeout_ms,
                   Message* reply_out) {
    if (!bus_) return false;

    const std::string cid = nextCorrelationId_();
    auto pr = std::make_shared<PendingReq>();
    pr->deadline_ms = tick_ms() + std::max(1, timeout_ms);

    {
        std::lock_guard<std::mutex> lk(pending_mu_);
        pendings_.emplace(cid, pr);
    }

    // 组装请求并发送
    Message req;
    req.header.type = type;
    req.header.from = id_;
    req.header.to   = to;
    req.header.correlation_id = cid;
    req.header.ts_epoch_ms = wall_ms();
    req.payload = payload;

    bus_->sendTo(to, req);

    // 等待
    std::unique_lock<std::mutex> lk(pr->m);
    while (!pr->done && !pr->cancelled) {
        const long long now = tick_ms();
        if (now >= pr->deadline_ms) break;
        const auto left = std::chrono::milliseconds(pr->deadline_ms - now);
        pr->cv.wait_for(lk, left);
    }

    bool ok = pr->done && !pr->cancelled;
    if (ok && reply_out) *reply_out = pr->reply;

    // 清理
    {
        std::lock_guard<std::mutex> lk2(pending_mu_);
        pendings_.erase(cid);
    }
    return ok;
}

void TaskBase::reply(const Message& req, const std::string& type, const QJsonObject& payload) {
    if (!bus_) return;
    Message r;
    r.header.type = type;
    r.header.from = id_;
    r.header.to   = req.header.from;            // 回到请求方
    r.header.correlation_id = req.header.correlation_id; // 必须原样带回
    r.header.ts_epoch_ms = wall_ms();
    r.payload = payload;
    bus_->sendTo(r.header.to, r);
}

bool TaskBase::interceptAskReply_(Message& m) {
    if (m.header.correlation_id.empty()) return false;

    std::shared_ptr<PendingReq> pr;
    {
        std::lock_guard<std::mutex> lk(pending_mu_);
        auto it = pendings_.find(m.header.correlation_id);
        if (it == pendings_.end()) return false;
        pr = it->second;
    }
    {
        std::lock_guard<std::mutex> lk(pr->m);
        pr->reply = m;
        pr->done  = true;
    }
    pr->cv.notify_all();
    return true;
}

// ============ 平台线程优先级 ============
void TaskBase::setThreadPriority(ThreadPrio p) {
    threadPrio_ = p;
    setNativePriority_(worker_, p);
}

void TaskBase::setNativePriority_(std::thread& th, ThreadPrio p) {
#ifdef _WIN32
    if (!th.joinable()) return;
    HANDLE h = (HANDLE)th.native_handle();
    int wpri = THREAD_PRIORITY_NORMAL;
    switch (p) {
        case TP_LOW:      wpri = THREAD_PRIORITY_BELOW_NORMAL; break;
        case TP_HIGH:     wpri = THREAD_PRIORITY_ABOVE_NORMAL; break;
        case TP_REALTIME: wpri = THREAD_PRIORITY_TIME_CRITICAL; break;
        default:          wpri = THREAD_PRIORITY_NORMAL; break;
    }
    ::SetThreadPriority(h, wpri);
#else
    if (!th.joinable()) return;
    int policy = SCHED_OTHER;
    sched_param sp; sp.sched_priority = 0;
    if (p == TP_REALTIME) {
        policy = SCHED_FIFO; sp.sched_priority = 10;
        if (pthread_setschedparam(th.native_handle(), policy, &sp) != 0) {
            policy = SCHED_OTHER; sp.sched_priority = 0;
            pthread_setschedparam(th.native_handle(), policy, &sp);
        }
    } else {
        pthread_setschedparam(th.native_handle(), policy, &sp);
    }
#endif
}

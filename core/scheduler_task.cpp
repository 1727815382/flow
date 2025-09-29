// core/scheduler_task.cpp
#include "core/scheduler_task.hpp"
#include <QDateTime>
#include <cstdlib>
#include <ctime>

static long long now_ms_qt(){ return (long long)QDateTime::currentMSecsSinceEpoch(); }
long long SchedulerTask::nowMs(){ return now_ms_qt(); }

long long SchedulerTask::todayMs(int hh, int mm, int ss){
    QDateTime now = QDateTime::currentDateTime();
    QDate d = now.date();
    QTime t(hh, mm, ss);
    QDateTime dt(d, t);
    return (long long)dt.toMSecsSinceEpoch();
}

void SchedulerTask::onStart(){
    status_ = TS_RUNNING;
    std::srand((unsigned int)std::time(0));
}

long long SchedulerTask::jittered(long long base, int jitter_ms){
    if (jitter_ms<=0) return base;
    int r = std::rand();
    int span = (jitter_ms*2+1);
    int delta = (r % span) - jitter_ms;
    return base + delta;
}

static std::string json_gets(const QJsonObject& o, const char* k, const char* defv){
    if (o.contains(k)) return o.value(k).toString().toStdString();
    return std::string(defv);
}
static int json_geti(const QJsonObject& o, const char* k, int defv){
    if (o.contains(k)) return o.value(k).toInt(defv);
    return defv;
}
static long long json_getll(const QJsonObject& o, const char* k, long long defv){
    if (o.contains(k)) return o.value(k).toVariant().toLongLong();
    return defv;
}

void SchedulerTask::pushJob(const Job& j){ jobs_[j.id]=j; heap_.push(j); }

void SchedulerTask::addOnce(const QJsonObject& o){
    Job j;
    j.id = json_gets(o,"id","job_once");
    j.next_ms = json_getll(o, "when_ms", nowMs());
    j.msg = o.value("msg").toObject();
    j.period_ms = 0; j.fixed_rate=false; j.paused=false;
    j.catch_up = o.contains("catch_up") ? o.value("catch_up").toBool() : false;
    j.jitter_ms = json_geti(o,"jitter_ms",0);
    j.left_runs = json_geti(o,"left_runs",1);
    j.priority = json_geti(o,"priority",1);
    j.next_ms = jittered(j.next_ms, j.jitter_ms);
    pushJob(j);
}
void SchedulerTask::addDelay(const QJsonObject& o){
    Job j;
    j.id = json_gets(o,"id","job_delay");
    j.next_ms = nowMs() + json_geti(o, "delay_ms", 0);
    j.msg = o.value("msg").toObject();
    j.period_ms=0; j.fixed_rate=false; j.paused=false;
    j.catch_up=false; j.jitter_ms=json_geti(o,"jitter_ms",0);
    j.left_runs = json_geti(o,"left_runs",1);
    j.priority = json_geti(o,"priority",1);
    j.next_ms = jittered(j.next_ms, j.jitter_ms);
    pushJob(j);
}
void SchedulerTask::addInterval(const QJsonObject& o){
    Job j; j.id=json_gets(o,"id","job_interval");
    j.period_ms = json_geti(o,"period_ms",1000);
    j.next_ms = nowMs() + j.period_ms;
    j.msg = o.value("msg").toObject();
    j.fixed_rate=false; j.paused=false;
    j.catch_up=false; j.jitter_ms=json_geti(o,"jitter_ms",0);
    j.left_runs = json_geti(o,"left_runs",0);
    j.priority = json_geti(o,"priority",1);
    j.next_ms = jittered(j.next_ms, j.jitter_ms);
    pushJob(j);
}
void SchedulerTask::addFixedRate(const QJsonObject& o){
    Job j; j.id=json_gets(o,"id","job_fixedrate");
    j.period_ms = json_geti(o,"period_ms",1000);
    j.next_ms = nowMs() + j.period_ms;
    j.msg = o.value("msg").toObject();
    j.fixed_rate=true; j.paused=false;
    j.catch_up = o.contains("catch_up") ? o.value("catch_up").toBool() : false;
    j.jitter_ms=json_geti(o,"jitter_ms",0);
    j.left_runs = json_geti(o,"left_runs",0);
    j.priority = json_geti(o,"priority",1);
    j.next_ms = jittered(j.next_ms, j.jitter_ms);
    pushJob(j);
}
void SchedulerTask::addDaily(const QJsonObject& o){
    int hh=json_geti(o,"hh",0), mm=json_geti(o,"mm",0), ss=json_geti(o,"ss",0);
    long long t = todayMs(hh,mm,ss);
    long long now = nowMs();
    if (t<=now) t += 24LL*3600*1000;
    Job j; j.id=json_gets(o,"id","job_daily");
    j.next_ms = t;
    j.msg=o.value("msg").toObject();
    j.period_ms = 24LL*3600*1000;
    j.fixed_rate=true; j.paused=false;
    j.catch_up = o.contains("catch_up") ? o.value("catch_up").toBool() : false;
    j.jitter_ms=json_geti(o,"jitter_ms",0);
    j.left_runs=json_geti(o,"left_runs",0);
    j.priority = json_geti(o,"priority",1);
    j.next_ms = jittered(j.next_ms, j.jitter_ms);
    pushJob(j);
}
void SchedulerTask::addWeekly(const QJsonObject& o){
    int dow = json_geti(o,"dow",0);
    int hh=json_geti(o,"hh",0), mm=json_geti(o,"mm",0), ss=json_geti(o,"ss",0);
    QDateTime now = QDateTime::currentDateTime();
    int qtDow = now.date().dayOfWeek(); // 1=Mon..7=Sun
    int sun0 = (qtDow%7);
    int delta = dow - sun0; if (delta<0) delta += 7;
    long long base = todayMs(0,0,0) + delta*24LL*3600*1000;
    QDateTime baseDt = QDateTime::fromMSecsSinceEpoch(base);
    QDate d = baseDt.date();
    QTime t(hh,mm,ss);
    long long target = QDateTime(d,t).toMSecsSinceEpoch();
    if (target <= now.toMSecsSinceEpoch()) target += 7LL*24*3600*1000;

    Job j; j.id=json_gets(o,"id","job_weekly");
    j.next_ms = target;
    j.msg = o.value("msg").toObject();
    j.period_ms = 7LL*24*3600*1000;
    j.fixed_rate=true; j.paused=false;
    j.catch_up = o.contains("catch_up") ? o.value("catch_up").toBool() : false;
    j.jitter_ms=json_geti(o,"jitter_ms",0);
    j.left_runs=json_geti(o,"left_runs",0);
    j.priority = json_geti(o,"priority",1);
    j.next_ms = jittered(j.next_ms, j.jitter_ms);
    pushJob(j);
}
void SchedulerTask::addChain(const QJsonObject& o){
    QJsonArray arr = o.value("steps").toArray();
    std::string baseId = json_gets(o,"id","job_chain");
    int prio = json_geti(o,"priority",1);
    long long base = nowMs();
    for (int i=0;i<arr.size();++i){
        QJsonObject s = arr[i].toObject();
        Job j; char buf[64]; std::sprintf(buf, "%s_%d", baseId.c_str(), i+1);
        j.id = buf;
        j.next_ms = base + json_geti(s,"delay_ms",0);
        j.msg = s.value("msg").toObject();
        j.period_ms=0; j.fixed_rate=false; j.paused=false;
        j.catch_up=false; j.jitter_ms=json_geti(s,"jitter_ms",0);
        j.left_runs=1; j.priority = json_geti(s,"priority",prio);
        j.next_ms = jittered(j.next_ms, j.jitter_ms);
        pushJob(j);
    }
}
void SchedulerTask::cancel(const QJsonObject& o){
    std::string id = json_gets(o,"id","");
    std::map<std::string, Job>::iterator it=jobs_.find(id);
    if (it!=jobs_.end()) jobs_.erase(it);
}
void SchedulerTask::pauseJob(const QJsonObject& o){
    std::string id = json_gets(o,"id","");
    std::map<std::string, Job>::iterator it=jobs_.find(id);
    if (it!=jobs_.end()) { Job j=it->second; j.paused=true; it->second=j; }
}
void SchedulerTask::resumeJob(const QJsonObject& o){
    std::string id = json_gets(o,"id","");
    std::map<std::string, Job>::iterator it=jobs_.find(id);
    if (it!=jobs_.end()) { Job j=it->second; j.paused=false; it->second=j; heap_.push(j); }
}

void SchedulerTask::sendJob(const Job& j){
    Message out;
    out.header.from = id();
    out.header.type = json_gets(j.msg,"_type","");
    out.header.to   = json_gets(j.msg,"_to","");
    out.header.topic= json_gets(j.msg,"_topic","");
    out.header.priority = json_geti(j.msg,"priority", j.priority);
    out.header.ts_epoch_ms = nowMs();
    out.payload = j.msg.value("payload").toObject();
    sendToBus(std::move(out));
}

void SchedulerTask::scheduleNext(Job j, long long fired_ms){
    if (j.period_ms<=0){
        if (j.left_runs>1){ j.left_runs--; j.next_ms = nowMs() + j.period_ms; pushJob(j);}
        return;
    }
    if (j.left_runs>0){ j.left_runs--; if (j.left_runs==0) { jobs_.erase(j.id); return; } }

    long long next_plan;
    if (j.fixed_rate){
        next_plan = j.next_ms + j.period_ms;
        if (!j.catch_up){
            long long now = nowMs();
            while (next_plan <= now) next_plan += j.period_ms;
        }
    } else {
        next_plan = fired_ms + j.period_ms;
    }
    j.next_ms = jittered(next_plan, j.jitter_ms);
    jobs_[j.id] = j;
    heap_.push(j);
}

void SchedulerTask::tick(){
    long long now = nowMs();
    while(!heap_.empty()){
        Job top = heap_.top();
        std::map<std::string, Job>::iterator it = jobs_.find(top.id);
        if (it==jobs_.end()){ heap_.pop(); continue; }
        Job cur = it->second;
        if (cur.paused){ heap_.pop(); continue; }
        if (cur.next_ms != top.next_ms){ heap_.pop(); continue; }
        if (top.next_ms > now) break;

        heap_.pop();
        sendJob(cur);
        scheduleNext(cur, now);
    }
}

void SchedulerTask::onMessage(Message& m){
    const std::string& t=m.header.type;
    if (t=="sched.add.once") addOnce(m.payload);
    else if (t=="sched.add.delay") addDelay(m.payload);
    else if (t=="sched.add.interval") addInterval(m.payload);
    else if (t=="sched.add.fixedrate") addFixedRate(m.payload);
    else if (t=="sched.add.daily") addDaily(m.payload);
    else if (t=="sched.add.weekly") addWeekly(m.payload);
    else if (t=="sched.add.chain") addChain(m.payload);
    else if (t=="sched.cancel") cancel(m.payload);
    else if (t=="sched.pause") pauseJob(m.payload);
    else if (t=="sched.resume") resumeJob(m.payload);
    else if (t=="sched.tick") tick();
}

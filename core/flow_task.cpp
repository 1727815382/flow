// core/flow_task.cpp
#include "core/flow_task.hpp"
#include <QDateTime>
#include <QJsonDocument>

static long long now_ms_qt(){ return (long long)QDateTime::currentMSecsSinceEpoch(); }
long long FlowTask::nowMs(){ return now_ms_qt(); }

static std::string jgets(const QJsonObject& o, const char* k, const char* defv){
    if (o.contains(k)) return o.value(k).toString().toStdString();
    return std::string(defv);
}
static int jgeti(const QJsonObject& o, const char* k, int defv){
    if (o.contains(k)) return o.value(k).toInt(defv);
    return defv;
}

void FlowTask::loadFromJson(const QJsonObject& o){
    steps_.clear();
    QJsonArray arr = o.value("steps").toArray();
    for (int i=0;i<arr.size();++i){
        QJsonObject so = arr[i].toObject();
        StepDef s;
        s.name = jgets(so,"name","");
        s.timeout_ms = jgeti(so,"timeout_ms",0);
        s.max_retry  = jgeti(so,"max_retry",0);
        s.onEnterDelayMs = jgeti(so,"onEnterDelayMs",0);
        s.onEnterMsgs = so.value("onEnterMsgs").toArray();
        s.onExitMsgs  = so.value("onExitMsgs").toArray();
        QJsonObject w = so.value("wait").toObject();
        QJsonArray anyA = w.value("anyOf").toArray();
        for (int k=0;k<anyA.size();++k) s.wait.anyOfTypes.push_back(anyA[k].toString().toStdString());
        QJsonArray allA = w.value("allOf").toArray();
        for (int k=0;k<allA.size();++k) s.wait.allOfTypes.push_back(allA[k].toString().toStdString());
        QJsonObject nb = so.value("nextByEvent").toObject();
        QStringList keys = nb.keys();
        for (int k=0;k<keys.size();++k){
            s.nextByEvent[keys[k].toStdString()] = nb.value(keys[k]).toString().toStdString();
        }
        s.nextDefault = jgets(so,"nextDefault","");
        s.nextOnTimeout = jgets(so,"nextOnTimeout","");
        s.nextOnFail = jgets(so,"nextOnFail","");
        if (!s.name.empty()) steps_[s.name] = s;
    }
    Message status; status.header.type="supervisor.status"; status.header.from=id(); status.header.to="supervisor";
    QJsonObject o2; o2.insert("phase","flow.loaded"); o2.insert("queue",0); o2.insert("status",(int)TS_RUNNING);
    status.payload = o2;
    sendToBus(std::move(status));
}

void FlowTask::fireMsgs(const QJsonArray& arr){
    for (int i=0;i<arr.size();++i){
        QJsonObject o = arr[i].toObject();
        Message out;
        out.header.from = id();
        out.header.type = jgets(o,"_type","");
        out.header.to   = jgets(o,"_to","");
        out.header.topic= jgets(o,"_topic","");
        out.header.priority = jgeti(o,"priority",1);
        out.header.ts_epoch_ms = nowMs();
        out.payload = o.value("payload").toObject();
        sendToBus(std::move(out));
    }
}

void FlowTask::gotoStep(const std::string& name){
    if (!step_.empty()){
        std::map<std::string, StepDef>::iterator it=steps_.find(step_);
        if (it!=steps_.end()) fireMsgs(it->second.onExitMsgs);
    }
    step_ = name;
    stepStart_ = nowMs();
    retryCount_ = 0;
    fulfilled_.clear();
    lastEventType_.clear();

    std::map<std::string, StepDef>::iterator it=steps_.find(step_);
    if (it!=steps_.end()){
        int delay = it->second.onEnterDelayMs;
        if (delay>0){
            std::thread th([this, delay](){
                std::this_thread::sleep_for(std::chrono::milliseconds(delay));
                Message m; m.header.type="flow._onenter"; m.header.from=this->id(); this->post(m);
            });
            th.detach();
        } else {
            fireMsgs(it->second.onEnterMsgs);
        }
    }
    Message s; s.header.type="supervisor.status"; s.header.from=id(); s.header.to="supervisor";
    QJsonObject o; o.insert("phase","flow.step"); o.insert("step", QString::fromStdString(step_)); o.insert("status",(int)TS_RUNNING);
    s.payload = o;
    sendToBus(std::move(s));
}

void FlowTask::handleEvent(Message& m){
    if (m.header.type=="flow._onenter"){
        std::map<std::string, StepDef>::iterator it=steps_.find(step_);
        if (it!=steps_.end()) fireMsgs(it->second.onEnterMsgs);
        return;
    }
    lastEventType_ = m.header.type;
    if (!step_.empty()){
        std::map<std::string, StepDef>::iterator it=steps_.find(step_);
        if (it!=steps_.end()){
            for (size_t i=0;i<it->second.wait.allOfTypes.size();++i){
                if (lastEventType_ == it->second.wait.allOfTypes[i]){
                    fulfilled_.insert(lastEventType_);
                    break;
                }
            }
        }
    }
}

void FlowTask::evaluate(){
    if (step_.empty()) return;
    std::map<std::string, StepDef>::iterator it=steps_.find(step_);
    if (it==steps_.end()) return;
    StepDef& st = it->second;
    long long now = nowMs();

    if (st.timeout_ms>0 && now - stepStart_ >= st.timeout_ms){
        if (!st.nextOnTimeout.empty()){ gotoStep(st.nextOnTimeout); return; }
        if (st.max_retry>0 && retryCount_<st.max_retry){ retryCount_++; gotoStep(st.name); return; }
        if (!st.nextDefault.empty()){ gotoStep(st.nextDefault); return; }
        return;
    }

    if (lastEventType_=="flow.fail"){
        if (st.max_retry>0 && retryCount_<st.max_retry){ retryCount_++; gotoStep(st.name); return; }
        if (!st.nextOnFail.empty()){ gotoStep(st.nextOnFail); return; }
        if (!st.nextDefault.empty()){ gotoStep(st.nextDefault); return; }
        return;
    }

    bool anyOk = true;
    if (!st.wait.anyOfTypes.empty()){
        anyOk = false;
        for (size_t i=0;i<st.wait.anyOfTypes.size();++i){
            if (lastEventType_ == st.wait.anyOfTypes[i]){ anyOk=true; break; }
        }
    }
    bool allOk = true;
    if (!st.wait.allOfTypes.empty()){
        for (size_t i=0;i<st.wait.allOfTypes.size();++i){
            if (fulfilled_.find(st.wait.allOfTypes[i])==fulfilled_.end()){ allOk=false; break; }
        }
    }
    if (anyOk && allOk){
        std::map<std::string, std::string>::iterator nx = st.nextByEvent.find(lastEventType_);
        if (nx!=st.nextByEvent.end()){ gotoStep(nx->second); return; }
        if (!st.nextDefault.empty()){ gotoStep(st.nextDefault); return; }
    }
}

void FlowTask::onMessage(Message& m){
    if (m.header.type=="flow.load"){ loadFromJson(m.payload); return; }
    if (m.header.type=="flow.start"){ gotoStep(m.payload.value("name").toString().toStdString()); return; }
    if (m.header.type=="flow.goto"){  gotoStep(m.payload.value("name").toString().toStdString()); return; }
    if (step_.empty()) return;
    handleEvent(m);
    evaluate();
}

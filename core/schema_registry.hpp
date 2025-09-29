#pragma once
#include "core/task_base.hpp"
#include <map>

class SchemaRegistry : public TaskBase {
public:
    SchemaRegistry(MessageBus* bus): TaskBase("schema", bus) {}
protected:
    void onStart(){ status_=TS_RUNNING; }
    void onMessage(Message& m){
        const std::string& t = m.header.type;
        if (t=="schema.add" || t=="schema.update") addOrUpdate(m.payload);
        else if (t=="schema.remove") remove(m.payload);
        else if (t=="schema.validate") validate(m.header.from, m.header.correlation_id, m.payload);
    }
private:
    std::map<std::string, QJsonObject> schemas_;

    static std::string keyOf(const QJsonObject& o){
        QString typ = o.value("type").toString();
        QString ver = o.value("version").toString("1.0");
        return (typ + "@" + ver).toStdString();
    }
    void addOrUpdate(const QJsonObject& o){
        std::string k = keyOf(o);
        QJsonObject sc = o.value("schema").toObject();
        schemas_[k] = sc;
    }
    void remove(const QJsonObject& o){
        std::string k = keyOf(o);
        std::map<std::string, QJsonObject>::iterator it=schemas_.find(k);
        if (it!=schemas_.end()) schemas_.erase(it);
    }
    void validate(const std::string& replyTo, const std::string& cid, const QJsonObject& o){
        QString typ = o.value("type").toString();
        QString ver = o.value("version").toString("1.0");
        std::string k = (typ + "@" + ver).toStdString();
        QJsonObject payload = o.value("payload").toObject();

        QJsonObject result; result.insert("ok", true);
        std::map<std::string, QJsonObject>::iterator it=schemas_.find(k);
        if (it!=schemas_.end()){
            QJsonArray req = it->second.value("required").toArray();
            QJsonArray miss;
            for (int i=0;i<req.size();++i){
                QString r = req[i].toString();
                if (!payload.contains(r)) miss.push_back(r);
            }
            if (!miss.isEmpty()){
                result.insert("ok", false);
                result.insert("missing", miss);
            }
        } else {
            result.insert("ok", false);
            result.insert("error", "schema_not_found");
        }
        Message r; r.header.type="schema.validate.reply"; r.header.to=replyTo; r.header.from=id(); r.header.correlation_id=cid;
        r.payload = result;
        sendToBus(std::move(r));
    }
};

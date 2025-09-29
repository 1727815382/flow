#include "modules/datahub/datahub_task.hpp"

void DataHubTask::onMessage(Message& m){
    const QString t = QString::fromUtf8(m.header.type.c_str());
    const QJsonObject& p = m.payload;

    if (t=="datahub.set"){
        QWriteLocker lk(&rw_);
        store_.insert(p.value("key").toString(), p.value("value"));
        Message r; r.header.type="datahub.value";
        r.payload = QJsonObject{{"key", p.value("key").toString()},
                                {"value", p.value("value")}};
        bus_->publish("datahub.value", r);
    } else if (t=="datahub.bulk"){
        QWriteLocker lk(&rw_);
        QJsonArray arr = p.value("items").toArray();
        for (int i=0;i<arr.size();++i){
            QJsonObject it = arr[i].toObject();
            store_.insert(it.value("key").toString(), it.value("value"));
            Message r; r.header.type="datahub.value";
            r.payload = QJsonObject{{"key", it.value("key").toString()},
                                    {"value", it.value("value")}};
            bus_->publish("datahub.value", r);
        }
    } else if (t=="datahub.get"){
        QString k = p.value("key").toString();
        QJsonValue v;
        { QReadLocker lk(&rw_); v = store_.value(k); }
        Message r; r.header.type="datahub.value";
        r.payload = QJsonObject{{"key", k},{"value", v}};
        bus_->publish("datahub.value", r);
    } else if (t=="datahub.get_many"){
        QJsonArray keys = p.value("keys").toArray();
        QJsonObject kv;
        { QReadLocker lk(&rw_);
          for (int i=0;i<keys.size();++i){
              QString k = keys[i].toString();
              kv.insert(k, store_.value(k));
          }
        }
        Message r; r.header.type="datahub.snapshot.reply";
        r.payload = QJsonObject{{"values", kv}};
        bus_->publish("datahub.snapshot.reply", r);
    } else if (t=="datahub.cas"){ // compare and set
        QString k = p.value("key").toString();
        QJsonValue oldv = p.value("expect");
        QJsonValue newv = p.value("value");
        bool ok=false;
        {
            QWriteLocker lk(&rw_);
            QJsonValue cur = store_.value(k);
            if (cur==oldv){ store_.insert(k, newv); ok=true; }
        }
        Message r; r.header.type="datahub.cas.reply";
        r.payload = QJsonObject{{"key",k},{"ok",ok}};
        bus_->publish("datahub.cas.reply", r);
    } else if (t=="datahub.snapshot"){
        QJsonObject kv;
        { QReadLocker lk(&rw_);
          for (auto it=store_.begin(); it!=store_.end(); ++it) kv.insert(it.key(), it.value());
        }
        Message r; r.header.type="datahub.snapshot.reply";
        r.payload = QJsonObject{{"values", kv}};
        bus_->publish("datahub.snapshot.reply", r);
    }
}

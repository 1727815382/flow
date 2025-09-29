#pragma once
#include "core/taskbase.hpp"
#include "core/message_bus.hpp"
#include <QVector>
#include <QJsonObject>
#include <QJsonArray>
#include <QFile>
#include <QTextStream>

class MeasureTask : public TaskBase {
    Q_OBJECT
public:
    explicit MeasureTask(QObject* p=nullptr):TaskBase(p){
        MessageBus::instance().subscribe([this](Message& m){ onMessage(m); });
    }
    void init() override { publish("measure.ready", {}); }

private:
    QVector<QJsonObject> _rows;
    double _adjLimit=20.0;

    void onMessage(Message& m){
        const auto& t=m.header.type; const auto p=m.payload;
        if(t=="measure.add")      add(p);
        else if(t=="measure.mod") mod(p);
        else if(t=="measure.del") del(p);
        else if(t=="measure.clear"){ _rows.clear(); dataset(); }
        else if(t=="measure.check") check();
        else if(t=="measure.file.save") save(p.value("path").toString());
        else if(t=="measure.file.load") load(p.value("path").toString());
        else if(t=="measure.request") dataset();
    }
    void add(QJsonObject r){ computeAdjust(r); _rows.push_back(r); dataset(); }
    void mod(QJsonObject p){ int i=p.value("row").toInt(-1); if(i<0||i>=_rows.size())return; computeAdjust(p); _rows[i]=p; dataset(); }
    void del(QJsonObject p){ int i=p.value("row").toInt(-1); if(i<0||i>=_rows.size())return; _rows.remove(i); dataset(); }

    void computeAdjust(QJsonObject& row){
        const double gauge=row.value("gauge").toDouble();
        const double adj = qBound(-_adjLimit, -gauge, _adjLimit);
        row["adj"]=QString::number(adj,'f',2);
    }
    void dataset(){
        QJsonArray a; for(auto& r:_rows) a.push_back(r);
        publish("measure.dataset", QJsonObject{{"rows",a}});
    }
    void check(){
        int pass=0, fail=0; QJsonArray issues;
        for(int i=0;i<_rows.size();++i){
            const double g=_rows[i].value("gauge").toDouble();
            if(fabs(g)>6.0){ ++fail; issues.push_back(QJsonObject{{"row",i},{"reason","轨距偏差超限"}}); }
            else ++pass;
        }
        publish("measure.check.result", QJsonObject{{"pass",pass},{"fail",fail},{"issues",issues}});
    }
    void save(const QString& path){
        QFile f(path); if(!f.open(QIODevice::WriteOnly|QIODevice::Text)){ publish("measure.error", QJsonObject{{"where","save"},{"path",path}}); return; }
        QTextStream out(&f); out.setCodec("UTF-8");
        out<<"ID,里程,轨距偏差,调整\n";
        for(auto& r:_rows) out<<r.value("id").toString()<<","<<r.value("mile").toString()<<","<<r.value("gauge").toString()<<","<<r.value("adj").toString()<<"\n";
        publish("measure.file.saved", QJsonObject{{"ok",true},{"path",path}});
    }
    void load(const QString& path){
        QFile f(path); if(!f.open(QIODevice::ReadOnly|QIODevice::Text)){ publish("measure.error", QJsonObject{{"where","load"},{"path",path}}); return; }
        _rows.clear();
        QTextStream in(&f); in.setCodec("UTF-8");
        if(!in.atEnd()) in.readLine(); // skip header
        while(!in.atEnd()){
            const auto line=in.readLine(); const auto c=line.split(",");
            if(c.size()<4) continue;
            _rows.push_back(QJsonObject{{"id",c[0]},{"mile",c[1]},{"gauge",c[2]},{"adj",c[3]}});
        }
        dataset();
        publish("measure.file.loaded", QJsonObject{{"ok",true},{"path",path}});
    }
    static void publish(const std::string& t, const QJsonObject& p){
        Message mm; mm.header.type=t; mm.payload=p; MessageBus::instance().post(mm);
    }
};

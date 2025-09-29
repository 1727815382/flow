#include "modules/log_service/log_service.hpp"

void LogService::onMessage(Message& m){
    const std::string& t = m.header.type;
    if (t=="log.config") {
        applyConfig(m.payload);
        return;
    }
    if (t=="log.write") {
        QString level = m.payload.value("level").toString("info");
        QString tag   = m.payload.value("tag").toString("app");
        QString msg   = m.payload.value("msg").toString();
        writeLine(level, tag, msg);

        // 镜像到主题（UI/回放可用）
        Message ev; ev.header.type="log.event"; ev.header.topic="log.event"; ev.header.from=id();
        ev.payload = QJsonObject{{"level",level},{"tag",tag},{"msg",msg}};
        sendToBus(std::move(ev));
        return;
    }
}

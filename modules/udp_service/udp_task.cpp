#include "modules/udp_service/udp_task.hpp"
#include "core/message_bus.hpp"
#include <QDateTime>
#include <QFile>
#include <QJsonArray>
#include <algorithm>

static long long now_ms_qt(){ return (long long)QDateTime::currentMSecsSinceEpoch(); }
long long UdpTask::nowMs(){ return now_ms_qt(); }

UdpTask::UdpTask(MessageBus* bus, uint16_t localPort)
    : TaskBase("udp", bus),
      localPort_(localPort),
      rxBufSize_(2048),
      stopRx_(false)
{
    defaultCfg_.ack_timeout_ms = 500;
    defaultCfg_.max_retries = 5;
    defaultCfg_.backoff_ms_init = 200;
    defaultCfg_.backoff_ms_max = 2000;

    // 12bit epoch，简单用启动秒取低 12bit
    uint32_t sec = (uint32_t)(QDateTime::currentSecsSinceEpoch() & 0xFFF);
    epoch_ = (sec & 0xFFF) << 20;
}

void UdpTask::onStart(){
    if (!sock_.open(localPort_)){
        reportException("udp.open","bind fail");
    }
    stopRx_.store(false);
    rxThread_ = std::thread(&UdpTask::rxLoop, this);
    txThread_ = std::thread(&UdpTask::txLoop, this);
    status_ = TS_RUNNING;
    reportStatus("started", "");
}

void UdpTask::onStop(){
    stopRx_.store(true);
    rx2tx_.close();
    if (rxThread_.joinable()) rxThread_.join();
    if (txThread_.joinable()) txThread_.join();
    sock_.close();
}

std::string UdpTask::keyOf(const std::string& ip, uint16_t port){
    char b[64]; std::sprintf(b, "%s:%u", ip.c_str(), (unsigned)port);
    return std::string(b);
}
std::string UdpTask::pkey(uint32_t msg_id, const std::string& ip, uint16_t port){
    char b[80]; std::sprintf(b, "%s#%u", keyOf(ip,port).c_str(), msg_id);
    return std::string(b);
}

QString UdpTask::toHex(const unsigned char* p, int n){
    static const char* hex="0123456789ABCDEF";
    QString s; s.reserve(n*2);
    for (int i=0;i<n;++i){ s.append(hex[p[i]>>4]); s.append(hex[p[i]&0xF]); }
    return s;
}

std::vector<unsigned char> UdpTask::parseHex(const QString& h){
    QString s; s.reserve(h.size());
    for (int i=0;i<h.size();++i){
        const QChar c = h.at(i);
        const ushort u = c.unicode();
        const bool isHex = (u>='0'&&u<='9')||(u>='a'&&u<='f')||(u>='A'&&u<='F');
        if (isHex) s.append(c);
    }
    if (s.size() & 1) s.prepend('0');
    std::vector<unsigned char> out; out.reserve(s.size()/2);
    for (int i=0;i<s.size(); i+=2){
        bool ok=false; unsigned int v = s.mid(i,2).toUInt(&ok,16);
        out.push_back(ok ? static_cast<unsigned char>(v) : 0);
    }
    return out;
}

std::vector<unsigned char> UdpTask::buildUR(uint32_t msgid, bool isAck, const unsigned char* payload, int n){
    std::vector<unsigned char> out;
    out.reserve(8+n);
    out.push_back('U'); out.push_back('R'); out.push_back(0x01);
    unsigned char flags = (unsigned char)(isAck?0x01:0x02);
    out.push_back(flags);
    out.push_back((msgid>>24)&0xFF); out.push_back((msgid>>16)&0xFF);
    out.push_back((msgid>>8)&0xFF); out.push_back((msgid)&0xFF);
    if (!isAck && payload && n>0) out.insert(out.end(), payload, payload+n);
    return out;
}

bool UdpTask::parseUR(const unsigned char* buf, int n, bool& isUR, bool& isAck, uint32_t& msgid,
                      const unsigned char*& payload, int& payloadLen){
    isUR=false; isAck=false; msgid=0; payload=NULL; payloadLen=0;
    if (n<8) return false;
    if (buf[0]!='U' || buf[1]!='R' || buf[2]!=0x01) return false;
    isUR=true;
    unsigned char flags=buf[3];
    isAck = (flags & 0x01) != 0;
    bool isRel = (flags & 0x02) != 0;
    msgid = ( (uint32_t)buf[4]<<24 ) | ( (uint32_t)buf[5]<<16 ) | ( (uint32_t)buf[6]<<8 ) | (uint32_t)buf[7];
    if (isAck){ payload=NULL; payloadLen=0; }
    else if (isRel){ payload = buf+8; payloadLen = n-8; }
    else { isUR=false; payload=buf; payloadLen=n; }
    return true;
}

void UdpTask::publishRecv(const std::string& ip, uint16_t port, const unsigned char* data, int n,
                          bool reliable, uint32_t msgid){
    // 截断 HEX 预览，避免 UI/日志爆量
    int show = std::min(n, debug_hex_max_);
    Message out; out.header.from=id(); out.header.type="udp.recv"; out.header.topic="udp.recv";
    out.payload = QJsonObject{
        {"ip", QString::fromStdString(ip)},
        {"port", (int)port},
        {"data_hex", toHex(data, show)},
        {"len", n},
        {"truncated", show<n},
        {"reliable", reliable},
        {"msg_id", (int)msgid}
    };
    sendToBus(std::move(out));
}

void UdpTask::sendRaw(const std::string& ip, uint16_t port, const unsigned char* data, int n){
    int ret = sock_.sendTo(ip, port, data, n);
    if (ret<0){
        reportException("udp.send","sendto fail");
        maybeReopenSocket(false, "send_error");
    }
}

void UdpTask::sendAck(const std::string& ip, uint16_t port, uint32_t msgid){
    std::vector<unsigned char> ack = buildUR(msgid, true, NULL, 0);
    sendRaw(ip, port, &ack[0], (int)ack.size());
    Message a; a.header.type="udp.ack.sent"; a.header.from=id(); a.header.topic="udp.ack.sent";
    a.payload = QJsonObject{{"ip",QString::fromStdString(ip)},{"port",(int)port},{"msg_id",(int)msgid}};
    sendToBus(std::move(a));
}

void UdpTask::scheduleReliableSend(const std::string& ip, uint16_t port,
                                   const unsigned char* payload, int n,
                                   const RetryCfg& cfg, uint32_t msgid){
    if (msgid==0) msgid = nextMsgId_compose();
    std::vector<unsigned char> frame = buildUR(msgid, false, payload, n);
    std::string key = pkey(msgid, ip, port);
    Pending pd;
    pd.key=key; pd.ip=ip; pd.port=port; pd.msg_id=msgid; pd.frame=frame;
    pd.tries=0; pd.cfg=cfg;
    pd.next_ms = nowMs();
    pd.first_send_ms = pd.next_ms;
    pendings_[key] = pd;
}

void UdpTask::updateRtt(const std::string& ip, uint16_t port, long long sent_ms, long long now_ms){
    std::string k = keyOf(ip, port);
    auto it = peers_.find(k);
    if (it==peers_.end()) return;
    double sample = double(now_ms - sent_ms);
    if (it->second.srtt_ms<=0){
        it->second.srtt_ms = sample;
        it->second.rttvar_ms = sample/2.0;
    }else{
        double alpha=1.0/8.0, beta=1.0/4.0;
        it->second.rttvar_ms = (1.0 - beta) * it->second.rttvar_ms + beta * std::abs(it->second.srtt_ms - sample);
        it->second.srtt_ms   = (1.0 - alpha) * it->second.srtt_ms + alpha * sample;
    }
}

void UdpTask::processAck(const std::string& ip, uint16_t port, uint32_t msgid){
    std::string key = pkey(msgid, ip, port);
    auto it=pendings_.find(key);
    if (it!=pendings_.end()){
        // RTT 统计
        updateRtt(ip, port, it->second.first_send_ms, nowMs());
        pendings_.erase(it);
        Message ok; ok.header.type="udp.ack"; ok.header.from=id(); ok.header.topic="udp.ack";
        ok.payload = QJsonObject{{"ip",QString::fromStdString(ip)},{"port",(int)port},{"msg_id",(int)msgid},{"ok",true}};
        sendToBus(std::move(ok));
    }
}

void UdpTask::maybeReopenSocket(bool force, const char* reason){
    if (!force && sock_.is_open()) return;
    bool ok = sock_.reopen();
    Message s; s.header.from=id(); s.header.topic="udp.socket";
    if (ok){
        s.header.type="udp.socket.reopen.ok";
        s.payload = QJsonObject{{"reason", reason ? reason : "auto"}};
    }else{
        s.header.type="udp.socket.reopen.fail";
        s.payload = QJsonObject{{"reason", reason ? reason : "auto"}};
    }
    sendToBus(std::move(s));
}

void UdpTask::updatePeerSeen_actor(const std::string& ip, uint16_t port){
    std::string k = keyOf(ip, port);
    auto it=peers_.find(k);
    if (it==peers_.end()) return;
    it->second.last_seen_ms = nowMs();
    if (!it->second.up){
        it->second.up=true;
        Message up; up.header.type="udp.peer.up"; up.header.from=id(); up.header.topic="udp.peer.state";
        up.payload = QJsonObject{{"ip",QString::fromStdString(ip)},{"port",(int)port}};
        sendToBus(std::move(up));
    }
}

void UdpTask::heartbeatSweep(){
    long long now = nowMs();
    for (auto it=peers_.begin(); it!=peers_.end(); ++it){
        Peer& p = it->second;
        if (p.heartbeat_ms>0){
            if (p.last_seen_ms>0 && now - p.last_seen_ms > p.heartbeat_ms*3){
                if (p.up){
                    p.up=false;
                    Message down; down.header.type="udp.peer.down"; down.header.from=id(); down.header.topic="udp.peer.state";
                    down.payload = QJsonObject{{"ip",QString::fromStdString(p.ip)},{"port",(int)p.port}};
                    sendToBus(std::move(down));
                }
            }
            if ((now / p.heartbeat_ms) != ((now - 100) / p.heartbeat_ms)){
                static const char* hb = "HB";
                sendRaw(p.ip, p.port, (const unsigned char*)hb, 2);
            }
        }
    }
}

void UdpTask::rxLoop(){
    std::vector<unsigned char> buf(rxBufSize_>0?rxBufSize_:2048);
    while(!stopRx_.load()){
        std::string ip; uint16_t port=0;
        int n = sock_.recvFrom(&buf[0], (int)buf.size(), ip, port);
        if (n>0){
            RxEvent ev; ev.ip=ip; ev.port=port; ev.bytes.assign(&buf[0], &buf[0]+n);
            const unsigned char* pay=nullptr; int payLen=0; bool isUR=false, isAck=false; uint32_t msgid=0;
            if (parseUR(&buf[0], n, isUR, isAck, msgid, pay, payLen)){
                ev.isUR=isUR; ev.isAck=isAck; ev.msgid=msgid;
                if (isUR && !isAck){ ev.payload = pay; ev.payloadLen = payLen; }
            }
            rx2tx_.push(std::move(ev));
        } else {
            if (n<0) maybeReopenSocket(false, "recv_error");
            std::this_thread::sleep_for(std::chrono::milliseconds(2));
        }
    }
}

void UdpTask::actorProcessOne(const RxEvent& ev){
    if (ev.isUR && ev.isAck){
        processAck(ev.ip, ev.port, ev.msgid);
        updatePeerSeen_actor(ev.ip, ev.port);
        return;
    }
    if (ev.isUR){
        // 去重窗口：按 (epoch,seq) 判重
        std::string k = keyOf(ev.ip, ev.port);
        auto it = peers_.find(k);
        if (it!=peers_.end()){
            uint32_t seq = (ev.msgid & 0xFFFFF);
            uint32_t epoch = (ev.msgid >> 20) & 0xFFF;
            // 简化策略：同 epoch 下，如果 seq 已出现则丢弃
            auto& rec = it->second.recent_seq;
            if (rec.find(ev.msgid)!=rec.end()){
                // 丢弃重复
            }else{
                if (rec.size() > 2048) rec.clear();
                rec.insert(ev.msgid);
                publishRecv(ev.ip, ev.port, (const unsigned char*)ev.payload, ev.payloadLen, true, ev.msgid);
                sendAck(ev.ip, ev.port, ev.msgid);
            }
        }else{
            publishRecv(ev.ip, ev.port, (const unsigned char*)ev.payload, ev.payloadLen, true, ev.msgid);
            sendAck(ev.ip, ev.port, ev.msgid);
        }
        updatePeerSeen_actor(ev.ip, ev.port);
        return;
    }
    publishRecv(ev.ip, ev.port, ev.bytes.data(), (int)ev.bytes.size(), false, 0);
    updatePeerSeen_actor(ev.ip, ev.port);
}

void UdpTask::txLoop(){
    long long lastBeat = nowMs();
    while(!stopRx_.load()){
        long long now = nowMs();
        // 重传扫描
        for (auto it=pendings_.begin(); it!=pendings_.end(); ){
            Pending& pd = it->second;
            if (pd.next_ms <= now){
                // 自适应初始超时
                std::string k = keyOf(pd.ip, pd.port);
                auto pit = peers_.find(k);
                int init_to = pd.cfg.ack_timeout_ms;
                if (pit!=peers_.end() && pit->second.srtt_ms>1.0){
                    int dyn = (int)std::min<double>(pd.cfg.backoff_ms_max,
                        pit->second.srtt_ms + 4.0*std::max(1.0, pit->second.rttvar_ms));
                    init_to = std::max(init_to, dyn);
                }

                sendRaw(pd.ip, pd.port, &pd.frame[0], (int)pd.frame.size());
                pd.tries++;
                if (pd.tries==1){
                    pd.next_ms = now + init_to;
                } else {
                    int backoff = pd.cfg.backoff_ms_init * (1<<std::min(pd.tries-1, 10));
                    if (backoff > pd.cfg.backoff_ms_max) backoff = pd.cfg.backoff_ms_max;
                    pd.next_ms = now + backoff;
                }
                Message ev; ev.header.from=id(); ev.header.topic="udp.resend";
                ev.header.type = (pd.tries==1 ? "udp.send.first" : "udp.send.retry");
                ev.payload = QJsonObject{
                    {"ip", QString::fromStdString(pd.ip)},
                    {"port", (int)pd.port},
                    {"msg_id", (int)pd.msg_id},
                    {"tries", pd.tries}
                };
                sendToBus(std::move(ev));

                if (pd.cfg.max_retries>0 && pd.tries>pd.cfg.max_retries){
                    Message drop; drop.header.type="udp.drop"; drop.header.from=id(); drop.header.topic="udp.drop";
                    drop.payload = QJsonObject{{"ip",QString::fromStdString(pd.ip)},{"port",(int)pd.port},
                                               {"msg_id",(int)pd.msg_id},{"reason","max_retries"}};
                    sendToBus(std::move(drop));
                    pendings_.erase(it++);
                    continue;
                }
            }
            ++it;
        }

        if (now - lastBeat >= 100){
            heartbeatSweep();
            lastBeat = now;
        }

        // 处理来自 RX 的事件
        UdpTask::RxEvent ev;
        if (rx2tx_.pop_blocking(ev)){
            actorProcessOne(ev);
            for (int i=0;i<8; ++i){
                UdpTask::RxEvent ev2;
                if (!rx2tx_.pop_blocking(ev2)) break;
                actorProcessOne(ev2);
            }
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
}

void UdpTask::cmdConfig(const QJsonObject& o){
    if (o.contains("local_port")){
        localPort_ = (uint16_t)o.value("local_port").toInt();
        maybeReopenSocket(true, "config_change");
    }
    if (o.contains("rx_buf"))  rxBufSize_ = o.value("rx_buf").toInt(2048);
    if (o.contains("debug_hex_max")) debug_hex_max_ = std::max(0, o.value("debug_hex_max").toInt(128));

    QJsonObject d = o.value("default").toObject();
    if (!d.isEmpty()){
        defaultCfg_.ack_timeout_ms = d.value("ack_timeout_ms").toInt(defaultCfg_.ack_timeout_ms);
        defaultCfg_.max_retries    = d.value("max_retries").toInt(defaultCfg_.max_retries);
        defaultCfg_.backoff_ms_init= d.value("backoff_ms_init").toInt(defaultCfg_.backoff_ms_init);
        defaultCfg_.backoff_ms_max = d.value("backoff_ms_max").toInt(defaultCfg_.backoff_ms_max);
    }
}

void UdpTask::cmdPeerAdd(const QJsonObject& o){
    std::string ip = o.value("ip").toString().toStdString();
    uint16_t port = (uint16_t)o.value("port").toInt();
    std::string k = keyOf(ip, port);
    Peer p; p.ip=ip; p.port=port;
    QJsonObject c = o.value("cfg").toObject();
    p.cfg.ack_timeout_ms = c.value("ack_timeout_ms").toInt(defaultCfg_.ack_timeout_ms);
    p.cfg.max_retries    = c.value("max_retries").toInt(defaultCfg_.max_retries);
    p.cfg.backoff_ms_init= c.value("backoff_ms_init").toInt(defaultCfg_.backoff_ms_init);
    p.cfg.backoff_ms_max = c.value("backoff_ms_max").toInt(defaultCfg_.backoff_ms_max);
    p.heartbeat_ms = o.value("heartbeat_ms").toInt(0);
    p.last_seen_ms = 0; p.up=false;
    peers_[k] = p;
    Message ok; ok.header.type="udp.peer.add.ok"; ok.header.from=id(); ok.header.topic="udp.peer.ctrl";
    ok.payload = QJsonObject{{"ip",QString::fromStdString(ip)},{"port",(int)port}};
    sendToBus(std::move(ok));
}

void UdpTask::cmdPeerRemove(const QJsonObject& o){
    std::string ip = o.value("ip").toString().toStdString();
    uint16_t port = (uint16_t)o.value("port").toInt();
    std::string k = keyOf(ip, port);
    peers_.erase(k);
    Message ok; ok.header.type="udp.peer.remove.ok"; ok.header.from=id(); ok.header.topic="udp.peer.ctrl";
    ok.payload = QJsonObject{{"ip",QString::fromStdString(ip)},{"port",(int)port}};
    sendToBus(std::move(ok));
}

uint32_t UdpTask::nextMsgId_compose(){
    uint32_t seqv = seq_.fetch_add(1) & 0xFFFFF; // 20 bit
    return epoch_ | seqv;
}

void UdpTask::cmdSend(const QJsonObject& o){
    std::string ip = o.value("ip").toString().toStdString();
    uint16_t port = (uint16_t)o.value("port").toInt();

    if (ip.empty() && o.contains("peer")){
        std::string pk = o.value("peer").toString().toStdString();
        auto it=peers_.find(pk);
        if (it!=peers_.end()){ ip=it->second.ip; port=it->second.port; }
    }
    if (ip.empty() || port==0) { reportException("udp.send","no target"); return; }

    std::vector<unsigned char> payload;
    if (o.contains("data_hex") && o.value("data_hex").isString()){
        payload = parseHex(o.value("data_hex").toString());
    } else if (o.contains("data_base64") && o.value("data_base64").isString()){
        QByteArray ba = QByteArray::fromBase64(o.value("data_base64").toString().toLatin1());
        payload.assign(ba.begin(), ba.end());
    } else if (o.contains("data_bytes") && o.value("data_bytes").isArray()){
        QJsonArray arr = o.value("data_bytes").toArray();
        payload.reserve(arr.size());
        for (int i=0;i<arr.size();++i){
            int v = arr[i].toInt(0);
            if (v<0) v=0; if (v>255) v=255;
            payload.push_back((unsigned char)v);
        }
    } else if (o.contains("data_utf8") && o.value("data_utf8").isString()){
        QByteArray ba = o.value("data_utf8").toString().toUtf8();
        payload.assign(ba.begin(), ba.end());
    } else if (o.contains("file") && o.value("file").isString()){
        QFile f(o.value("file").toString());
        if (f.open(QIODevice::ReadOnly)){
            QByteArray ba = f.readAll();
            payload.assign(ba.begin(), ba.end());
        } else {
            reportException("udp.send","open file fail");
        }
    }
    if (payload.empty() && !o.value("allow_empty").toBool(false)) return;

    bool reliable = o.value("reliable").toBool(false);
    RetryCfg cfg = defaultCfg_;
    QJsonObject c = o.value("cfg").toObject();
    if (!c.isEmpty()){
        cfg.ack_timeout_ms  = c.value("ack_timeout_ms").toInt(cfg.ack_timeout_ms);
        cfg.max_retries     = c.value("max_retries").toInt(cfg.max_retries);
        cfg.backoff_ms_init = c.value("backoff_ms_init").toInt(cfg.backoff_ms_init);
        cfg.backoff_ms_max  = c.value("backoff_ms_max").toInt(cfg.backoff_ms_max);
    }
    std::string k = keyOf(ip, port);
    auto pit=peers_.find(k);
    if (pit!=peers_.end()) cfg = pit->second.cfg;

    if (reliable){
        scheduleReliableSend(ip, port, payload.empty()?NULL:&payload[0], (int)payload.size(), cfg,
                             (uint32_t)o.value("msg_id").toInt(0));
    } else {
        sendRaw(ip, port, payload.empty()?NULL:&payload[0], (int)payload.size());
    }
}

void UdpTask::onMessage(Message& m){
    const std::string& t = m.header.type;
    if (t=="udp.config")      cmdConfig(m.payload);
    else if (t=="udp.peer.add")    cmdPeerAdd(m.payload);
    else if (t=="udp.peer.remove") cmdPeerRemove(m.payload);
    else if (t=="udp.send")        cmdSend(m.payload);
}

// modules/proto_engine/protocol_engine_task.cpp
#include "modules/proto_engine/protocol_engine_task.hpp"
#include "modules/proto_engine/bt.hpp"

#include <QFile>
#include <QJsonDocument>
#include <QDateTime>
#include <QtGlobal>

#include <algorithm>
#include <climits>
#include <cmath>
#include <limits>
#include <cstring>
#include <atomic>
#include <mutex>
#include <condition_variable>
#include <queue>

// ==========================
// 文件内工具 & 全局开关
// ==========================
namespace {

// 以稳态时钟实现毫秒时间，避免系统时间跳变的影响
inline int64_t steady_ms() {
    using namespace std::chrono;
    static const steady_clock::time_point s0 = steady_clock::now();
    return duration_cast<milliseconds>(steady_clock::now() - s0).count();
}

static std::atomic<int> g_debug_hex_limit(256); // 可被配置覆盖

// ---------- HEX 工具 ----------
inline QByteArray stripHex(const QString& s){
    QString h; h.reserve(s.size());
    for (int i=0;i<s.size();++i){ QChar c=s[i]; if (!c.isSpace()) h.append(c); }
    QByteArray out; out.reserve(h.size()/2);
    for (int i=0;i+1<h.size(); i+=2){
        bool ok=false; uchar v=(uchar)h.mid(i,2).toUInt(&ok,16);
        out.append((char)(ok ? v : 0));
    }
    return out;
}
inline QString bytesToHex(const QByteArray& ba, bool withSpaces){
    static const char* H="0123456789ABCDEF";
    QString s; s.reserve(ba.size()*3);
    for (int i=0;i<ba.size();++i){
        uchar v=(uchar)ba[i];
        s.append(H[(v>>4)&0xF]); s.append(H[v&0xF]);
        if (withSpaces && i+1<ba.size()) s.append(' ');
    }
    return s;
}
inline QByteArray hexToBytes(const QString& hex){ return stripHex(hex); }

// ---------- CRC/SUM/XOR 轻量实现（如 FieldCodec 已有可替换调用） ----------
inline quint16 crc16_modbus_impl(const QByteArray& d, int from, int to){
    quint16 crc = 0xFFFF;
    int end = (to<0 || to>=(d.size()-1)) ? (d.size()-1) : to;
    for (int i=std::max(0,from); i<=end; ++i){
        crc ^= (uchar)d[i];
        for (int j=0;j<8;++j){
            if (crc & 1) crc = (crc>>1) ^ 0xA001; else crc >>= 1;
        }
    }
    return crc;
}
inline quint16 crc16_ccitt_impl(const QByteArray& d, int from, int to){
    quint16 crc = 0xFFFF;
    int end = (to<0 || to>=(d.size()-1)) ? (d.size()-1) : to;
    for (int i=std::max(0,from); i<=end; ++i){
        crc ^= (quint16)((uchar)d[i] << 8);
        for (int j=0;j<8;++j){
            if (crc & 0x8000) crc = (crc << 1) ^ 0x1021; else crc <<= 1;
        }
    }
    return crc;
}
inline quint32 crc32_ieee_impl(const QByteArray& d, int from, int to){
    quint32 crc = 0xFFFFFFFFu;
    static quint32 table[256]; static bool inited=false;
    if (!inited){
        for (quint32 i=0;i<256;++i){
            quint32 c=i;
            for (int j=0;j<8;++j) c = (c&1)? (0xEDB88320u ^ (c>>1)) : (c>>1);
            table[i]=c;
        }
        inited=true;
    }
    int end = (to<0 || to>=(d.size()-1)) ? (d.size()-1) : to;
    for (int i=std::max(0,from); i<=end; ++i){
        crc = table[(crc ^ (uchar)d[i]) & 0xFF] ^ (crc >> 8);
    }
    return crc ^ 0xFFFFFFFFu;
}
inline uchar sum8_impl(const QByteArray& d, int from, int to){
    int end = (to<0 || to>=(d.size()-1)) ? (d.size()-1) : to;
    unsigned s = 0;
    for (int i=std::max(0,from); i<=end; ++i) s += (uchar)d[i];
    return (uchar)(s & 0xFF);
}
inline uchar xor8_impl(const QByteArray& d, int from, int to){
    int end = (to<0 || to>=(d.size()-1)) ? (d.size()-1) : to;
    uchar x = 0;
    for (int i=std::max(0,from); i<=end; ++i) x ^= (uchar)d[i];
    return x;
}

// ---------- BCD ----------
inline QString bcd_to_string(const QByteArray& d){
    QString out; out.reserve(d.size()*2);
    for (int i=0;i<d.size(); ++i){
        uchar b = (uchar)d[i];
        out.append(QChar('0' + ((b>>4)&0xF)));
        out.append(QChar('0' + (b&0xF)));
    }
    return out;
}

} // anonymous

// ==========================
// ProtocolEngineTask 实现
// ==========================

// ====== 析构 ======
ProtocolEngineTask::~ProtocolEngineTask(){}

// ====== 构造 ======
ProtocolEngineTask::ProtocolEngineTask(MessageBus* bus)
    : TaskBase("proto", bus),
      eval_(new ExprEval(this)),
      codec_(new FieldCodec())
{
}

// ====== IDataProvider ======
QVariant ProtocolEngineTask::get(const QString& key){
    auto it = datahub_.find(key);
    return it==datahub_.end()? QVariant() : it->second;
}

// ====== 生命周期 ======
void ProtocolEngineTask::onStart(){
    status_ = TS_RUNNING;
    bus_->subscribe(id(), "proto.config.load");
    bus_->subscribe(id(), "udp.recv");
    bus_->subscribe(id(), "datahub.value");

    // 启动引擎自己的定时线程（稳态时间）
    tstop_.store(false);
    tworker_ = std::thread(&ProtocolEngineTask::timerThread, this);
}
void ProtocolEngineTask::onStop(){
    { std::lock_guard<std::mutex> lk(tmu_); tstop_.store(true); }
    tcv_.notify_all();
    if (tworker_.joinable()) tworker_.join();
    status_ = TS_STOPPED;
}

// ====== HEX 工具（成员版转发至文件内静态实现）======
QByteArray ProtocolEngineTask::stripHex(const QString& s){ return ::stripHex(s); }
QString    ProtocolEngineTask::bytesToHex(const QByteArray& ba, bool withSpaces){ return ::bytesToHex(ba, withSpaces); }
QByteArray ProtocolEngineTask::hexToBytes(const QString& hex){ return ::hexToBytes(hex); }

// ====== 定时器线程（堆：按 due 升序触发；使用 steady_clock）======
void ProtocolEngineTask::timerThread(){
    std::unique_lock<std::mutex> lk(tmu_);
    while(!tstop_.load()){
        if (tqueue_.empty()){
            tcv_.wait(lk, [this]{ return tstop_.load() || !tqueue_.empty(); });
            if (tstop_.load()) break;
        } else {
            int64_t now = steady_ms();
            TimerItem ti = tqueue_.top();
            if (ti.due <= now){
                tqueue_.pop();
                // 执行回调放到锁外
                lk.unlock();
                // 安全：所有动作走 execAction（仍在 proto 任务线程上下文），这里只是调用你注册的动作
                execAction(ti.action.value("name").toString(), ti.action.value("args").toObject(), ti.ctx);
                lk.lock();
                if (tstop_.load()) break;
                if (ti.repeat<0 || ti.repeat>1){
                    if (ti.repeat>1) ti.repeat--;
                    ti.due = steady_ms() + std::max(0, ti.period_ms);
                    tqueue_.push(ti);
                }
                continue;
            }
            int64_t waitms = std::max<int64_t>(0, ti.due - now);
            tcv_.wait_for(lk, std::chrono::milliseconds(waitms));
        }
    }
}
void ProtocolEngineTask::scheduleTimer(int delay_ms, int repeat, const QJsonObject& action, const QJsonObject& ctx){
    TimerItem ti;
    ti.due = steady_ms() + std::max(0, delay_ms);
    ti.repeat = repeat;
    ti.period_ms = std::max(0, delay_ms);
    ti.action = action;
    ti.ctx = ctx;
    ti.id = timer_seq_++;
    {
        std::lock_guard<std::mutex> lk(tmu_);
        tqueue_.push(ti);
    }
    tcv_.notify_one();
}

// ====== 消息入口 ======
void ProtocolEngineTask::onMessage(Message& m){
    const QString tp = QString::fromUtf8(m.header.type.c_str());

    if (tp=="proto.config.load"){
        QString path = m.payload.value("file").toString("configs/proto_demo.json");
        bool ok = loadConfigFile(path);
        Message dbg; dbg.header.type="proto.debug";
        QJsonObject meta; meta.insert("what", ok?"config.loaded":"config.load.fail");
        meta.insert("path", path);
        meta.insert("bindings", (int)cfg_.bindings.size());
        meta.insert("messages", (int)cfg_.messages.size());
        meta.insert("rules",    (int)cfg_.rules.size());
        dbg.payload = meta; bus_->publish("proto.debug", dbg);
        return;
    }

    if (tp=="datahub.value"){
        QString k = m.payload.value("key").toString();
        QVariant v = m.payload.value("value").toVariant();
        datahub_[k] = v;
        return;
    }

    if (tp=="udp.recv"){
        QString ip = m.payload.value("ip").toString();
        int port   = m.payload.value("port").toInt();
        QByteArray bytes = hexToBytes(m.payload.value("data_hex").toString());

        for (size_t i=0;i<cfg_.messages.size();++i){
            const MessageDef& md = cfg_.messages[i];
            if (!matchMessage(md, bytes)) continue;

            QJsonObject fields = decodeFields(md, bytes);

            QJsonObject ctx;
            ctx.insert("message", md.id);
            ctx.insert("binding", md.binding);
            ctx.insert("source_ip", ip);
            ctx.insert("source_port", port);
            ctx.insert("msg", fields);

            if (cfg_.debug){
                Message dbg; dbg.header.type="proto.debug";
                QJsonObject det; det.insert("message", md.id);
                det.insert("binding", md.binding);
                det.insert("fields", fields);
                QJsonObject meta; meta.insert("what","recv.decoded");
                meta.insert("detail", det);
                dbg.payload = meta; bus_->publish("proto.debug", dbg);
            }

            // 规则驱动
            for (size_t r=0;r<cfg_.rules.size();++r){
                const Rule& rl = cfg_.rules[r];
                if (rl.when!="on_receive") continue;
                if (!rl.message.isEmpty() && rl.message!=md.id) continue;
                if (!rl.binding.isEmpty() && rl.binding!=md.binding) continue;

                if (!rl.bt.isEmpty()){
                    if (cfg_.debug){
                        Message dbg; dbg.header.type="proto.debug";
                        QJsonObject meta; meta.insert("what","bt.start"); meta.insert("rule", rl.id);
                        dbg.payload=meta; bus_->publish("proto.debug", dbg);
                    }

                    auto execThunk = [this](const QString& n, const QJsonObject& a, const QJsonObject& c)->bool{
                        return this->execAction(n, a, c);
                    };
                    std::unique_ptr<BTNode> root(BTBuilder::build(rl.bt, eval_.get(), execThunk));
                    if (root) {
                        root->exec(ctx, this, eval_.get());
                    }

                    if (cfg_.debug){
                        Message dbg; dbg.header.type="proto.debug";
                        QJsonObject meta; meta.insert("what","bt.done"); meta.insert("rule", rl.id);
                        dbg.payload=meta; bus_->publish("proto.debug", dbg);
                    }
                } else {
                    for (size_t a=0;a<rl.actions.size();++a){
                        execAction(rl.actions[a].type, rl.actions[a].args, ctx);
                    }
                }
            }
        }
        return;
    }
}

// ====== 动作分发 ======
bool ProtocolEngineTask::execAction(const QString& name, const QJsonObject& args, const QJsonObject& ctx){
    if (name=="publish"){ actPublish(args, ctx); return true; }
    if (name=="reply")  { actReply(args, ctx);   return true; }
    if (name=="timer")  { actTimer(args, ctx);   return true; }
    if (name=="state.set"){ actStateSet(args, ctx); return true; }
    if (name=="bt.exec"){
        // args: { "tree": <JSON>, "ctx": <ctx override?> }
        QJsonObject subTree = args.value("tree").toObject();
        QJsonObject merge = args.value("ctx").toObject();
        QJsonObject subCtx = ctx;
        for (auto k: merge.keys()) subCtx.insert(k, merge.value(k));
        auto execThunk = [this](const QString& n, const QJsonObject& a, const QJsonObject& c)->bool{
            return this->execAction(n, a, c);
        };
        std::unique_ptr<BTNode> node(BTBuilder::build(subTree, eval_.get(), execThunk));
        if (node) node->exec(subCtx, this, eval_.get());
        return true;
    }
    return false;
}

// ====== 配置加载（含基础校验 & 选项）======
bool ProtocolEngineTask::loadConfigFile(const QString& path){
    cfg_.bindings.clear(); cfg_.messages.clear(); cfg_.rules.clear();
    cfg_.msgIndex.clear(); cfg_.templates.clear(); cfg_.debug=true;

    QFile f(path);
    if (!f.open(QIODevice::ReadOnly)) return false;
    QJsonParseError er; QJsonDocument jd = QJsonDocument::fromJson(f.readAll(), &er);
    if (er.error!=QJsonParseError::NoError || !jd.isObject()) return false;
    QJsonObject root = jd.object();

    // options
    QJsonObject opt = root.value("options").toObject();
    cfg_.debug = opt.value("debug").toBool(true);
    int hexLimit = opt.value("debug_hex_limit").toInt(256);
    if (hexLimit <= 0) hexLimit = 256;
    g_debug_hex_limit.store(hexLimit);

    // bindings
    QJsonArray binds = root.value("bindings").toArray();
    for (int i=0;i<binds.size();++i){
        QJsonObject bo = binds[i].toObject();
        Binding b; b.id = bo.value("id").toString();
        b.udpTask = bo.value("udp_task").toString("udp");
        b.listenPort = bo.value("listen_port").toInt(0);
        QJsonArray peers = bo.value("peers").toArray();
        for (int j=0;j<peers.size();++j){
            QJsonObject po = peers[j].toObject();
            Binding::Peer p; p.ip = po.value("ip").toString(); p.port = po.value("port").toInt();
            b.peers.push_back(p);
        }
        cfg_.bindings.push_back(b);
    }

    // messages / templates
    QJsonArray msgs = root.value("messages").toArray();
    for (int i=0;i<msgs.size();++i){
        QJsonObject mo = msgs[i].toObject();
        if (mo.contains("compose")){
            TemplateDef t; t.id=mo.value("id").toString(); t.compose = mo.value("compose").toArray();
            cfg_.templates.insert(std::make_pair(t.id, t));
        }
        if (!mo.contains("binding")) continue; // 模板不加入 messages
        MessageDef md; md.id=mo.value("id").toString(); md.binding=mo.value("binding").toString();

        // match（支持 mask_hex）
        QJsonObject match = mo.value("match").toObject();
        auto parseCond = [&](const QJsonArray& arr, std::vector<MatchCond>& out){
            for (int k=0;k<arr.size();++k){
                QJsonObject c = arr[k].toObject();
                MatchCond mc; mc.offset=c.value("offset").toInt();
                mc.equals=hexToBytes(c.value("equals_hex").toString());
                if (c.contains("mask_hex")) mc.mask = hexToBytes(c.value("mask_hex").toString());
                out.push_back(mc);
            }
        };
        parseCond(match.value("all").toArray(), md.matchAll);
        parseCond(match.value("any").toArray(), md.matchAny);

        // fields
        QJsonArray fs = mo.value("fields").toArray();
        for (int k=0;k<fs.size();++k){
            QJsonObject fo = fs[k].toObject();
            FieldDef fd; fd.name=fo.value("name").toString();
            fd.type=fo.value("type").toString(); fd.offset=fo.value("offset").toInt();
            fd.endian=fo.value("endian").toString(); fd.lenFrom=fo.value("len_from").toString();
            fd.fixlen = fo.value("len").toInt(0);
            QJsonArray bits = fo.value("bits").toArray();
            for (int b=0;b<bits.size();++b){
                QJsonObject bo = bits[b].toObject();
                Bit bi; bi.name=bo.value("name").toString(); bi.pos=bo.value("pos").toInt(); bi.len=bo.value("len").toInt(1);
                fd.bits.push_back(bi);
            }
            md.fields.push_back(fd);
        }

        // checksum（解析阶段不使用）
        md.hasChecksum=false;
        QJsonObject cs = mo.value("checksum").toObject();
        if (!cs.isEmpty()){
            md.hasChecksum=true; md.checksum.algo=cs.value("algo").toString();
            QJsonObject range=cs.value("range").toObject();
            md.checksum.rangeFrom=range.value("from").toInt(0);
            md.checksum.rangeTo=range.value("to").isString()? range.value("to").toString(): QString::number(range.value("to").toInt(-1));
        }

        cfg_.msgIndex.insert(std::make_pair(md.id, (int)cfg_.messages.size()));
        cfg_.messages.push_back(md);
    }

    // rules
    QJsonArray rs = root.value("rules").toArray();
    for (int i=0;i<rs.size();++i){
        QJsonObject ro = rs[i].toObject();
        Rule r; r.id=ro.value("id").toString();
        r.when=ro.value("when").toString();
        r.message=ro.value("message").toString();
        r.binding=ro.value("binding").toString();
        r.bt = ro.value("bt").toObject();
        QJsonArray acts = ro.value("actions").toArray();
        for (int a=0;a<acts.size();++a){
            QJsonObject ao = acts[a].toObject();
            Action ac; ac.type=ao.value("type").toString(); ac.args=ao.value("args").toObject();
            r.actions.push_back(ac);
        }
        cfg_.rules.push_back(r);
    }

    // 基础 schema 校验：len_from 引用存在
    for (size_t i=0;i<cfg_.messages.size();++i){
        const auto& md = cfg_.messages[i];
        for (size_t j=0;j<md.fields.size();++j){
            const auto& fd = md.fields[j];
            if (!fd.lenFrom.isEmpty()){
                bool ok=false;
                for (size_t t=0;t<md.fields.size();++t){ if (md.fields[t].name==fd.lenFrom){ ok=true; break; } }
                if (!ok){
                    Message dbg; dbg.header.type="proto.debug";
                    QJsonObject meta; meta.insert("what","config.load.fail");
                    meta.insert("error", QString("len_from 引用不存在: messages[%1].fields[%2].len_from=%3")
                                .arg((int)i).arg((int)j).arg(fd.lenFrom));
                    dbg.payload=meta; bus_->publish("proto.debug", dbg);
                    return false;
                }
            }
        }
    }
    return true;
}

// ====== 匹配/解码 ======
bool ProtocolEngineTask::matchMessage(const MessageDef& md, const QByteArray& bytes) const {
    auto matchOne = [&](const MatchCond& c)->bool{
        if (c.offset<0 || c.offset + c.equals.size() > bytes.size()) return false;
        if (c.mask.isEmpty()){
            return bytes.mid(c.offset, c.equals.size()) == c.equals;
        }else{
            for (int i=0;i<c.equals.size();++i){
                uchar bb = (uchar)bytes[c.offset+i];
                uchar me = (uchar)c.equals[i];
                uchar ms = (uchar)(i<c.mask.size()? c.mask[i] : 0xFF);
                if ((bb & ms) != (me & ms)) return false;
            }
            return true;
        }
    };

    for (size_t i=0;i<md.matchAll.size();++i) if (!matchOne(md.matchAll[i])) return false;
    if (!md.matchAny.empty()){
        bool anyOk=false;
        for (size_t i=0;i<md.matchAny.size();++i){ if (matchOne(md.matchAny[i])) { anyOk=true; break; } }
        if (!anyOk) return false;
    }
    return true;
}

QJsonObject ProtocolEngineTask::decodeFields(const MessageDef& md, const QByteArray& bytes) const {
    QJsonObject o;
    for (size_t i=0;i<md.fields.size();++i){
        const FieldDef& f = md.fields[i];

        // ---- 标量整型 ----
        if (f.type=="u8"){
            quint8 v = FieldCodec::readU8(bytes, f.offset);
            if (f.bits.empty()){
                o.insert(f.name, (int)v);
            }else{
                QJsonObject bb;
                for (size_t b=0;b<f.bits.size();++b){
                    const Bit& bi=f.bits[b];
                    int mask = ((1<<bi.len)-1);
                    int val = (v >> bi.pos) & mask;
                    bb.insert(bi.name, val);
                }
                o.insert(f.name, bb);
            }
        }else if (f.type=="u16"){
            quint16 v = (f.endian=="le") ? FieldCodec::readU16LE(bytes, f.offset) : FieldCodec::readU16BE(bytes, f.offset);
            o.insert(f.name, (int)v);
        }else if (f.type=="u32"){
            quint32 v = (f.endian=="le") ? FieldCodec::readU32LE(bytes, f.offset) : FieldCodec::readU32BE(bytes, f.offset);
            o.insert(f.name, (double)v);
        }else if (f.type=="i16"){
            qint16 v = (f.endian=="le") ? (qint16)FieldCodec::readU16LE(bytes, f.offset) : (qint16)FieldCodec::readU16BE(bytes, f.offset);
            o.insert(f.name, (int)v);
        }else if (f.type=="i32"){
            qint32 v = (f.endian=="le") ? (qint32)FieldCodec::readU32LE(bytes, f.offset) : (qint32)FieldCodec::readU32BE(bytes, f.offset);
            o.insert(f.name, (double)v);
        }
        // ---- 浮点 ----
        else if (f.type=="float"){
            quint32 w = (f.endian=="le") ? FieldCodec::readU32LE(bytes, f.offset) : FieldCodec::readU32BE(bytes, f.offset);
            float fv; std::memcpy(&fv, &w, sizeof(float)); // 假定 IEEE754
            o.insert(f.name, (double)fv);
        }else if (f.type=="double"){
            quint32 hi, lo;
            if (f.endian=="le"){
                quint32 lo32 = FieldCodec::readU32LE(bytes, f.offset);
                quint32 hi32 = FieldCodec::readU32LE(bytes, f.offset+4);
                lo=lo32; hi=hi32;
            }else{
                quint32 hi32 = FieldCodec::readU32BE(bytes, f.offset);
                quint32 lo32 = FieldCodec::readU32BE(bytes, f.offset+4);
                lo=lo32; hi=hi32;
            }
            quint64 w = ((quint64)hi<<32) | lo;
            double dv; std::memcpy(&dv, &w, sizeof(double));
            o.insert(f.name, dv);
        }
        // ---- 原始字节/字符串/BCD ----
        else if (f.type=="bytes"){
            int len = f.fixlen;
            if (!f.lenFrom.isEmpty()){
                QJsonValue lv=o.value(f.lenFrom);
                len = lv.isDouble()? lv.toInt() : len;
            }
            QByteArray d = FieldCodec::readBytes(bytes, f.offset, len);
            o.insert(f.name+"_hex", bytesToHex(d, true));
        }else if (f.type=="string.fix"){
            int len = std::max(0, f.fixlen);
            QByteArray d = FieldCodec::readBytes(bytes, f.offset, len);
            o.insert(f.name, QString::fromLatin1(d));
        }else if (f.type=="string.from"){
            int len=0;
            if (!f.lenFrom.isEmpty()){
                QJsonValue lv=o.value(f.lenFrom);
                len = lv.isDouble()? lv.toInt() : 0;
            }
            QByteArray d = FieldCodec::readBytes(bytes, f.offset, len);
            o.insert(f.name, QString::fromLatin1(d));
        }else if (f.type=="bcd"){
            int len = f.fixlen;
            if (!f.lenFrom.isEmpty()){
                QJsonValue lv=o.value(f.lenFrom);
                len = lv.isDouble()? lv.toInt() : len;
            }
            QByteArray d = FieldCodec::readBytes(bytes, f.offset, len);
            o.insert(f.name, bcd_to_string(d));
        }
        // ---- 简单 TLV（T:1 L:1，可用 fixlen/len_from 限定 region）----
        else if (f.type=="tlv"){
            int region = f.fixlen;
            if (!f.lenFrom.isEmpty()){
                QJsonValue lv=o.value(f.lenFrom);
                region = lv.isDouble()? lv.toInt() : region;
            }
            if (region <= 0) region = bytes.size() - f.offset;
            int pos = f.offset;
            QJsonArray arr;
            while (pos + 2 <= f.offset + region){
                int t = (uchar)bytes[pos++];
                int l = (uchar)bytes[pos++];
                if (pos + l > f.offset + region) break;
                QByteArray v = bytes.mid(pos, l); pos += l;
                QJsonObject tv; tv.insert("t", t); tv.insert("l", l); tv.insert("v_hex", bytesToHex(v, false));
                arr.push_back(tv);
            }
            o.insert(f.name, arr);
        }
    }
    return o;
}

// ====== 校验写入（compose 阶段用）======
bool ProtocolEngineTask::applyChecksum(QByteArray& buf, const QJsonObject& cs){
    QString algo = cs.value("algo").toString("crc16_modbus");
    QJsonObject range = cs.value("range").toObject();
    int from=range.value("from").toInt(0), to=-1;
    if (range.value("to").isDouble()) to=range.value("to").toInt(-1);
    QString wend = cs.value("write_endian").toString(); // 可选: "le"/"be"，不设置时按算法惯例

    if (algo=="crc16_modbus"){
        quint16 crc=crc16_modbus_impl(buf, from, to);
        if (wend=="be") FieldCodec::writeU16BE(buf, crc); else FieldCodec::writeU16LE(buf, crc);
        return true;
    }else if (algo=="crc16_ccitt"){
        quint16 crc=crc16_ccitt_impl(buf, from, to);
        if (wend=="le") FieldCodec::writeU16LE(buf, crc); else FieldCodec::writeU16BE(buf, crc);
        return true;
    }else if (algo=="crc32"){
        quint32 crc=crc32_ieee_impl(buf, from, to);
        if (wend=="le") FieldCodec::writeU32LE(buf, crc); else FieldCodec::writeU32BE(buf, crc);
        return true;
    }else if (algo=="sum8"){
        uchar s = sum8_impl(buf, from, to);
        FieldCodec::writeU8(buf, s);
        return true;
    }else if (algo=="xor8"){
        uchar x = xor8_impl(buf, from, to);
        FieldCodec::writeU8(buf, x);
        return true;
    }
    return false;
}

// ====== 动作 ======
void ProtocolEngineTask::actPublish(const QJsonObject& args, const QJsonObject& ctx){
    QString topic = args.value("topic").toString("proto.event");
    QJsonObject payload = args.value("payload").toObject();

    // 简单替换 ${expr}
    QStringList ks = payload.keys();
    for (int i=0;i<ks.size();++i){
        QString k=ks[i];
        if (payload.value(k).isString()){
            QString v=payload.value(k).toString();
            if (v.startsWith("${") && v.endsWith("}")){
                QString expr=v.mid(2, v.length()-3);
                bool ok=false; QVariant rv = eval_->eval(expr, ctx, &ok);
                if (ok) payload.insert(k, QJsonValue::fromVariant(rv));
            }
        }
    }

    Message e; e.header.type = topic.toStdString();
    e.payload = payload;
    bus_->publish(topic.toStdString(), e);
}

void ProtocolEngineTask::actReply(const QJsonObject& args, const QJsonObject& ctx){
    QString tpl = args.value("template").toString();
    QString toPeer = args.value("to_peer").toString("source");
    QByteArray payload = composeFromTemplate(tpl, ctx);

    // hex 截断
    int hex_limit = g_debug_hex_limit.load();
    QString hex_full = bytesToHex(payload, true);
    QString hex_show = hex_full;
    if (hex_limit > 0 && hex_full.size() > hex_limit*3) {
        hex_show = hex_full.left(hex_limit*3) + " ...";
    }

    if (toPeer=="all"){
        QString bind = ctx.value("binding").toString();
        for (size_t i=0;i<cfg_.bindings.size();++i){
            if (cfg_.bindings[i].id==bind){
                for (size_t j=0;j<cfg_.bindings[i].peers.size();++j){
                    const Binding::Peer& p=cfg_.bindings[i].peers[j];
                    Message s; s.header.type="udp.send"; s.header.to="udp";
                    QJsonObject sp; sp.insert("ip", p.ip); sp.insert("port", p.port);
                    sp.insert("reliable", false); sp.insert("data_hex", bytesToHex(payload, false));
                    s.payload=sp; bus_->sendTo("udp", s);
                }
                if (cfg_.debug){
                    Message dbg; dbg.header.type="proto.debug"; QJsonObject meta; meta.insert("what","reply.sent.all");
                    meta.insert("bytes", hex_show); dbg.payload=meta; bus_->publish("proto.debug", dbg);
                }
                return;
            }
        }
    } else {
        QString ip = ctx.value("source_ip").toString();
        int     port = ctx.value("source_port").toInt();
        Message s; s.header.type="udp.send"; s.header.to="udp";
        QJsonObject sp; sp.insert("ip", ip); sp.insert("port", port);
        sp.insert("reliable", false); sp.insert("data_hex", bytesToHex(payload,false));
        s.payload=sp; bus_->sendTo("udp", s);
        if (cfg_.debug){
            Message dbg; dbg.header.type="proto.debug"; QJsonObject meta; meta.insert("what","reply.sent");
            meta.insert("ip", ip); meta.insert("port", port);
            meta.insert("bytes", hex_show); dbg.payload=meta; bus_->publish("proto.debug", dbg);
        }
    }
}

void ProtocolEngineTask::actTimer(const QJsonObject& args, const QJsonObject& ctx){
    int delay = args.value("delay_ms").toInt(1000);
    int repeat= args.value("repeat").toInt(1);
    QJsonObject inner = args.value("action").toObject();
    scheduleTimer(delay, repeat, inner, ctx);

    if (cfg_.debug){
        Message dbg; dbg.header.type="proto.debug"; QJsonObject meta; meta.insert("what","timer.scheduled");
        meta.insert("delay_ms", delay); meta.insert("repeat", repeat); dbg.payload=meta; bus_->publish("proto.debug", dbg);
    }
}

void ProtocolEngineTask::actStateSet(const QJsonObject& args, const QJsonObject& ctx){
    (void)ctx;
    QString scope=args.value("scope").toString("session");
    QString key  =args.value("key").toString();
    QString expr =args.value("expr").toString();
    bool ok=false; QVariant v=eval_->eval(expr, ctx, &ok);
    if (!ok) return;
    if (scope=="global") {
        // 如需要可扩展全局存储；此处仍然把会话态写入 datahub_ 便于 UI 观察
        datahub_[key]=v;
    } else {
        datahub_[key]=v;
    }

    if (cfg_.debug){
        Message dbg; dbg.header.type="proto.debug";
        QJsonObject meta; meta.insert("what","state.set"); meta.insert("scope", scope);
        meta.insert("key", key); meta.insert("value", QJsonValue::fromVariant(v));
        dbg.payload=meta; bus_->publish("proto.debug", dbg);
    }
}

// ====== 组包 ======
QByteArray ProtocolEngineTask::composeFromTemplate(const QString& tplId, const QJsonObject& ctx){
    auto it = cfg_.templates.find(tplId);
    if (it==cfg_.templates.end()) return QByteArray();
    return composeBySteps(it->second.compose, ctx);
}

QByteArray ProtocolEngineTask::composeBySteps(const QJsonArray& steps, const QJsonObject& ctx){
    QByteArray out;
    for (int i=0;i<steps.size();++i){
        QJsonObject st = steps[i].toObject();
        if (st.contains("type")){
            QString ty = st.value("type").toString();
            if (ty=="u8"){
                if (st.contains("value_hex")){
                    QByteArray v = hexToBytes(st.value("value_hex").toString());
                    if (!v.isEmpty()) FieldCodec::writeU8(out, (quint8)(uchar)v[0]);
                }else if (st.contains("expr")){
                    bool ok=false; quint8 v=(quint8)eval_->eval(st.value("expr").toString(), ctx, &ok).toInt();
                    FieldCodec::writeU8(out, v);
                }
            }else if (ty=="u16"){
                bool le=st.value("endian").toString()=="le";
                bool ok=false; quint16 v=(quint16)eval_->eval(st.value("expr").toString(), ctx, &ok).toInt();
                if (le) FieldCodec::writeU16LE(out, v); else FieldCodec::writeU16BE(out, v);
            }else if (ty=="u32"){
                bool le=st.value("endian").toString()=="le";
                bool ok=false; quint32 v=(quint32)eval_->eval(st.value("expr").toString(), ctx, &ok).toUInt();
                if (le) FieldCodec::writeU32LE(out, v); else FieldCodec::writeU32BE(out, v);
            }else if (ty=="bytes"){
                bool ok=false; QVariant vv=eval_->eval(st.value("from").toString(), ctx, &ok);
                QByteArray b; if (ok && vv.type()==QVariant::String) b = hexToBytes(vv.toString());
                FieldCodec::writeBytes(out, b);
            }else if (ty=="checksum"){
                applyChecksum(out, st.value("checksum").toObject());
            }
        } else if (st.contains("checksum")){
            applyChecksum(out, st.value("checksum").toObject());
        }
    }
    return out;
}

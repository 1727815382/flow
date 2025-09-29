#include "modules/replay_service/replay_task.hpp"
#include "core/message_bus.hpp"
#include <QDateTime>
#include <QJsonDocument>
#include <algorithm>

static long long now_ms() { return (long long)QDateTime::currentMSecsSinceEpoch(); }
long long ReplayTask::nowMs() { return now_ms(); }

void ReplayTask::onMessage(Message& m) {
    const std::string& t = m.header.type;

    if (t == "replay.record.start") {
        QString path = m.payload.value("file").toString("replay.ndjson");
        if (file_.isOpen()) file_.close();
        file_.setFileName(path);
        if (!file_.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text)) {
            reportException("replay.record.start", "open file fail");
            return;
        }
        ts_.setDevice(&file_);
        ts_.setCodec("UTF-8");
        startEpochMs_ = nowMs();
        QJsonObject meta{{"kind", "meta"}, {"app", "your_app"}, {"start_ms", (qint64)startEpochMs_}, {"version", "1.0"}};
        ts_ << QString::fromUtf8(QJsonDocument(meta).toJson(QJsonDocument::Compact)) << "\n";
        ts_.flush();
        mode_ = RECORDING;
        return;
    }

    if (t == "replay.record.stop") {
        if (file_.isOpen()) file_.close();
        mode_ = IDLE;
        return;
    }

    if (t == "replay.play.load") {
        QString path = m.payload.value("file").toString();
        stopPlay();
        loadFile(path);
        return;
    }

    if (t == "replay.play.start") {
        double r = m.payload.value("rate").toDouble(1.0);
        injected_ = !m.payload.value("mirror").toBool(false);
        startPlay(r);
        return;
    }

    if (t == "replay.play.pause") { mode_ = PAUSED; return; }
    if (t == "replay.play.stop") { stopPlay(); return; }
    if (t == "replay.play.seek") { long long rel = m.payload.value("ms").toVariant().toLongLong(); seekTo(rel); return; }

    // 录制阶段：收集三类镜像事件
    if (mode_ == RECORDING) {
        if (m.header.type == "ui.render" || m.header.type == "ui.nav" || m.header.type == "ui.cmd") {
            recordLine(QString::fromStdString(m.header.type), m.payload);
        }
    }
}

void ReplayTask::recordLine(const QString& kind, const QJsonObject& data) {
    long long rel = nowMs() - startEpochMs_;
    QJsonObject rec{{"kind", kind}, {"t", (qint64)rel}, {"data", data}};
    ts_ << QString::fromUtf8(QJsonDocument(rec).toJson(QJsonDocument::Compact)) << "\n";
    ts_.flush();
}

void ReplayTask::loadFile(const QString& path) {
    recs_.clear();
    playIdx_ = 0;
    QFile f(path);
    if (!f.open(QIODevice::ReadOnly | QIODevice::Text)) {
        reportException("replay.play.load", "open fail");
        return;
    }

    while (!f.atEnd()) {
        QByteArray line = f.readLine().trimmed();
        if (line.isEmpty()) continue;
        QJsonParseError er;
        QJsonDocument jd = QJsonDocument::fromJson(line, &er);
        if (er.error != QJsonParseError::NoError || !jd.isObject()) continue;
        QJsonObject o = jd.object();
        QString kind = o.value("kind").toString();
        if (kind == "meta") continue;
        Rec r;
        r.kind = kind;
        r.t = o.value("t").toVariant().toLongLong();
        r.data = o.value("data").toObject();
        recs_.push_back(r);
    }

    std::sort(recs_.begin(), recs_.end(), [](const Rec& a, const Rec& b) { return a.t < b.t; });
    mode_ = PAUSED;
}

void ReplayTask::startPlay(double rate) {
    if (recs_.empty()) return;
    rate_ = (rate > 0 ? rate : 1.0);
    stopFlag_.store(false);
    if (playThread_.joinable()) playThread_.join();
    mode_ = PLAYING;
    playThread_ = std::thread(&ReplayTask::playLoop, this);
}

void ReplayTask::stopPlay() {
    stopFlag_.store(true);
    mode_ = IDLE;
    if (playThread_.joinable()) playThread_.join();
}

void ReplayTask::seekTo(long long relMs) {
    size_t L = 0, R = recs_.size();
    while (L < R) {
        size_t M = (L + R) / 2;
        if (recs_[M].t < relMs) L = M + 1; else R = M;
    }
    playIdx_ = L;
}

void ReplayTask::playLoop() {
    if (playIdx_ >= recs_.size()) playIdx_ = 0;
    long long baseWall = nowMs();
    long long baseT = (playIdx_ < recs_.size() ? recs_[playIdx_].t : 0);

    while (!stopFlag_.load() && mode_ == PLAYING && playIdx_ < recs_.size()) {
        Rec r = recs_[playIdx_];
        long long due = baseWall + (long long)((r.t - baseT) / rate_);
        long long now = nowMs();

        if (now < due) {
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
            continue;
        }

        dispatch(r);
        ++playIdx_;
    }
    mode_ = PAUSED;
}

void ReplayTask::dispatch(const Rec& r) {
    if (r.kind == "ui.render") {
        Message e;
        e.header.type = "ui.event";
        e.header.to = "ui";
        e.header.from = id();
        e.payload = r.data;
        sendToBus(std::move(e));
    } else if (r.kind == "ui.nav") {
        Message e;
        e.header.type = "ui.nav";
        e.header.to = "ui";
        e.header.from = id();
        e.payload = r.data;
        sendToBus(std::move(e));
    } else if (r.kind == "ui.cmd") {
        if (injected_) {
            Message e;
            e.header.type = "ui.event";
            e.header.to = "ui";
            e.header.from = id();
            QJsonObject d = r.data;
            d["type"] = "ui.cmd.preview";
            e.payload = d;
            sendToBus(std::move(e));
        } else {
            Message cmd;
            cmd.header.from = id();
            cmd.header.type = r.data.value("_type").toString().toStdString();
            cmd.header.to = r.data.value("_to").toString().toStdString();
            cmd.header.topic = r.data.value("_topic").toString().toStdString();
            cmd.payload = r.data.value("payload").toObject();
            sendToBus(std::move(cmd));
        }
    }
}

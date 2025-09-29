#include "ui/builtin_views/view_udp_console.hpp"
#include "ui/ui_bridge.hpp"
#include "core/message.hpp"
#include "core/message_bus.hpp"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QDateTime>
#include <QJsonArray>

static QString nowStr() {
    return QDateTime::currentDateTime().toString("HH:mm:ss.zzz");
}

ViewUdpConsole::ViewUdpConsole(QWidget* parent, MessageBus* bus, UiBridge* bridge)
    : QWidget(parent), bus_(bus) {

    QVBoxLayout* layout = new QVBoxLayout(this);

    console_ = new QTextEdit(this);
    console_->setReadOnly(true);
    layout->addWidget(console_);

    QHBoxLayout* row1 = new QHBoxLayout();
    clearButton_ = new QPushButton("Clear Console", this);
    testLogBtn_  = new QPushButton("Send Test Log", this);
    row1->addWidget(clearButton_);
    row1->addWidget(testLogBtn_);
    layout->addLayout(row1);

    connect(clearButton_, &QPushButton::clicked, this, [this]() { console_->clear(); });
    connect(testLogBtn_,  &QPushButton::clicked, this, &ViewUdpConsole::onSendTestLog);

    QHBoxLayout* row2 = new QHBoxLayout();
    recStartBtn_ = new QPushButton("Replay: Start Rec", this);
    recStopBtn_  = new QPushButton("Replay: Stop Rec",  this);
    playBtn_     = new QPushButton("Replay: Play",      this);
    emitCmdBtn_  = new QPushButton("Emit ui.cmd",       this);
    row2->addWidget(recStartBtn_);
    row2->addWidget(recStopBtn_);
    row2->addWidget(playBtn_);
    row2->addWidget(emitCmdBtn_);
    layout->addLayout(row2);

    connect(recStartBtn_, &QPushButton::clicked, this, &ViewUdpConsole::onReplayStart);
    connect(recStopBtn_,  &QPushButton::clicked, this, &ViewUdpConsole::onReplayStop);
    connect(playBtn_,     &QPushButton::clicked, this, &ViewUdpConsole::onReplayPlay);
    connect(emitCmdBtn_,  &QPushButton::clicked, this, &ViewUdpConsole::onEmitUiCmd);

    // 监听 UI 桥
    connect(bridge, SIGNAL(uiMessage(QString,QJsonObject)),
            this,   SLOT(onUiMessage(QString,QJsonObject)));
}

void ViewUdpConsole::onSendTestLog() 
{
    Message m; 
    m.header.type="log.write"; 
    m.header.to="log";
    m.payload = QJsonObject{{"level","info"},{"tag","ui"},{"msg","Hello from UI button"}};
    bus_->sendTo("log", m);
}

void ViewUdpConsole::onUiMessage(const QString& type, const QJsonObject& payload) {
    if (type == "udp.recv") {
        const QString ip  = payload.value("ip").toString();
        const int     port= payload.value("port").toInt();
        const int     len = payload.value("len").toInt();
        const QString hex = payload.value("data_hex").toString();
        if (!hex.isEmpty())
            console_->append(QString("[%1][RX] %2:%3  len=%4  %5").arg(nowStr(), ip).arg(port).arg(len).arg(hex));
        else
            console_->append(QString("[%1][RX] %2:%3  len=%4").arg(nowStr(), ip).arg(port).arg(len));
    } else if (type == "udp.ack") {
        console_->append(QString("[%1][ACK] id=%2").arg(nowStr()).arg(payload.value("msg_id").toInt()));
    } else if (type == "udp.drop") {
        console_->append(QString("[%1][DROP] id=%2 reason=%3")
                         .arg(nowStr()).arg(payload.value("msg_id").toInt())
                         .arg(payload.value("reason").toString()));
    } else if (type.startsWith("udp.socket")) {
        console_->append(QString("[%1][SOCKET] %2").arg(nowStr(), type));
    } else if (type == "log.event") {
        console_->append(QString("[%1][LOG][%2][%3] %4")
                         .arg(nowStr(),
                              payload.value("level").toString(),
                              payload.value("tag").toString(),
                              payload.value("msg").toString()));
    } else if (type == "metrics.summary") {
        const int n = payload.value("items").toArray().size();
        console_->append(QString("[%1][METRICS] %2 items").arg(nowStr()).arg(n));
    } else if (type == "replay.state") {
        console_->append(QString("[%1][REPLAY] %2").arg(nowStr(), payload.value("state").toString()));
    } else if (type == "ui.nav") {
        console_->append(QString("[%1][NAV] view=%2").arg(nowStr(), payload.value("view").toString()));
    }
}

void ViewUdpConsole::onReplayStart() {
    Message r; r.header.to="replay"; r.header.type="replay.record.start";
    r.payload = QJsonObject{{"file","replay_demo.ndjson"}};
    bus_->sendTo("replay", r);
    console_->append(QString("[%1] start recording").arg(nowStr()));
}
void ViewUdpConsole::onReplayStop() {
    Message r; r.header.to="replay"; r.header.type="replay.record.stop";
    bus_->sendTo("replay", r);
    console_->append(QString("[%1] stop recording").arg(nowStr()));
}
void ViewUdpConsole::onReplayPlay() {
    Message a; a.header.to="replay"; a.header.type="replay.play.load";
    a.payload = QJsonObject{{"file","replay_demo.ndjson"}};
    bus_->sendTo("replay", a);

    Message b; b.header.to="replay"; b.header.type="replay.play.start";
    b.payload = QJsonObject{{"rate",1.0},{"mirror",true}};
    bus_->sendTo("replay", b);
    console_->append(QString("[%1] play loaded file").arg(nowStr()));
}
void ViewUdpConsole::onEmitUiCmd() {
    Message c; c.header.type="ui.cmd";
    c.payload = QJsonObject{
        {"_type","log.write"}, {"_to","log"}, {"_topic",""},
        {"payload", QJsonObject{{"level","info"},{"tag","replay"},{"msg","manual-cmd"}}}
    };
    bus_->publish("ui.cmd", c);
    console_->append(QString("[%1] emit ui.cmd(log.write)").arg(nowStr()));
}

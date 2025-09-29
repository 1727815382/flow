#include "ui/builtin_views/view_protocol_console.hpp"
#include <QJsonDocument>
#include <QTextCursor>

ViewProtocolConsole::ViewProtocolConsole(QWidget* parent, MessageBus* bus, UiBridge* bridge)
    : QWidget(parent), bus_(bus)
{
    setWindowTitle("Protocol Console");

    // —— 控件 —— //
    log_ = new QTextEdit(this);
    log_->setReadOnly(true);

    path_ = new QLineEdit(this);
    path_->setText("E:/work/github_proj/flow/configs/proto_demo.json");
    QPushButton* btnLoad = new QPushButton("加载配置", this);

    ip_ = new QLineEdit(this);   ip_->setText("127.0.0.1");
    port_ = new QLineEdit(this); port_->setText("9000");
    hex_ = new QLineEdit(this);  hex_->setPlaceholderText("01 00 2A 00 03 AA BB CC");
    QPushButton* btnSend      = new QPushButton("发送原始HEX", this);
    QPushButton* btnDemoPing  = new QPushButton("发送Demo Ping", this);
    QPushButton* btnStatusReq = new QPushButton("发送StatusReq", this);

    dhKey_ = new QLineEdit(this); dhKey_->setText("ui.mode");
    dhVal_ = new QLineEdit(this); dhVal_->setText("auto");
    QPushButton* btnSetDH = new QPushButton("设置DataHub", this);
    QPushButton* btnClear = new QPushButton("清空日志", this);

    // —— 布局 —— //
    QFormLayout* lf = new QFormLayout();
    lf->addRow("配置文件：", path_);
    QHBoxLayout* l1 = new QHBoxLayout();
    l1->addWidget(btnLoad);
    l1->addStretch();
    QGroupBox* gLoad = new QGroupBox("配置");
    QVBoxLayout* vg1 = new QVBoxLayout(); vg1->addLayout(lf); vg1->addLayout(l1); gLoad->setLayout(vg1);

    QFormLayout* fs = new QFormLayout();
    fs->addRow("目标IP：", ip_);
    fs->addRow("目标端口：", port_);
    fs->addRow("HEX：", hex_);
    QHBoxLayout* l2 = new QHBoxLayout();
    l2->addWidget(btnSend);
    l2->addWidget(btnDemoPing);
    l2->addWidget(btnStatusReq);
    QGroupBox* gSend = new QGroupBox("发送");
    QVBoxLayout* vg2 = new QVBoxLayout(); vg2->addLayout(fs); vg2->addLayout(l2); gSend->setLayout(vg2);

    QFormLayout* fd = new QFormLayout();
    fd->addRow("Key：", dhKey_);
    fd->addRow("Value：", dhVal_);
    QHBoxLayout* l3 = new QHBoxLayout();
    l3->addWidget(btnSetDH);
    l3->addWidget(btnClear);
    QGroupBox* gDH = new QGroupBox("DataHub");
    QVBoxLayout* vg3 = new QVBoxLayout(); vg3->addLayout(fd); vg3->addLayout(l3); gDH->setLayout(vg3);

    QVBoxLayout* root = new QVBoxLayout();
    root->addWidget(gLoad);
    root->addWidget(gSend);
    root->addWidget(gDH);
    root->addWidget(new QLabel("流程日志："));
    root->addWidget(log_, 1);
    setLayout(root);

    // —— 连接 —— //
    connect(btnLoad,      &QPushButton::clicked, this, &ViewProtocolConsole::onLoadConfig);
    connect(btnSend,      &QPushButton::clicked, this, &ViewProtocolConsole::onSendHex);
    connect(btnDemoPing,  &QPushButton::clicked, this, &ViewProtocolConsole::onSendDemoPing);
    connect(btnStatusReq, &QPushButton::clicked, this, &ViewProtocolConsole::onSendStatusReq);
    connect(btnSetDH,     &QPushButton::clicked, this, &ViewProtocolConsole::onSetDataHub);
    connect(btnClear,     &QPushButton::clicked, this, &ViewProtocolConsole::onClear);

    connect(bridge, SIGNAL(uiMessage(QString,QJsonObject)),
            this,   SLOT(onBusJson(QString,QJsonObject)));
}

void ViewProtocolConsole::onLoadConfig(){
    Message m; m.header.type="proto.config.load"; m.header.to="proto";
    QJsonObject p; p.insert("file", path_->text());
    m.payload = p;
    bus_->sendTo("proto", m);
    appendLine(QString("→ 发送: proto.config.load file=%1").arg(path_->text()));
}

void ViewProtocolConsole::onSendHex(){
    Message s; s.header.type="udp.send"; s.header.to="udp";
    QJsonObject sp;
    sp.insert("ip", ip_->text());
    sp.insert("port", port_->text().toInt());
    sp.insert("reliable", false);
    sp.insert("data_hex", hex_->text());
    s.payload = sp;
    bus_->sendTo("udp", s);
    appendLine(QString("→ 发送 UDP 到 %1:%2, HEX=%3").arg(ip_->text()).arg(port_->text()).arg(hex_->text()));
}

void ViewProtocolConsole::onSendDemoPing(){
    QString demo = "01 00 2A 00 03 AA BB CC";
    hex_->setText(demo);
    onSendHex();
}

void ViewProtocolConsole::onSendStatusReq(){
    QString demo = "10 01";
    hex_->setText(demo);
    onSendHex();
}

void ViewProtocolConsole::onSetDataHub(){
    Message m; m.header.type="datahub.set"; m.header.to="datahub";
    QJsonObject p; p.insert("key", dhKey_->text()); p.insert("value", dhVal_->text());
    m.payload = p;
    bus_->sendTo("datahub", m);
    appendLine(QString("→ DataHub 设置：%1 = %2").arg(dhKey_->text()).arg(dhVal_->text()));
}

void ViewProtocolConsole::onClear(){ log_->clear(); }

void ViewProtocolConsole::onBusJson(const QString& topic, const QJsonObject& payload){
    QString js = QString::fromUtf8(QJsonDocument(payload).toJson(QJsonDocument::Compact));
    if (topic=="proto.debug"){
        QString what = payload.value("what").toString();
        if (what=="recv.decoded"){
            QJsonObject det = payload.value("detail").toObject();
            appendLine(QString("🟢 命中消息: %1 @%2")
                       .arg(det.value("message").toString(),
                            det.value("binding").toString()));
            appendLine(QString("    字段: %1")
                       .arg(QString::fromUtf8(QJsonDocument(det.value("fields").toObject())
                                              .toJson(QJsonDocument::Compact))));
            return;
        } else if (what=="reply.sent" || what=="reply.sent.all"){
            appendLine(QString("📤 已回复: %1").arg(js)); return;
        } else if (what=="bt.start"){
            appendLine(QString("🌳 行为树开始: %1").arg(js)); return;
        } else if (what=="bt.done"){
            appendLine(QString("✅ 行为树结束: %1").arg(js)); return;
        } else if (what=="timer.scheduled"){
            appendLine(QString("⏱ 定时器: %1").arg(js)); return;
        } else if (what=="state.set"){
            appendLine(QString("🗂 状态更新: %1").arg(js)); return;
        }
    } else if (topic=="proto.error"){
        appendLine(QString("❌ 错误: %1").arg(js)); return;
    } else if (topic=="proto.status"){
        appendLine(QString("ℹ️ 状态: %1").arg(js)); return;
    } else if (topic=="udp.recv"){
        appendLine(QString("📥 收到 UDP: %1").arg(js)); return;
    } else if (topic=="datahub.value"){
        appendLine(QString("🔄 DataHub: %1").arg(js)); return;
    }
    appendLine(QString("%1 %2").arg(topic, js));
}

void ViewProtocolConsole::appendLine(const QString& s){
    log_->append(s);
    log_->moveCursor(QTextCursor::End);
}

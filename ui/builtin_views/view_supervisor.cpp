#include "ui/builtin_views/view_supervisor.hpp"
#include "ui/ui_bridge.hpp"
#include "core/message.hpp"
#include "core/message_bus.hpp"
#include <QVBoxLayout>
#include <QJsonArray>
#include <QVariant>

ViewSupervisor::ViewSupervisor(QWidget* parent, MessageBus* bus, UiBridge* bridge)
    : QWidget(parent), bus_(bus),
      statusLabel_(new QLabel("metrics: (waiting ...)", this)),
      flushBtn_(new QPushButton("Flush Metrics", this)) {

    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->addWidget(statusLabel_);
    layout->addWidget(flushBtn_);

    connect(flushBtn_, &QPushButton::clicked, this, &ViewSupervisor::onFlushClicked);
    connect(bridge, SIGNAL(uiMessage(QString,QJsonObject)),
            this,   SLOT(onUiMessage(QString,QJsonObject)));
}

void ViewSupervisor::onFlushClicked() {
    Message m; m.header.type = "metrics.flush"; m.header.to = "metrics";
    bus_->sendTo("metrics", m);
}

void ViewSupervisor::onUiMessage(const QString& type, const QJsonObject& payload) {
    if (type == "metrics.summary") {
        const qint64 ts = payload.value("ts").toVariant().toLongLong();
        const QJsonArray items = payload.value("items").toArray();
        statusLabel_->setText(QString("metrics: %1 items, ts=%2").arg(items.size()).arg((qlonglong)ts));
    }
}

#include "ui/ui_adapter_task.hpp"
#include "core/message.hpp"
#include <QMetaObject>
#include <QMetaType>

UiAdapterTask::UiAdapterTask(MessageBus* bus, UiBridge* bridge)
    : TaskBase("ui_bridge", bus), bridge_(bridge) {
    qRegisterMetaType<QJsonObject>("QJsonObject");
}

void UiAdapterTask::onStart() {
    status_ = TS_RUNNING;
    // 按需订阅协议相关主题（你要看的都加上）
    bus_->subscribe(id(), "proto.debug");
    bus_->subscribe(id(), "proto.error");
    bus_->subscribe(id(), "proto.status");
    bus_->subscribe(id(), "datahub.value");
    bus_->subscribe(id(), "udp.recv");
}

void UiAdapterTask::onMessage(Message& m) {
    const QString topic = QString::fromUtf8(m.header.type.c_str());
    const QJsonObject payload = m.payload; // 拷贝到本线程栈
    // 切到 UI 线程调用 UiBridge::post → 发 uiMessage 信号
    QMetaObject::invokeMethod(
        bridge_, "post", Qt::QueuedConnection,
        Q_ARG(QString, topic),
        Q_ARG(QJsonObject, payload)
    );
}

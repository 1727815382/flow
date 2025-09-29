#pragma once
#include <QWidget>
#include <QTextEdit>
#include <QPushButton>
#include <QLineEdit>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGroupBox>
#include <QLabel>
#include <QJsonObject>
#include "core/message_bus.hpp"
#include "ui/ui_adapter_task.hpp"   // UiBridge 声明

class ViewProtocolConsole : public QWidget {
    Q_OBJECT
public:
    explicit ViewProtocolConsole(QWidget* parent, MessageBus* bus, UiBridge* bridge);

private slots:
    void onLoadConfig();
    void onSendHex();
    void onSendDemoPing();
    void onSendStatusReq();
    void onSetDataHub();
    void onClear();
    void onBusJson(const QString& topic, const QJsonObject& payload);

private:
    void appendLine(const QString& s);

private:
    MessageBus* bus_;
    QTextEdit*  log_;
    QLineEdit *path_, *ip_, *port_, *hex_, *dhKey_, *dhVal_;
};

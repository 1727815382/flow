#pragma once
#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QJsonObject>

class MessageBus;
class UiBridge;

class ViewSupervisor : public QWidget {
    Q_OBJECT
public:
    explicit ViewSupervisor(QWidget* parent, MessageBus* bus, UiBridge* bridge);

private slots:
    void onUiMessage(const QString& type, const QJsonObject& payload);
    void onFlushClicked();

private:
    MessageBus* bus_;
    QLabel* statusLabel_;
    QPushButton* flushBtn_;
};

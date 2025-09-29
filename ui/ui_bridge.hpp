#pragma once
#include <QObject>
#include <QJsonObject>

class UiBridge : public QObject {
    Q_OBJECT
public:
    explicit UiBridge(QObject* parent=nullptr) : QObject(parent) {}

public slots:
    // 后台任务线程通过 invokeMethod(Queued) 调用它
    void post(const QString& type, const QJsonObject& payload) {
        emit uiMessage(type, payload);
    }

signals:
    // 所有视图只用监听这个信号即可
    void uiMessage(const QString& type, const QJsonObject& payload);
};

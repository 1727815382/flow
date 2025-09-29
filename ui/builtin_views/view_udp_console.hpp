#pragma once
#include <QWidget>
#include <QTextEdit>
#include <QPushButton>
#include <QJsonObject>

class UiBridge;
class MessageBus;

class ViewUdpConsole : public QWidget {
    Q_OBJECT
public:
    explicit ViewUdpConsole(QWidget* parent, MessageBus* bus, UiBridge* bridge);

    // ...
private slots:
    void onUiMessage(const QString& type, const QJsonObject& payload);
    void onSendTestLog();

    // 新增：回放相关
    void onReplayStart();
    void onReplayStop();
    void onReplayPlay();
    void onEmitUiCmd(); // 录制期内发一条 ui.cmd（实际是 log.write）

private:
    MessageBus* bus_;
    QTextEdit* console_;
    QPushButton* clearButton_;
    QPushButton* testLogBtn_;

    // 新增：回放按钮
    QPushButton* recStartBtn_;
    QPushButton* recStopBtn_;
    QPushButton* playBtn_;
    QPushButton* emitCmdBtn_;

};

#pragma once
#include "core/task_base.hpp"
#include <QFile>
#include <QTextStream>
#include <vector>

class ReplayTask : public TaskBase {
public:
    explicit ReplayTask(MessageBus* bus)
        : TaskBase("replay", bus), mode_(IDLE), rate_(1.0), playIdx_(0),
          startEpochMs_(0), injected_(true), stopFlag_(false) {}

protected:
    void onStart(){ status_ = TS_RUNNING; }
    void onStop(){
        stopFlag_.store(true);
        if (playThread_.joinable()) playThread_.join();
        if (file_.isOpen()) file_.close();
    }
    void onMessage(Message& m);

private:
    enum Mode { IDLE, RECORDING, PLAYING, PAUSED };
    struct Rec {
        long long t;
        QString   kind;   // "ui.render" / "ui.nav" / "ui.cmd"
        QJsonObject data;
    };

    Mode mode_;
    double rate_;
    std::vector<Rec> recs_;
    size_t playIdx_;
    long long startEpochMs_;
    bool injected_;
    std::atomic<bool> stopFlag_;
    std::thread playThread_;
    QFile file_;
    QTextStream ts_;

    static long long nowMs();
    void recordLine(const QString& kind, const QJsonObject& data);
    void loadFile(const QString& path);
    void startPlay(double rate);
    void stopPlay();
    void seekTo(long long relMs);
    void playLoop();
    void dispatch(const Rec& r);
};

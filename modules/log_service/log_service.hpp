#pragma once
#include "core/task_base.hpp"
#include <QFile>
#include <QTextStream>
#include <QDateTime>

class LogService : public TaskBase {
public:
    explicit LogService(MessageBus* bus)
        : TaskBase("log", bus), toFile_(false), alsoStderr_(true) {}

protected:
    void onStart() { status_ = TS_RUNNING; }
    void onStop() {
        if (file_.isOpen()) file_.close();
    }
    void onMessage(Message& m);

private:
    QFile file_;
    QTextStream ts_;
    bool toFile_;
    bool alsoStderr_;

    static QString nowStr() {
        return QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz");
    }

    void applyConfig(const QJsonObject& o) {
        if (o.contains("file")) {
            QString path = o.value("file").toString();
            if (file_.isOpen()) file_.close();
            file_.setFileName(path);
            if (file_.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text)) {
                ts_.setDevice(&file_);
                ts_.setCodec("UTF-8");
                toFile_ = true;
            } else {
                toFile_ = false;
                reportException("log.config", "open log file fail");
            }
        }
        if (o.contains("stderr")) {
            alsoStderr_ = o.value("stderr").toBool(true);
        }
    }

    void writeLine(const QString& level, const QString& tag, const QString& msg) {
        const QString line = QString("[%1] [%2] [%3] %4\n")
                                .arg(nowStr(), level, tag, msg);
        if (toFile_) { ts_ << line; ts_.flush(); }
        if (alsoStderr_) {
#ifdef _WIN32
            fprintf(stderr, "%s", line.toLocal8Bit().constData());
#else
            fprintf(stderr, "%s", line.toUtf8().constData());
#endif
        }
    }
};

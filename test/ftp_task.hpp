#pragma once
#include "core/taskbase.hpp"
#include "core/message_bus.hpp"
#include <QFile>
#include <QThread>

#if defined(USE_CURL_FTP)
// 基于 libcurl 的实现
#include <curl/curl.h>
class FtpTask : public TaskBase {
    Q_OBJECT
public:
    void init() override {
        MessageBus::instance().subscribe([this](Message& m){ onMessage(m); });
        curl_global_init(CURL_GLOBAL_DEFAULT);
    }
    void stop() override { curl_global_cleanup(); }

private:
    static size_t read_cb(void* ptr,size_t size,size_t nmemb,void* stream){
        QFile* f = static_cast<QFile*>(stream);
        return f->read((char*)ptr, size*nmemb);
    }
    void onMessage(Message& m){
        if(m.header.type=="ftp.put"){
            const auto p=m.payload;
            const QString url = p.value("url").toString(); // ftp://user:pwd@host/path/file
            const QString path= p.value("file").toString();
            QFile f(path);
            if(!f.open(QIODevice::ReadOnly)){
                publish("ftp.error", QJsonObject{{"where","open"},{"file",path}});
                return;
            }
            CURL* h = curl_easy_init();
            curl_easy_setopt(h, CURLOPT_URL, url.toUtf8().constData());
            curl_easy_setopt(h, CURLOPT_UPLOAD, 1L);
            curl_easy_setopt(h, CURLOPT_READFUNCTION, &FtpTask::read_cb);
            curl_easy_setopt(h, CURLOPT_READDATA, &f);
            auto rc = curl_easy_perform(h);
            curl_easy_cleanup(h);
            if(rc==CURLE_OK) publish("ftp.done", QJsonObject{{"op","put"},{"url",url}});
            else publish("ftp.error", QJsonObject{{"where","put"},{"code",int(rc)}});
        }else if(m.header.type=="ftp.get"){
            // 留作扩展：下载
            publish("ftp.error", QJsonObject{{"where","get"},{"msg","not implemented in curl branch"}});
        }
    }
    static void publish(const std::string& t, const QJsonObject& p){
        Message mm; mm.header.type=t; mm.payload=p; MessageBus::instance().post(mm);
    }
};
#else
// 基于你项目内的 QFtp（qftp.h）实现；若你项目已有实现只需包含头并链接
#include "qftp.h"
class FtpTask : public TaskBase {
    Q_OBJECT
public:
    explicit FtpTask(QObject* p=nullptr):TaskBase(p){}
    void init() override {
        MessageBus::instance().subscribe([this](Message& m){ onMessage(m); });
        _ftp = new QFtp(this);
        connect(_ftp, SIGNAL(done(bool)), this, SLOT(onDone(bool)));
        connect(_ftp, SIGNAL(commandFinished(int,bool)), this, SLOT(onCmd(int,bool)));
    }
    void stop() override { if(_ftp) _ftp->close(); }

private:
    QFtp* _ftp{}; QString _lastUrl; QString _lastFile;

    void onMessage(Message& m){
        if(m.header.type=="ftp.put"){
            const auto p=m.payload;
            _lastUrl = p.value("url").toString(); // ftp://user:pwd@host:21/path
            _lastFile= p.value("file").toString();

            QUrl u(_lastUrl); _ftp->connectToHost(u.host(), u.port(21));
            if(!u.userName().isEmpty()) _ftp->login(u.userName(), u.password()); else _ftp->login();
            if(!u.path().isEmpty()) _ftp->cd(u.path().left(u.path().lastIndexOf('/')));
            QFile* f = new QFile(_lastFile, this);
            if(!f->open(QIODevice::ReadOnly)){ publish("ftp.error", QJsonObject{{"where","open"},{"file",_lastFile}}); return; }
            _ftp->put(f, QFileInfo(_lastFile).fileName());
        }
    }
private slots:
    void onCmd(int, bool err){ if(err) publish("ftp.error", QJsonObject{{"where","command"}}); }
    void onDone(bool err){
        if(!err) publish("ftp.done", QJsonObject{{"op","put"},{"url",_lastUrl},{"file",_lastFile}});
    }
    static void publish(const std::string& t, const QJsonObject& p){
        Message mm; mm.header.type=t; mm.payload=p; MessageBus::instance().post(mm);
    }
};
#endif

#pragma once
#include <QWidget>
#include <QGroupBox>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QLineEdit>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPushButton>
#include <QLabel>
#include "ui_bus.hpp"

class ConfigFormWidget : public QWidget {
    Q_OBJECT
public:
    explicit ConfigFormWidget(QWidget* parent=nullptr): QWidget(parent){
        auto* root = new QVBoxLayout(this);

        // —— 轨排参数 ——
        auto* gbTrack = new QGroupBox(QStringLiteral("轨排参数"));
        auto* gt = new QGridLayout(gbTrack);
        _trackCount = new QSpinBox; _trackCount->setRange(1, 64); _trackCount->setValue(20);
        _d1 = new QDoubleSpinBox; _d1->setRange(-10000, 10000); _d1->setDecimals(3); _d1->setValue(0.0);
        _d2 = new QDoubleSpinBox; _d2->setRange(-10000, 10000); _d2->setDecimals(3); _d2->setValue(0.0);
        _dd = new QDoubleSpinBox; _dd->setRange(0.001, 10000); _dd->setDecimals(3); _dd->setValue(1.0);

        add(gt,0,0,QStringLiteral("轨排数量"), _trackCount);
        add(gt,1,0,QStringLiteral("参数 d1"),  _d1);
        add(gt,1,2,QStringLiteral("参数 d2"),  _d2);
        add(gt,2,0,QStringLiteral("参数 dd"),  _dd);

        // —— FTP（可选项，供精调下载使用） ——
        auto* gbFtp = new QGroupBox(QStringLiteral("FTP 设置（可选）"));
        auto* gf = new QGridLayout(gbFtp);
        _ftpHost = new QLineEdit("192.168.1.100");
        _ftpPort = new QSpinBox; _ftpPort->setRange(1,65535); _ftpPort->setValue(21);
        _ftpUser = new QLineEdit("anonymous");
        _ftpPass = new QLineEdit; _ftpPass->setEchoMode(QLineEdit::Password);
        add(gf,0,0,QStringLiteral("主机"), _ftpHost);
        add(gf,0,2,QStringLiteral("端口"), _ftpPort);
        add(gf,1,0,QStringLiteral("用户名"), _ftpUser);
        add(gf,1,2,QStringLiteral("密码"),  _ftpPass);

        // —— 操作按钮 ——
        auto* btns = new QHBoxLayout();
        _btnReq  = new QPushButton(QStringLiteral("读取配置"));
        _btnSave = new QPushButton(QStringLiteral("保存配置"));
        btns->addStretch(); btns->addWidget(_btnReq); btns->addWidget(_btnSave);

        root->addWidget(gbTrack);
        root->addWidget(gbFtp);
        root->addLayout(btns);
        root->addStretch();

        connect(_btnReq,  &QPushButton::clicked, this, [this]{ send_ui_message("config.request", {}); });
        connect(_btnSave, &QPushButton::clicked, this, [this]{ send_ui_message("config.save", currentAsJson()); });
    }

public slots:
    // 任务侧把最新配置广播回来
    void applyConfig(const QJsonObject& o){
        if(o.contains("track_count")) _trackCount->setValue(o.value("track_count").toInt(_trackCount->value()));
        if(o.contains("d1")) _d1->setValue(o.value("d1").toDouble(_d1->value()));
        if(o.contains("d2")) _d2->setValue(o.value("d2").toDouble(_d2->value()));
        if(o.contains("dd")) _dd->setValue(o.value("dd").toDouble(_dd->value()));

        if(o.contains("ftp_host")) _ftpHost->setText(o.value("ftp_host").toString(_ftpHost->text()));
        if(o.contains("ftp_port")) _ftpPort->setValue(o.value("ftp_port").toInt(_ftpPort->value()));
        if(o.contains("ftp_user")) _ftpUser->setText(o.value("ftp_user").toString(_ftpUser->text()));
        if(o.contains("ftp_pass")) _ftpPass->setText(o.value("ftp_pass").toString(_ftpPass->text()));
    }

private:
    QJsonObject currentAsJson() const {
        return QJsonObject{
            {"track_count", _trackCount->value()},
            {"d1", _d1->value()},
            {"d2", _d2->value()},
            {"dd", _dd->value()},
            {"ftp_host", _ftpHost->text()},
            {"ftp_port", _ftpPort->value()},
            {"ftp_user", _ftpUser->text()},
            {"ftp_pass", _ftpPass->text()}
        };
    }
    template<typename W>
    void add(QGridLayout* g, int r, int c, const QString& name, W* w){
        g->addWidget(new QLabel(name), r, c); g->addWidget(w, r, c+1);
    }

private:
    // 轨排参数
    QSpinBox* _trackCount{};
    QDoubleSpinBox *_d1{}, *_d2{}, *_dd{};
    // FTP
    QLineEdit *_ftpHost{}, *_ftpUser{}, *_ftpPass{};
    QSpinBox* _ftpPort{};
    // 操作
    QPushButton *_btnReq{}, *_btnSave{};
};

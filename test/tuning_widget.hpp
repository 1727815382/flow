// tuning_widget.hpp
#pragma once
#include <QWidget>
#include <QPushButton>
#include <QLineEdit>
#include <QLabel>
#include <QFileDialog>
#include <QGridLayout>
#include <QGroupBox>
#include "ui_bus.hpp"

class TuningWidget : public QWidget {
    Q_OBJECT
public:
    explicit TuningWidget(QWidget* parent=nullptr): QWidget(parent){
        auto* root = new QVBoxLayout(this);

        auto* gbFile = new QGroupBox(QStringLiteral("数据文件"));
        auto* gf = new QGridLayout(gbFile);
        _path = new QLineEdit; _path->setPlaceholderText(QStringLiteral("*.asc / *.csv ..."));
        auto* choose = new QPushButton(QStringLiteral("选择文件"));
        auto* download = new QPushButton(QStringLiteral("FTP下载..."));
        auto* compute = new QPushButton(QStringLiteral("解析并计算TQI"));
        gf->addWidget(new QLabel(QStringLiteral("文件路径")),0,0);
        gf->addWidget(_path,0,1,1,3);
        gf->addWidget(choose,0,4);
        gf->addWidget(download,1,4);
        gf->addWidget(compute,1,3);
        root->addWidget(gbFile);

        auto* gbRes = new QGroupBox(QStringLiteral("TQI 汇总"));
        auto* gr = new QGridLayout(gbRes);
        addPair(gr,0,0,QStringLiteral("轨距偏差TQI"), _tqi_gj);
        addPair(gr,1,0,QStringLiteral("扭曲2.5TQI"), _tqi_nq);
        addPair(gr,2,0,QStringLiteral("超高偏差TQI"), _tqi_cg);
        addPair(gr,0,2,QStringLiteral("LTQI 轨向1"), _ltqi_gx);
        addPair(gr,1,2,QStringLiteral("LTQI 高低1"), _ltqi_gd);
        addPair(gr,2,2,QStringLiteral("RTQI 轨向1"), _rtqi_gx);
        addPair(gr,3,2,QStringLiteral("RTQI 高低1"), _rtqi_gd);
        root->addWidget(gbRes);
        root->addStretch();

        connect(choose, &QPushButton::clicked, this, [this]{
            auto f = QFileDialog::getOpenFileName(this, QStringLiteral("选择文件"), {}, QStringLiteral("文本 (*.asc *.csv);;所有文件 (*)"));
            if(!f.isEmpty()) _path->setText(f);
        });
        connect(download, &QPushButton::clicked, this, [this]{
            send_ui_message("ftp.open.dialog", {});  // 打开你们已有的FTP子窗体/或触发下载任务
        });
        connect(compute, &QPushButton::clicked, this, [this]{
            send_ui_message("tuning.parse_and_calc", QJsonObject{{"path", _path->text()}});
        });
    }

public slots:
    void applyTqrSummary(const QJsonObject& s){
        _tqi_gj->setText(s.value("TQI_GJPC").toVariant().toString());
        _tqi_nq->setText(s.value("TQI_NQ").toVariant().toString());
        _tqi_cg->setText(s.value("TQI_CGPC").toVariant().toString());
        _ltqi_gx->setText(s.value("LTQI_GXPC").toVariant().toString());
        _ltqi_gd->setText(s.value("LTQI_GDPC").toVariant().toString());
        _rtqi_gx->setText(s.value("RTQI_GXPC").toVariant().toString());
        _rtqi_gd->setText(s.value("RTQI_GDPC").toVariant().toString());
    }

private:
    void addPair(QGridLayout* gl, int r, int c, const QString& name, QLineEdit*& out){
        gl->addWidget(new QLabel(name), r, c);
        out = new QLineEdit; out->setReadOnly(true); out->setAlignment(Qt::AlignCenter);
        gl->addWidget(out, r, c+1);
    }

private:
    QLineEdit* _path{};
    QLineEdit *_tqi_gj{},*_tqi_nq{},*_tqi_cg{},*_ltqi_gx{},*_ltqi_gd{},*_rtqi_gx{},*_rtqi_gd{};
};

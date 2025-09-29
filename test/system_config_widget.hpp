#pragma once
#include <QWidget>
#include <QPushButton>
#include <QTextEdit>
#include <QVBoxLayout>

class SystemConfigWidget : public QWidget {
    Q_OBJECT
public:
    explicit SystemConfigWidget(QWidget* p=nullptr):QWidget(p){
        auto* lay=new QVBoxLayout(this);
        auto* btn=new QPushButton("刷新设备清单",this);
        _log = new QTextEdit(this); _log->setReadOnly(true);
        lay->addWidget(btn); lay->addWidget(_log);
        connect(btn,&QPushButton::clicked,this,[this]{ requestSummary(); });
    }
signals: void requestSummary();
public slots:
    void setSummary(const QString& s){ _log->setPlainText(s); }
private:
    QTextEdit* _log{};
};

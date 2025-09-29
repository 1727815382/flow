#ifndef FORM_H
#define FORM_H

#include <QWidget>

#include <QUrl>
#include "qftp/qftp.h"
#include "FtpClient.h"
#include <QFile>

#pragma execution_character_set("utf-8")

namespace Ui {
class TuingForm;
}

class TuingForm : public QWidget
{
    Q_OBJECT

public:
    static QString m_ftpFileName;
    explicit TuingForm(QWidget *parent = 0);
    ~TuingForm();

private slots:
    void on_pushButton_29_clicked();

    void on_pushButton_downLoad_clicked();
    void ftpCommandFinished(int, bool error);

private:
    void connectToFtp();
    void downloadFile();


private:
    Ui::TuingForm *ui;
    QString m_localDir;
    QString m_file;
    FtpClient* m_FtpClient;

};

#endif // FORM_H

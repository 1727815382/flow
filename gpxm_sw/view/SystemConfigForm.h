#ifndef SYSTEMCONFIGFORM_H
#define SYSTEMCONFIGFORM_H

#include <QWidget>
#include "aixsim.h"

namespace Ui {
class SystemConfigForm;
}

class SystemConfigForm : public QWidget
{
    Q_OBJECT

public:
    explicit SystemConfigForm(QWidget *parent = 0);
    ~SystemConfigForm();
signals:
    void signCtrlClicked(QString ,int , int );
private slots:
    void on_pBtnSetZero_clicked();

    void on_pBtnToZero_clicked();

    void slotMotorSettingStatus(MotorSettingStatus settingStatus);

    void slotShowInfo(int state,QString info);

private:
    Ui::SystemConfigForm *ui;
};

#endif // SYSTEMCONFIGFORM_H

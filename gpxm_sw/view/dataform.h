#ifndef DATAFORM_H
#define DATAFORM_H

#include <QWidget>

#include "aixsim.h"

namespace Ui {
class DataForm;
}

class DataForm : public QWidget
{
    Q_OBJECT

public:
    explicit DataForm(QWidget *parent = 0);
    ~DataForm();

    void keyPressEvent(QKeyEvent *event);

    void clearPlc ();

signals:

    //plc/轨排选择
    void signPlcSelected (int);
    //断面选择
    void signSectionSelected (int);
    //控制按钮信号， name= 控制按钮名字
    void signCtrlClicked (QString name, double value);

    //支腿压实
    void signFoldDoClicked(QString name, double value);

private slots:

    void slotPlcSelected (int index);
    void slotSectionSelected(int index);

    void slotUpdatePlc (PlcResponse& pr);
    void slotUpdateMotor (MotorResponse& mr);

    //轨排控制按钮按下
    void slotCtrlClicked ();
    //电机复位
    void slotResetMotor ();

    void slotCannotWorkMessage (MotorStatus status);

    void slotMotorFoldDoState (int& state);

    void slotMotorSettingStatus(MotorSettingStatus settingStatus);

    void slotShowInfo(int state,QString info);

private:

    QString getMotorsStatus(short value);
    QString getMotorsErrNo(short value);

private:
    Ui::DataForm *ui;
};

#endif // DATAFORM_H

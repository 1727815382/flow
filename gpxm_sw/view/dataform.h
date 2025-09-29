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

    //plc/����ѡ��
    void signPlcSelected (int);
    //����ѡ��
    void signSectionSelected (int);
    //���ư�ť�źţ� name= ���ư�ť����
    void signCtrlClicked (QString name, double value);

    //֧��ѹʵ
    void signFoldDoClicked(QString name, double value);

private slots:

    void slotPlcSelected (int index);
    void slotSectionSelected(int index);

    void slotUpdatePlc (PlcResponse& pr);
    void slotUpdateMotor (MotorResponse& mr);

    //���ſ��ư�ť����
    void slotCtrlClicked ();
    //�����λ
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

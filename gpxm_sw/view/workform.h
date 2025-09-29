#ifndef WORKFORM_H
#define WORKFORM_H

#include <QTimer>
#include <QWidget>
#include <QTableWidgetItem>
#include <QThread>

#include <device/TrackPanelDevice.h>
#include <device/TractorDevice.h>

#include "aixsim.h"

namespace Ui {
class WorkForm;
}

class WorkForm : public QWidget
{
    Q_OBJECT

public:
    explicit WorkForm(QWidget *parent = 0);
    ~WorkForm();

signals:

    void signRowSelected (int row);
    /**
     * @brief signMenuAddMesure �ֶ����
     */
    void signMenuAddMesure (TrackMesure&);
    void signMenuMdyMesure (TrackMesure&);
    void signMenuClrMesure ();
    void signMenuDelMesure ();
    //�����ź�
    void signDealSection (int type);//type��0-ȫ��������1-���Ҹ̣߳�2-���ߣ�3-��̣߳�4-�Ҹ߳�
    //ֹͣ�����ź�
    void signStopDealSection ();

    //�������
    void signSingleMesure (int direction);//direction��0-����1-����

    void signAutoMesure(int direction);//direction��0-����1-����
    //ֹͣ����
    void signStopMesure();

    // ���ݼ��
    void signDataCheck ();

public slots:

    //��ӽ��յ��Ĳ�������
    void slotMesureUpdate(TrackMesure data);
    void updataStatus(int index, short status);

    /**
     * @brief slotMesureUpdate ����ָ���е�����
     * @param row
     * @param data
     */
    void slotMesureUpdate (int row, TrackMesure& data);
    //ѡ�������ݣ�ˢ���ұ�
    void slotSelectedMesure (TrackMesure& me);

    void slotMesureRemoved (int row);
    void slotOldMesure (TrackMesure& me);
    void slotUpdateTractorDevStatus(bool status);
    void slotUpdateTrackPanelDevStatus(bool status);

    void slotShowErrInfo(QString info);

    void slotMotorRunOver();

private slots:

    void on_pushButton_CJ_add_clicked();

    void on_pushButton_CJ_del_clicked();

    void on_pushButton_CJ_modify_clicked();

    void on_pushButton_CJ_clean_clicked();

    //���� sw
    void on_pushButton_ctrl_deal_clicked(bool enable);

    //���
    void on_pushButton_check_clicked();
    //չʾ��һ������
    void on_pushButton_LastData_clicked(bool enable);

    //����
    void on_btnSingle_clicked(bool enable);

    //�Զ�
    void on_btnAuto_clicked(bool enable);

    //�����ļ�
    void on_pushButton_saveData_clicked();

    //��ȡ�ļ�
    void on_pushButton_readData_clicked();


private:
    Ui::WorkForm *ui;
    QTimer *_pTimer;

    TrackPanelDevice* m_TrackPanelDevice;
    TractorDevice* m_TractorDevice;
    QThread *m_TrackPanelDeviceThread;
    QThread *m_TractorDeviceThread;


    void CalAdjustValue(TrackMesure& data);
//    int modifyRow;
//    QList<TrackMesure> _wdata_last;
public:

//    QList<TrackMesure> _wdata;
};

#endif // WORKFORM_H

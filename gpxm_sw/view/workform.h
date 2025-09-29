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
     * @brief signMenuAddMesure 手动添加
     */
    void signMenuAddMesure (TrackMesure&);
    void signMenuMdyMesure (TrackMesure&);
    void signMenuClrMesure ();
    void signMenuDelMesure ();
    //精调信号
    void signDealSection (int type);//type：0-全部调整，1-左右高程，2-中线，3-左高程，4-右高程
    //停止精调信号
    void signStopDealSection ();

    //逐枕测量
    void signSingleMesure (int direction);//direction：0-正向，1-反向

    void signAutoMesure(int direction);//direction：0-正向，1-反向
    //停止测量
    void signStopMesure();

    // 数据检查
    void signDataCheck ();

public slots:

    //添加接收到的测量数据
    void slotMesureUpdate(TrackMesure data);
    void updataStatus(int index, short status);

    /**
     * @brief slotMesureUpdate 更新指定行的数据
     * @param row
     * @param data
     */
    void slotMesureUpdate (int row, TrackMesure& data);
    //选中了数据，刷到右边
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

    //精调 sw
    void on_pushButton_ctrl_deal_clicked(bool enable);

    //检查
    void on_pushButton_check_clicked();
    //展示上一次数据
    void on_pushButton_LastData_clicked(bool enable);

    //逐枕
    void on_btnSingle_clicked(bool enable);

    //自动
    void on_btnAuto_clicked(bool enable);

    //保存文件
    void on_pushButton_saveData_clicked();

    //读取文件
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

#ifndef DEVICEMANAGER_H
#define DEVICEMANAGER_H

#include <QString>
#include <QObject>
#include <QMap>
#include <QTimer>
#include <QFile>
#include <QScopedPointer>

#include "aixsim.h"
#include "network/modbussequencer.h"
#include "device/plcdevice.h"
#include "network/udpclient.h"
#include "levelingsession.h"

class DeviceManager : public QObject
{
    Q_OBJECT

public:
    DeviceManager();

public:

    void saveConfig(const QString &filePath);
    void loadConfig(const QString &filePath);

    void addPlc(PlcDevice* plc);
    void removePlc(int plcId);

    double calculateAvrHeight();
    void moveToTarget (PlcDevice* plc, QVector<MotorDevice*> motors);
    QString getMotorName(int index);

    void initDevCfg();

    MotorStatus checkMotorsCanWork(QVector<MotorDevice*>& motors);

    MotorSettingStatus checkMotorsAllSetting (MotorCtrlCMD cmd, QList<PlcDevice*>& motors);

    bool checkmotorsAllMoment (QList<PlcDevice*>& plcs);

    static void ensureCsvHeader(QFile &file, QTextStream &ts);

    static bool appendTrackMesureCsv(const QString &filePath, const TrackMesure &m,
                                     int precision = 6);

public slots:

    void slotUpdatePlcSelected (int index); //轨排选中


    /**
     * @brief slotMenuDealTrack 精调指令
     */
    void slotDealSection(int type);//type：0-全部调整，1-左右高程，2-中线，3-左高程，4-右高程
    /**
     * @brief slotMenuDealTrack  精调单轨排指令
     */
    void slotDealSingleSection(int type,TrackMesure mesure);//type：0-全部调整，1-左右高程，2-中线，3-左高程，4-右高程

    /**
     * @brief slotUpdateDevice 轮训更新电机参数
     */
    void slotUpdateDevice ();

    /**
     * @brief slotTakeSettingStatus 轮训检查收腿和参考点设置状态
     */
    void slotTakeSettingStatus (int plcIndex, int cmd);

//    void slotTakeSettingEach(int plcIndex, int cmd);
    //检查设置时间，超过次数报超时
    void slotTaskSettingTime (int cmd);

    /**
     * @brief slotAutoMesure 停止精调
     */
    void slotStopDealSection();

public slots:

    //更新接收到的测量数据
    void slotMesureUpdate(TrackMesure data);

    void slotRowSelected (int row); //work工作表选中
    /**
     * @brief slotMenuAddMesure 手动添加数据
     * @param data
     */
    void slotMenuAddMesure (TrackMesure& data);
    /**
     * @brief slotMenuMdyMesure 修改数据
     * @param data
     */
    void slotMenuMdyMesure (TrackMesure& data);

    /**
     * @brief slotMenuDelMesure 删除
     */
    void slotMenuDelMesure ();

    /**
     * @brief slotMenuClrMesure 清空数据
     */
    void slotMenuClrMesure ();

    /**
     * @brief slotSingleMesure 逐枕测量
     */
    void slotSingleMesure(int direction);

    /**
     * @brief slotAutoMesure 自动测量
     */
    void slotAutoMesure (int direction);

    /**
     * @brief slotAutoMesure 停止测量
     */
    void slotStopMesure ();

        //一组电机控制(需要多次发送加入间隔以及重发)
    void slotCtrlGroup ();

    /**
     * @brief slotSectionSelected 断面选择
     * @param index
     */
    void slotSectionSelected (int index);
    //控制断面的电机
    void slotCtrlSection(int section, PlcDevice* device);

    /**
     * @brief slotCtrlClicked 通过控制按钮控制断面电机
     * @param name
     */
    void slotCtrlClicked (QString name, double value);

    /**
     * @brief slotCtrlClicked 通过控制按钮设置电机支腿收腿位置
     * @param name
     */
    void slotCtrlClicked (QString name,int plcIndex, int sectionIndex);

    /**
     * @brief slotFoldDoClicked 执行支腿压实动作
     * @param name
     * @param value
     */
    void slotFoldDoClicked (QString name, double value);

    void slotDataCheck ();

public slots:

    //udp 小车数据处理
    void slotUdpRecvd (QByteArray data);

signals:

    void signMotorRunOver();//电机精调完成标志
    void signPlcAdded(PlcDevice&);
    void signPlcRemoved(int id);

    void signPlcUpdate (PlcResponse&);
    void signMotorUpdate (MotorResponse&);

    void signShowInfo (QString info);
    void signShowInfoToSys (int state, QString info);

    void signShowInfoToData (int state, QString info);

    /**
     * @brief signSelectedMesure 选中测量数据
     */
    void signSelectedMesure(TrackMesure&);
    /**
     * @brief signOldMesure 更新旧数据
     */
    void signOldMesure (TrackMesure&);
    /**
     * @brief signMesureDeleted 通知界面移除指定数据
     * @param row
     */
    void signMesureRemoved (int row);

    /**
     * @brief signMesureUpdate 通知界面更新界面
     * @param row
     */
    void signMesureUpdated (int row, TrackMesure& data);

    /**
     * @brief signMesureUpdate 添加心数据
     */
    void signMesureUpdate (TrackMesure);

    /**
     * @brief signCannotWorkMessage
     * @param state
     */
    void signCannotWorkMessage (MotorStatus state);

    /**
     * @brief signMotorSettingStatus 设置状态信息回复
     * @param setting
     */
    void signMotorSettingStatus (MotorSettingStatus setting);

    /**
     * @brief signMotorFoldDo 支腿压实状态回复
     */
    void signMotorFoldDoState (int&);

private:

    QMap<int,PlcDevice*> _plcDevices; //plcId plc设备映射
    QTimer _updateTimer; //点击刷新s
    int _crtSelectIndex; //当前选中的轨排
    int _crtSelectSection; // 当前选中的断面
    int _crtMesureSelectIndex; //当前选中的测量数据

    QList<TrackMesure> _mesures; //测量数据
    QList<TrackMesure> _oldMesures; //历史测量数据  

    QMap<int,ModbusSequencer* >_sequencers;

    UdpClient* _udp;

    QList<unsigned short> _errMotor;//

    bool _runMotor;//记录异常电机序号
    int _plcCount;

    QTimer _settingTimer; //设置定时器轮训，检查是否设置到位
//    QHash<quint8, AssembleBuf> _bufTable;

    QScopedPointer<LevelingSession> m_session;

    QVector<TrackLevelingSolver::Actuator> m_acts;
    TrackLevelingSolver::Params m_params;

    double m_tolForStop = 0.1;
    double m_firstScale = 1.0;
    int m_lastRowCount = 1; //当前run行数

    QList <PlcDevice*> _zeroDeviceList;
    QList <PlcDevice*> _foldSetDeviceList;
//    QList <PlcDevice*> _

    QVector<TrackLevelingSolver::Actuator> buildActsFromMeasurement(const QVector<TrackLevelingSolver::Measure>& rows);
    void ensureSession(const QVector<TrackLevelingSolver::Measure>& firstMeas);
    void resetSession();
};

#endif // DEVICEMANAGER_H

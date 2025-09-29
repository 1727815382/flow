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

    void slotUpdatePlcSelected (int index); //����ѡ��


    /**
     * @brief slotMenuDealTrack ����ָ��
     */
    void slotDealSection(int type);//type��0-ȫ��������1-���Ҹ̣߳�2-���ߣ�3-��̣߳�4-�Ҹ߳�
    /**
     * @brief slotMenuDealTrack  ����������ָ��
     */
    void slotDealSingleSection(int type,TrackMesure mesure);//type��0-ȫ��������1-���Ҹ̣߳�2-���ߣ�3-��̣߳�4-�Ҹ߳�

    /**
     * @brief slotUpdateDevice ��ѵ���µ������
     */
    void slotUpdateDevice ();

    /**
     * @brief slotTakeSettingStatus ��ѵ������ȺͲο�������״̬
     */
    void slotTakeSettingStatus (int plcIndex, int cmd);

//    void slotTakeSettingEach(int plcIndex, int cmd);
    //�������ʱ�䣬������������ʱ
    void slotTaskSettingTime (int cmd);

    /**
     * @brief slotAutoMesure ֹͣ����
     */
    void slotStopDealSection();

public slots:

    //���½��յ��Ĳ�������
    void slotMesureUpdate(TrackMesure data);

    void slotRowSelected (int row); //work������ѡ��
    /**
     * @brief slotMenuAddMesure �ֶ��������
     * @param data
     */
    void slotMenuAddMesure (TrackMesure& data);
    /**
     * @brief slotMenuMdyMesure �޸�����
     * @param data
     */
    void slotMenuMdyMesure (TrackMesure& data);

    /**
     * @brief slotMenuDelMesure ɾ��
     */
    void slotMenuDelMesure ();

    /**
     * @brief slotMenuClrMesure �������
     */
    void slotMenuClrMesure ();

    /**
     * @brief slotSingleMesure �������
     */
    void slotSingleMesure(int direction);

    /**
     * @brief slotAutoMesure �Զ�����
     */
    void slotAutoMesure (int direction);

    /**
     * @brief slotAutoMesure ֹͣ����
     */
    void slotStopMesure ();

        //һ��������(��Ҫ��η��ͼ������Լ��ط�)
    void slotCtrlGroup ();

    /**
     * @brief slotSectionSelected ����ѡ��
     * @param index
     */
    void slotSectionSelected (int index);
    //���ƶ���ĵ��
    void slotCtrlSection(int section, PlcDevice* device);

    /**
     * @brief slotCtrlClicked ͨ�����ư�ť���ƶ�����
     * @param name
     */
    void slotCtrlClicked (QString name, double value);

    /**
     * @brief slotCtrlClicked ͨ�����ư�ť���õ��֧������λ��
     * @param name
     */
    void slotCtrlClicked (QString name,int plcIndex, int sectionIndex);

    /**
     * @brief slotFoldDoClicked ִ��֧��ѹʵ����
     * @param name
     * @param value
     */
    void slotFoldDoClicked (QString name, double value);

    void slotDataCheck ();

public slots:

    //udp С�����ݴ���
    void slotUdpRecvd (QByteArray data);

signals:

    void signMotorRunOver();//���������ɱ�־
    void signPlcAdded(PlcDevice&);
    void signPlcRemoved(int id);

    void signPlcUpdate (PlcResponse&);
    void signMotorUpdate (MotorResponse&);

    void signShowInfo (QString info);
    void signShowInfoToSys (int state, QString info);

    void signShowInfoToData (int state, QString info);

    /**
     * @brief signSelectedMesure ѡ�в�������
     */
    void signSelectedMesure(TrackMesure&);
    /**
     * @brief signOldMesure ���¾�����
     */
    void signOldMesure (TrackMesure&);
    /**
     * @brief signMesureDeleted ֪ͨ�����Ƴ�ָ������
     * @param row
     */
    void signMesureRemoved (int row);

    /**
     * @brief signMesureUpdate ֪ͨ������½���
     * @param row
     */
    void signMesureUpdated (int row, TrackMesure& data);

    /**
     * @brief signMesureUpdate ���������
     */
    void signMesureUpdate (TrackMesure);

    /**
     * @brief signCannotWorkMessage
     * @param state
     */
    void signCannotWorkMessage (MotorStatus state);

    /**
     * @brief signMotorSettingStatus ����״̬��Ϣ�ظ�
     * @param setting
     */
    void signMotorSettingStatus (MotorSettingStatus setting);

    /**
     * @brief signMotorFoldDo ֧��ѹʵ״̬�ظ�
     */
    void signMotorFoldDoState (int&);

private:

    QMap<int,PlcDevice*> _plcDevices; //plcId plc�豸ӳ��
    QTimer _updateTimer; //���ˢ��s
    int _crtSelectIndex; //��ǰѡ�еĹ���
    int _crtSelectSection; // ��ǰѡ�еĶ���
    int _crtMesureSelectIndex; //��ǰѡ�еĲ�������

    QList<TrackMesure> _mesures; //��������
    QList<TrackMesure> _oldMesures; //��ʷ��������  

    QMap<int,ModbusSequencer* >_sequencers;

    UdpClient* _udp;

    QList<unsigned short> _errMotor;//

    bool _runMotor;//��¼�쳣������
    int _plcCount;

    QTimer _settingTimer; //���ö�ʱ����ѵ������Ƿ����õ�λ
//    QHash<quint8, AssembleBuf> _bufTable;

    QScopedPointer<LevelingSession> m_session;

    QVector<TrackLevelingSolver::Actuator> m_acts;
    TrackLevelingSolver::Params m_params;

    double m_tolForStop = 0.1;
    double m_firstScale = 1.0;
    int m_lastRowCount = 1; //��ǰrun����

    QList <PlcDevice*> _zeroDeviceList;
    QList <PlcDevice*> _foldSetDeviceList;
//    QList <PlcDevice*> _

    QVector<TrackLevelingSolver::Actuator> buildActsFromMeasurement(const QVector<TrackLevelingSolver::Measure>& rows);
    void ensureSession(const QVector<TrackLevelingSolver::Measure>& firstMeas);
    void resetSession();
};

#endif // DEVICEMANAGER_H

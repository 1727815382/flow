#include "devicemanager.h"
#include "device/plcdevice.h"

#include <QSettings>
#include <QMutex>
#include <QDebug>
#include <QMutexLocker>
#include <QThread>
#include <QDir>
#include <QTextCodec>

#include "network/modbushelper.h"
#include "tracklevelingsolver.h"
#include "levelingsession.h"

// ������ ��ӡÿ����õĵ���ֵ����ÿ����̾ۺϣ���/��/�ң� ������
// ������ �� Qt Creator �� Application Output �������ӡ��� ������
static void printCommands(const QVector<TrackLevelingSolver::Measure>& rows,
                          const QVector<double>& cmd,
                          const char* title)
{
    const int expected = rows.size() * 3;
    if (cmd.size() != expected) {
        qWarning().noquote() << QString("[WARN] commands.size(%1) != rows.size()*3(%2)")
                                .arg(cmd.size()).arg(expected);
        return;
    }

    qDebug().noquote() << QString("\n===== %1 =====").arg(QString::fromUtf8(title));
    qDebug().noquote() << "Idx   s(m)        Left(mm)   Center(mm)   Right(mm)";
    qDebug().noquote() << "----  ----------  ---------  -----------  ----------";

    for (int i = 0; i < rows.size(); ++i) {
        const double s = rows[i].s;
        const double l = cmd[3*i + 0];
        const double c = cmd[3*i + 1];
        const double r = cmd[3*i + 2];

        QString line = QString("%1  %2  %3  %4  %5")
                .arg(i, 4)                      // Idx ���4
                .arg(s, 10, 'f', 2)             // s(m)  ����2λ
                .arg(l, 9,  'f', 3)             // Left  ����3λ
                .arg(c, 11, 'f', 3)             // Center
                .arg(r, 10, 'f', 3);            // Right

        qDebug().noquote() << line;
    }
}

static void applyCommandToMotors(const QVector<double>& cmd) {
    qDebug() << "[apply] commands =" << cmd.size();
    int show = qMin(6, cmd.size());
    QStringList firstFew;
    for (int i = 0; i < show; ++i) firstFew << QString::number(cmd[i], 'f', 3);
    qDebug() << "  first few:" << firstFew.join(", ");
}

static QVector<TrackLevelingSolver::Measure> acquireMeasurement_pass(QList<TrackMesure> srcs)
{
    QVector<TrackLevelingSolver::Measure> v;

    foreach (TrackMesure src, srcs)
    {
        TrackLevelingSolver::Measure m;
        m.s = src._mileage;
        m.L = src._lRailElevationn;
        m.C = src._cLinePlann;
        m.R = src._rRailElevationn;
        m.Esup = src._supDeviationn;
        m.G = src._guiDeviationn;
        v.append(m);
    }

    return v;
}

// ==== Demo: ��һ�β��������滻����Ĵ����������� ====
// ע�⣺Measure �ֶ�˳��Ϊ��s, L, C, R, Esup, G
static QVector<TrackLevelingSolver::Measure> acquireMeasurement_demo_pass0() {
    QVector<TrackLevelingSolver::Measure> v;

    TrackLevelingSolver::Measure m;
    m.s = 64429.68; m.L = 23.7;  m.C = -172.2; m.R = 11.5;  m.Esup = 0.04; m.G = 0.0;
    v.append(m);
    m.s = 64431.56; m.L = 16.1;  m.C = -5.5; m.R = 9.69;  m.Esup = 0.05; m.G = 0.0;
    v.append(m);
    m.s = 64432.81; m.L = 12.9;  m.C = -143.4; m.R = 8.10;  m.Esup = 0.05; m.G = 0.0;
    v.append(m);
    m.s = 64434.68; m.L = 9.6;   m.C = -125.1; m.R = 7.30;  m.Esup = 0.05; m.G = 0.0;
    v.append(m);
    m.s = 64435.91; m.L = 8.19;  m.C = -111.5; m.R = 7.00;  m.Esup = 0.05; m.G = 0.0;
    v.append(m);
    m.s = 64437.17; m.L = 6.60;  m.C = -98.5;  m.R = 6.10;  m.Esup = 0.05; m.G = 0.0;
    v.append(m);

    return v;
}

// ==== Demo: �ڶ��β�����ִ�е�һ��������ٲ⣩ ====
static QVector<TrackLevelingSolver::Measure> acquireMeasurement_demo_pass1() {
    QVector<TrackLevelingSolver::Measure> v;

    TrackLevelingSolver::Measure m;
    m.s = 64429.74; m.L = 0.7;   m.C = -176.2; m.R = 0.2;   m.Esup = 0.05; m.G = 0.0;
    v.append(m);
    m.s = 64431.56; m.L = -0.8;  m.C = -8.69; m.R = 0.1;  m.Esup = 0.06; m.G = 0.0;
    v.append(m);
    m.s = 64432.81; m.L = -0.8;  m.C = -146.0; m.R = -0.7;  m.Esup = 0.06; m.G = 0.0;
    v.append(m);
    m.s = 64434.68; m.L = 0.1;   m.C = -126.7; m.R = 0.3;   m.Esup = 0.06; m.G = 0.0;
    v.append(m);
    m.s = 64435.91; m.L = 0.7;   m.C = -112.7; m.R = 1.0;   m.Esup = 0.06; m.G = 0.0;
    v.append(m);
    m.s = 64437.17; m.L = 0.6;   m.C = -99.5;  m.R = 1.0;   m.Esup = 0.06; m.G = 0.0;
    v.append(m);

    return v;
}

// ==== Demo: �����β�����ִ�еڶ���������ٲ⣩ ====
static QVector<TrackLevelingSolver::Measure> acquireMeasurement_demo_pass2()
{
    QVector<TrackLevelingSolver::Measure> v;

    TrackLevelingSolver::Measure m;
    m.s = 64429.74; m.L = 0.8;   m.C = -176.3; m.R = 0.3;   m.Esup = 0.05; m.G = 0.0;
    v.append(m);
    m.s = 64431.56; m.L = -0.3;  m.C = -8.5; m.R = 0.0;   m.Esup = 0.06; m.G = 0.0;
    v.append(m);
    m.s = 64432.81; m.L = -0.4;  m.C = -145.8; m.R = -0.5;  m.Esup = 0.05; m.G = 0.0;
    v.append(m);
    m.s = 64434.68; m.L = 0.2;   m.C = -126.6; m.R = 0.2;   m.Esup = 0.06; m.G = 0.0;
    v.append(m);
    m.s = 64435.91; m.L = 0.6;   m.C = -112.6; m.R = 0.7;   m.Esup = 0.06; m.G = 0.0;
    v.append(m);
    m.s = 64437.17; m.L = 0.5;   m.C = -99.4;  m.R = 0.8;   m.Esup = 0.06; m.G = 0.0;
    v.append(m);

    return v;
}

// ���ݲ�������ִ������ÿ�� 3 ��������������ˮƽ����������ִ������� = �����
static QVector<TrackLevelingSolver::Actuator>
buildActsFromMeasurement(const QVector<TrackLevelingSolver::Measure>& y) {
    QVector<TrackLevelingSolver::Actuator> acts;
    acts.reserve(y.size() * 3);

    for (int i = 0; i < y.size(); ++i) {
        TrackLevelingSolver::Actuator a;

        a.type = TrackLevelingSolver::ActType::VertL;
        a.s    = y[i].s;
        a.hLeak = 0.0; a.a = 1.0; a.b = 0.0; a.id = -1;
        acts.append(a);

        a.type = TrackLevelingSolver::ActType::HorizCenter;
        a.s    = y[i].s;
        a.hLeak = 0.0; a.a = 1.0; a.b = 0.0; a.id = -1;
        acts.append(a);

        a.type = TrackLevelingSolver::ActType::VertR;
        a.s    = y[i].s;
        a.hLeak = 0.0; a.a = 1.0; a.b = 0.0; a.id = -1;
        acts.append(a);
    }
    return acts;
}


DeviceManager::DeviceManager():
    _crtMesureSelectIndex(-1)
{
    _udp = new UdpClient();

    QThread* thread= new QThread();
    _udp->moveToThread(thread);
    thread->start();

    connect (_udp, SIGNAL(signUdpRecvd(QByteArray)), this, SLOT(slotUdpRecvd(QByteArray)));

    _plcCount = 10;
}


void DeviceManager::initDevCfg()
{
#ifdef ON_PAD
    loadConfig("deviceConfig.ini");
#else
    loadConfig("E:\\WGZ\\Pad\\gpxm_sw\\deviceConfig.ini");
#endif
    connect (&_updateTimer, SIGNAL(timeout()), this, SLOT(slotUpdateDevice()));

    _updateTimer.start(300);
    _crtSelectIndex = 0;
    _crtSelectSection = 1; //Ĭ��ѡ�ж���
    _runMotor = false;
}
void DeviceManager::addPlc(PlcDevice* plc)
{
    //    QMutexLocker locker(&mutex);

    if (_plcDevices.contains(plc->id()))
    {
        qWarning() << "PLC ID" << plc->id() << "already exists";
        return;
    }

    _plcDevices.insert(plc->id(), plc);

    ModbusSequencer* sequencer = new ModbusSequencer(plc->client());
    _sequencers.insert(plc->id(), sequencer);

    emit signPlcAdded(*plc);

    qDebug() << "Added PLC:" << plc->name() << "ID:" << plc->id();
}

void DeviceManager::removePlc(int plcId)
{
    //    QMutexLocker locker(&mutex);

    if (!_plcDevices.contains(plcId))
    {
        qWarning() << "PLC ID" << plcId << "does not exist";
        return;
    }

    _plcDevices.remove(plcId);

    _sequencers.remove(plcId);
    emit signPlcRemoved(plcId);

    qDebug() << "Removed PLC ID:" << plcId;
}

double DeviceManager::calculateAvrHeight()
{
    double avrHeight = 0;
    double totalHeight = 0;
    int num = 0;
    foreach (PlcDevice* plc, _plcDevices)
    {
        if (plc->isConnect())
        {
            num ++;
            totalHeight+=plc->totalHeight ();
        }
    }

    avrHeight = totalHeight / (double)num;

    return avrHeight;
}

// �����ļ�����
void DeviceManager::saveConfig(const QString &filePath)
{
    QSettings settings(filePath, QSettings::IniFormat);

    // ����PLC�豸
    settings.beginWriteArray("PLCDevices");
    int index = 0;
    PlcDevice* device = NULL;
    for (PlcDevice* plc : _plcDevices)
    {
        device = plc;
        settings.setArrayIndex(index++);
        settings.setValue("id", plc->id());
        settings.setValue("name", plc->name());
        settings.setValue("ip", plc->ip());
        settings.setValue("port", plc->port());

        //        // ��������б�
        //        QStringList sections;
        //        for (int sec : plc->sections())
        //        {
        //            sections << QString::number(sec);
        //        }
        //        settings.setValue("sections", sections.join(","));
    }

    settings.endArray();

    // �������-���ӳ��
    if (device == NULL)
        return;

    QMap<int, QVector<int>> sectionToMotors = device->sectionMap();
    settings.beginWriteArray("SectionMotorMappings");
    index = 0;
    for (auto it = sectionToMotors.constBegin(); it != sectionToMotors.constEnd(); ++it)
    {
        settings.setArrayIndex(index++);
        settings.setValue("section", it.key());

        QStringList motors;
        for (int motor : it.value())
        {
            motors << QString::number(motor);
        }

        settings.setValue("motors", motors.join(","));
    }

    settings.endArray();

    qDebug() << "Configuration saved to" << filePath;
}

void DeviceManager::loadConfig(const QString &filePath)
{
    //    QMutexLocker locker(&mutex);

    QSettings settings(filePath, QSettings::IniFormat);


    // ����PLC�豸
    settings.beginReadArray("PLCDevices");

    int standValue = settings.value("standValue").toInt();
    int maxMixtake = settings.value("maxMixtake").toInt();
    int plcCount = settings.value("size").toInt();

    qDebug()<<"plcCount"<<plcCount;
    if(plcCount>30)
        _plcCount = 30;
    else if(plcCount <3)
        _plcCount = 10;
    else
    {
        _plcCount =  plcCount;
    }
    for (int i = 0; i < _plcCount; i++)
    {
        PlcDevice* plc = new PlcDevice(i);
        plc->setName(QString("plc%1").arg(i));
        plc->setId(i);
        plc->setPort(502);
        plc->setIp(QString("192.168.1.%1").arg(10+i));
        this->addPlc(plc);
        //        ModbusSequencer* sequencer = new ModbusSequencer(plc->client());
        //        _sequencers.insert(i, sequencer);
    }
    for (int i = 0; i < plcCount; ++i)
    {
        settings.setArrayIndex(i);

        QString name = settings.value("name").toString();
        int section1 = settings.value("section1").toInt();
        int section2 = settings.value("section2").toInt();
        int section3 = settings.value("section3").toInt();
        int section4 = settings.value("section4").toInt();
        int IP = settings.value("ip").toInt();

        foreach (PlcDevice* device, _plcDevices)
        {
            if(device->name() == name)
            {
                device->appendSectionID(section1,1);
                device->appendSectionID(section2,2);
                device->appendSectionID(section3,3);
                device->appendSectionID(section4,4);
                device->setIp(QString("192.168.1.%1").arg(IP));
                device->init();
            }
        }
    }
    settings.endArray();

    qDebug() << "Configuration loaded from" << filePath << "with"
             << plcCount << "PLCs and";
}

void DeviceManager::slotUpdatePlcSelected(int index)
{
    _crtSelectIndex = index;
}

void DeviceManager::slotDealSingleSection(int type,TrackMesure mesure)
{
    int section = mesure._id;

    foreach (PlcDevice* device, _plcDevices)
    {
        if (device && !device->isConnect())
        {
            emit signShowInfo(QString::fromLocal8Bit("%1#���ž���ʧ��,PLC�豸�����쳣").arg(section));
            continue;
        }
        QMap<int, int> sectionMap = device->sectionIDMap();
        foreach (int plcSec, sectionMap.keys())
        {
            //Ѱ��section���ڵ�plc
            if (plcSec == section)
            {
                QVector<MotorDevice*> motors = device->motorsOnSection(sectionMap.value(section));
                QVector<MotorDevice*> selectedMotors;
                foreach (MotorDevice* motor, motors)
                {
                    //idx����-0��2��4��6����-1��3��5��7����-11��10��9��8
                    int idx = motor->response()._index;
                    if(idx % 2 == 0 && idx < 8)
                    {
                        motor->setTargetPosition(motor->responPosition()-(int)(mesure._lGuiAdjustmentn*100));
                        //type��0-ȫ��������1-���Ҹ̣߳�2-���ߣ�3-��̣߳�4-�Ҹ߳�
                        if(type == 0 || type == 1 || type == 3)
                        {
                            selectedMotors.append(motor);
                        }

                    }
                    else if(idx % 2 == 1 && idx < 8)
                    {
                        motor->setTargetPosition(motor->responPosition()-(int)(mesure._cLinedjustmentn*100));
                        //type��0-ȫ��������1-���Ҹ̣߳�2-���ߣ�3-��̣߳�4-�Ҹ߳�
                        if(type == 0 || type == 2)
                        {
                            selectedMotors.append(motor);
                        }
                    }
                    else if(idx >= 8)
                    {
                        motor->setTargetPosition(motor->responPosition()-(int)(mesure._rGuiAdjustmentn*100));
                        //type��0-ȫ��������1-���Ҹ̣߳�2-���ߣ�3-��̣߳�4-�Ҹ߳�
                        if(type == 0 || type == 1 || type == 4)
                        {
                            selectedMotors.append(motor);
                        }
                    }

                }
                foreach (MotorDevice* motor, selectedMotors)
                {
                    qDebug()<<"id="<<motor->response()._id<<"position"<<motor->responPosition()<<"new position"<<motor->targetPosition();

                }
                this->moveToTarget(device,selectedMotors);
                return;
            }
        }
    }
}

void DeviceManager::slotDealSection(int type)
{
    //����ĳһ���Ŷ���
    if (_crtMesureSelectIndex != -1)
    {
        TrackMesure mesure = _mesures.at(_crtMesureSelectIndex);
        slotDealSingleSection(type,mesure);
    }
    else//�������й���
    {
        QList<int> _DealMesuresIndex; //��Ҫ�����Ĳο���������
        if(_mesures.count() > 0)
        {
            qDebug() << "mesures.count()" << _mesures.count();
            for(int i = _mesures.count()-1;i>=0;i--)
            {
                if(!_DealMesuresIndex.contains(_mesures.at(i)._id))
                {
                    _DealMesuresIndex.append(_mesures.at(i)._id);
                    TrackMesure mesure = _mesures.at(i);
                    slotDealSingleSection(type,mesure);
                    qDebug() << "mesures_id=" << _mesures.at(i)._id<<" index = "<<i;
                }

            }
        }
    }
}

void DeviceManager::slotStopDealSection()
{

}

void DeviceManager::slotUpdateDevice ()
{
    QString str = "";
    bool changeflag = false;
    bool curRunMotor = false;
    for (PlcDevice* plc : _plcDevices)
    {
        plc->startPolling();

        PlcResponse res = plc->response();
        emit signPlcUpdate (res);

        QVector<MotorDevice*> motors = plc->motors();
        for (int i = 0; i < motors.size(); i++)
        {
            MotorResponse ms = motors[i]->response();
            if (plc->index() == _crtSelectIndex)
            {
                emit signMotorUpdate(ms);
            }

            unsigned short index = (plc->index()<<8) +i;

            if(ms._status > 0&& ms._status < 100)
            {
                _runMotor = true;
                curRunMotor = true;
            }


            if(_errMotor.contains(index))
            {
                if(ms._status < 100)
                {
                    _errMotor.removeOne(index);
                    changeflag = true;
                }
            }
            else
            {
                if(ms._status > 100)
                {
                    char temp[64];
                    sprintf(temp,"����%d",plc->index()+1);
                    str += QString::fromLocal8Bit(temp)+getMotorName(i)+QString::fromLocal8Bit("�쳣\n");
                    _errMotor.append(index);
                    changeflag = true;
                }
            }
        }
    }
    if(changeflag)
        emit signShowInfo(str);
    if(!curRunMotor)
    {
        if(_runMotor)
        {
            _runMotor = false;
            emit signMotorRunOver();
        }
    }
}

void DeviceManager::slotTakeSettingStatus(int plcIndex, int cmd)
{
    if(plcIndex == 20)
    {
        if (cmd == Motor_Ctrl_Folded_POS_Set)
        {
            static int oldSetting = -1;
            MotorSettingStatus setting = this->checkMotorsAllSetting(Motor_Ctrl_Folded_POS_Set, this->_plcDevices.values());

            if (oldSetting != setting)
            {
                oldSetting = setting;
                emit signMotorSettingStatus (setting);
                if (oldSetting!= Motor_Ctrl_IDEL)
                {
                    _settingTimer.stop();
                    oldSetting = -1;
                }
            }
            //                }

            else if (cmd == Motor_Ctrl_ZERO)
            {
                static int oldSetting = -1;
                MotorSettingStatus setting = this->checkMotorsAllSetting(Motor_Ctrl_ZERO, this->_plcDevices.values());

                if (oldSetting != setting)
                {
                    oldSetting = setting;
                    emit signMotorSettingStatus (setting);
                    if (oldSetting!= Motor_Ctrl_IDEL)
                    {
                        _settingTimer.stop();
                        oldSetting = -1;
                    }
                }
            }

            else if (cmd == Motor_Ctrl_FoldDo)
            {
                //��ȡ��ǰŤ��
                int oldRet = -1;

                bool ret = this->checkmotorsAllMoment (this->_plcDevices.values());
                if (oldRet != ret)
                {
                    oldRet = ret;
                    if (ret)
                    {
                        //ֹͣ
                        this->slotCtrlClicked("stop", 0);
                        //��ʾ��Ϣ
                        int state = 1;
                        emit signMotorFoldDoState (state);
                        _settingTimer.stop();
                        oldRet = -1;
                    }

                    //����
                    else
                    {
                        int state = 0;
                        emit signMotorFoldDoState (state);
                    }
                }
            }
        }

    }

    else
    {

        foreach (PlcDevice* plc, _plcDevices)
        {
            if (plc->index() == plcIndex)
            {

                if (plc && !plc->isConnect())
                {
                    emit signShowInfo(QString::fromLocal8Bit("%1#������λ����ʧ��,PLC�豸�����쳣").arg(_plcDevices.key(plc)));
                    continue;
                }

                if (cmd == Motor_Ctrl_Folded_POS_Set)
                {
                    static int oldSetting = -1;
                    MotorSettingStatus setting = this->checkMotorsAllSetting(Motor_Ctrl_Folded_POS_Set, QList<PlcDevice*>{plc});

                    if (oldSetting != setting)
                    {
                        oldSetting = setting;
                        emit signMotorSettingStatus (setting);
                        if (setting!= Motor_Ctrl_IDEL)
                        {
                            _settingTimer.stop();
                            oldSetting = -1;
                        }
                    }
                }

                else if (cmd == Motor_Ctrl_IDEL)
                {
                    static int oldSetting = -1;
                    MotorSettingStatus setting = this->checkMotorsAllSetting(Motor_Ctrl_ZERO, QList<PlcDevice*>{plc});

                    if (oldSetting != setting)
                    {
                        oldSetting = setting;
                        emit signMotorSettingStatus (setting);
                        if (oldSetting!= Motor_Ctrl_IDEL)
                        {
                            _settingTimer.stop();
                            oldSetting = -1;
                        }
                    }
                }

                else if (cmd == Motor_Ctrl_FoldDo)
                {
                    //��ȡ��ǰŤ��
                    int oldRet = -1;
                    bool ret = this->checkmotorsAllMoment (QList<PlcDevice*>{plc});
                    if (oldRet != ret)
                    {
                        oldRet = ret;
                        if (ret)
                        {
                            //ֹͣ
                            this->slotCtrlClicked("stop", 0);
                            //��ʾ��Ϣ
                            int state = 1;
                            emit signMotorFoldDoState (state);
                            _settingTimer.stop();
                            oldRet = -1;
                        }

                        //����
                        else
                        {
                            int state = 0;
                            emit signMotorFoldDoState (state);
                        }
                    }
                }

                break;
            }
        }
    }
}

#if 0
void DeviceManager::slotTakeSettingEach(int plcIndex, int cmd)
{
    QList<PlcDevice*>::iterator it;
    for (it = _foldSetDeviceList.begin(); it != _foldSetDeviceList.end(); it++)
    {
        if (cmd == Motor_Ctrl_Folded_POS_Set)
        {
            PlcDevice* plc = *it;
            QVector<MotorDevice*> motors = plc->motors();
            MotorSettingStatus setting = this->checkMotorsAllSetting(Motor_Ctrl_ZERO, QList<PlcDevice*> plc);
            if (setting == Motor_Setting_Idel)
            {
                emit signShowInfoToData(0,QString::fromLocal8Bit("%1#����������������λ��").arg(_plcDevices.key(plc)+1));
                it = _foldSetDeviceList.erase(it);
            }

            else if (setting == Motor_Setting_Fold_Success)
            {
                emit signShowInfoToData(0,QString::fromLocal8Bit("%1#������������λ�óɹ�").arg(_plcDevices.key(plc)+1));

            }

            else if (setting == Motor_Setting_Fold_Fail)
            {
                emit signShowInfoToData(0,QString::fromLocal8Bit("%1#������������λ��ʧ��").arg(_plcDevices.key(plc)+1));
            }


            else if (cmd == Motor_Ctrl_ZERO)
            {
                static int oldSetting = -1;
                MotorSettingStatus setting = this->checkMotorsAllSetting(Motor_Ctrl_ZERO, this->_plcDevices.values());

                if (oldSetting != setting)
                {
                    oldSetting = setting;
                    emit signMotorSettingStatus (setting);
                    if (oldSetting!= Motor_Ctrl_IDEL)
                    {
                        _settingTimer.stop();
                        oldSetting = -1;
                    }
                }
            }

            else if (cmd == Motor_Ctrl_FoldDo)
            {
                //��ȡ��ǰŤ��
                int oldRet = -1;

                bool ret = this->checkmotorsAllMoment (this->_plcDevices.values());
                if (oldRet != ret)
                {
                    oldRet = ret;
                    if (ret)
                    {
                        //ֹͣ
                        this->slotCtrlClicked("stop", 0);
                        //��ʾ��Ϣ
                        int state = 1;
                        emit signMotorFoldDoState (state);
                        _settingTimer.stop();
                        oldRet = -1;
                    }

                    //����
                    else
                    {
                        int state = 0;
                        emit signMotorFoldDoState (state);
                    }
                }
            }
        }

    }

    else
    {

        foreach (PlcDevice* plc, _plcDevices)
        {
            if (plc->index() == plcIndex)
            {

                if (plc && !plc->isConnect())
                {
                    emit signShowInfo(QString::fromLocal8Bit("%1#������λ����ʧ��,PLC�豸�����쳣").arg(_plcDevices.key(plc)));
                    continue;
                }

                if (cmd == Motor_Ctrl_Folded_POS_Set)
                {
                    static int oldSetting = -1;
                    MotorSettingStatus setting = this->checkMotorsAllSetting(Motor_Ctrl_Folded_POS_Set, QList<PlcDevice*>{plc});

                    if (oldSetting != setting)
                    {
                        oldSetting = setting;
                        emit signMotorSettingStatus (setting);
                        if (setting!= Motor_Ctrl_IDEL)
                        {
                            _settingTimer.stop();
                            oldSetting = -1;
                        }
                    }
                }

                else if (cmd == Motor_Ctrl_IDEL)
                {
                    static int oldSetting = -1;
                    MotorSettingStatus setting = this->checkMotorsAllSetting(Motor_Ctrl_ZERO, QList<PlcDevice*>{plc});

                    if (oldSetting != setting)
                    {
                        oldSetting = setting;
                        emit signMotorSettingStatus (setting);
                        if (oldSetting!= Motor_Ctrl_IDEL)
                        {
                            _settingTimer.stop();
                            oldSetting = -1;
                        }
                    }
                }

                else if (cmd == Motor_Ctrl_FoldDo)
                {
                    //��ȡ��ǰŤ��
                    int oldRet = -1;
                    bool ret = this->checkmotorsAllMoment (QList<PlcDevice*>{plc});
                    if (oldRet != ret)
                    {
                        oldRet = ret;
                        if (ret)
                        {
                            //ֹͣ
                            this->slotCtrlClicked("stop", 0);
                            //��ʾ��Ϣ
                            int state = 1;
                            emit signMotorFoldDoState (state);
                            _settingTimer.stop();
                            oldRet = -1;
                        }

                        //����
                        else
                        {
                            int state = 0;
                            emit signMotorFoldDoState (state);
                        }
                    }
                }

                break;
            }
        }
    }
}
#endif
void DeviceManager::slotTaskSettingTime(int cmd)
{
    //    static int timeNum = 0;

    //    timeNum ++;
    //    if (timeNum > 20)
    //    {
    //        _settingTimer.stop();
    //        //��ʱ����
    //        timeNum = 0;
    //    }
}

void DeviceManager::slotRowSelected(int row)
{
    this->_crtMesureSelectIndex = row;
    if (_crtMesureSelectIndex != -1)
    {
        if (_mesures.size() > row)
        {
            TrackMesure mesure = _mesures.at(row);

            emit signSelectedMesure(mesure);
        }
    }
}

void DeviceManager::slotMesureUpdate(TrackMesure data)
{
    //
    _mesures.append(data);
}

void DeviceManager::slotMenuAddMesure(TrackMesure &data)
{
    data._serialNumber = _mesures.size();
    _mesures.append(data);
    //    _crtMesureSelectIndex = _mesures.size() -1;

}

void DeviceManager::slotMenuMdyMesure(TrackMesure &data)
{
    if (_crtMesureSelectIndex != -1)
    {
        _mesures.replace(_crtMesureSelectIndex, data);
        emit signMesureUpdated (_crtMesureSelectIndex, data);
    }
}

void DeviceManager::slotMenuDelMesure()
{
    if (_crtMesureSelectIndex != -1)
    {
        _mesures.removeAt(_crtMesureSelectIndex);
        emit signMesureRemoved (_crtMesureSelectIndex);
        _crtMesureSelectIndex = -1;
    }
}

void DeviceManager::slotMenuClrMesure()
{
    _oldMesures = _mesures;
    _mesures.clear();
    _crtMesureSelectIndex = -1;

    foreach (TrackMesure me, _oldMesures)
    {
        emit signOldMesure (me);
    }
}

void DeviceManager::slotSingleMesure(int direction)
{
    //�������
    CarCtrl car;
    memset(&car, 0,sizeof(CarCtrl));
    car._head1 = 0x28;
    car._id = 5;
    car._cmd = 2;
    car._fwd = direction+1;
    car._count = 60;

    if (_udp)
    {
        _udp->send((char*)&car, sizeof(CarCtrl));
    }
}

void DeviceManager::slotAutoMesure(int direction)
{
    //�Զ�����
    CarCtrl car;
    memset(&car, 0,sizeof(CarCtrl));
    car._head1 = 0x28;
    car._id = 5;
    car._cmd = 1;
    car._fwd = direction+1;
    car._count = 30;

    if (_udp)
    {
        _udp->send((char*)&car, sizeof(CarCtrl));
    }
}

void DeviceManager::slotStopMesure()
{
    //ֹͣ����
    CarCtrl car;
    memset(&car, 0,sizeof(CarCtrl));
    car._head1 = 0x28;
    car._id = 5;
    car._cmd = 0;
    car._fwd = 0;
    car._count = 0;

    if (_udp)
    {
        _udp->send((char*)&car, sizeof(CarCtrl));
    }
}

void DeviceManager::slotCtrlGroup ()
{
    //    foreach (var, _mesures)
    //    {

    //    }
}

void DeviceManager::slotSectionSelected(int index)
{
    //section ��1��ʼ����
    this->_crtSelectSection = index +1;
}

void DeviceManager::slotCtrlSection(int section, PlcDevice *device)
{
    int avantHeight = this->calculateAvrHeight();
    QVector<MotorDevice*> motors = device->motorsOnSection(section);
    //    motors
    foreach (MotorDevice* motor, motors)
    {
        motor->setTargetPosition (avantHeight);
    }

    this->moveToTarget(device,motors);
}

MotorStatus DeviceManager::checkMotorsCanWork(QVector<MotorDevice*>& motors)
{
    for (int i = 0; i < motors.size(); i++)
    {
        if (motors[i]->response()._status != MOTOR_STATUS_IDEL)
        {
            return (MotorStatus)motors[i]->response()._status;
        }
    }

    return MOTOR_STATUS_IDEL;
}

MotorSettingStatus DeviceManager::checkMotorsAllSetting(MotorCtrlCMD cmd, QList<PlcDevice *> &plcs)
{
    //ֻҪ��һ�������򷵻ش��󣬳ɹ����붼�ɹ���idel ��ʾ����ִ�У�
    bool allSuccess = true;

    if (cmd == Motor_Ctrl_Folded_POS_Set)
    {
        for (int k = 0; k < plcs.size(); k++)
        {
            QVector<MotorDevice*> motors = plcs[k]->motors();

            for (int i = 0; i < motors.size(); i++)
            {
                MotorSettingStatus setting = motors[i]->settingStatus();
                if (setting == Motor_Setting_Fold_Fail)
                {
                    allSuccess = false;
                    return Motor_Setting_Fold_Fail;
                }

                else if (setting == Motor_Setting_Idel)
                {
                    allSuccess = false;
                    return Motor_Setting_Idel;
                }
            }
        }

        if (allSuccess)
            return Motor_Setting_Fold_Success;
        return Motor_Setting_Idel;
    }


    else if (cmd == Motor_Ctrl_ZERO)
    {
        for (int k = 0; k < plcs.size(); k++)
        {
            QVector<MotorDevice*> motors = plcs[k]->motors();
            for (int i = 0; i < motors.size(); i++)
            {
                MotorSettingStatus setting = motors[i]->settingStatus();
                if (setting == Motor_Setting_Zero_Fail)
                {
                    allSuccess = false;
                    return Motor_Setting_Zero_Fail;
                }

                else if (setting == Motor_Setting_Idel)
                {
                    allSuccess = false;
                    return Motor_Setting_Idel;
                }
            }
        }

        if (allSuccess)
            return Motor_Setting_Zero_Success;
        return Motor_Setting_Idel;
    }

    else
    {
        qDebug () << "error cmd";
        return Motor_Setting_Zero_Fail;
    }
}

bool DeviceManager::checkmotorsAllMoment(QList<PlcDevice *> &plcs)
{
    foreach (PlcDevice* plc, plcs)
    {
        QVector<MotorDevice*> motors = plc->motors();

        for (int i = 0; i < motors.size(); i++)
        {
            int moment = motors[i]->response()._moment;
            if (moment < 400)
                return false;
        }
    }

    return true;
}

void DeviceManager::slotCtrlClicked(QString name, double value)
{
    int motorIndex = -1;
    int potCha = value; //�˶���Ŀ����Ծ���
    if(name == QString("h1d"))
    {
        //����h1��Ӧ���Ƕ����е���С��ŵ��
        motorIndex = 0;
        potCha *= 1;
    }

    else if (name == QString("h1u"))
    {
        motorIndex = 0;
        potCha *= -1;
    }

    else if (name == QString("md"))
    {
        motorIndex = 1;
        potCha *= 1;
    }

    else if (name == QString("mu"))
    {
        motorIndex = 1;
        potCha *= -1;
    }

    else if (name == QString("h3d"))
    {
        motorIndex = 2;
        potCha *= 1;
    }

    else if (name == QString("h3u"))
    {
        motorIndex = 2;
        potCha *= -1;
    }

    else if (name == QString("zeroSet")) //��λ����
    {
        //plc���е������
        int code = Motor_Ctrl_IDEL;

        for (PlcDevice* plc : _plcDevices)
        {

            if(_crtSelectIndex == 20)
            {
                if (plc && !plc->isConnect())
                {
                    emit signShowInfoToData(0,QString::fromLocal8Bit("%1#������λ����ʧ��,PLC�豸�����쳣").arg(_plcDevices.key(plc)+1));
                    continue;
                }
                int key = _plcDevices.key(plc);
                QVector<MotorDevice*> motors = plc->motors();

                MotorStatus state = this->checkMotorsCanWork (motors);

                if (state != MOTOR_STATUS_IDEL)
                {
                    emit signCannotWorkMessage(state);
                    return;
                }

                for (int i = 0; i < motors.size(); i++)
                {
                    int position = motors[i]->responPosition();
                    motors[i]->setZeroPosition(position);

                    int idx = motors[i]->response()._index;
                    int motorId = motors[i]->response()._id;

                    int16_t posLow = (position & 0xFFFF);
                    int16_t posHigh = ((position >> 16)& 0xFFFF);

                    int16_t zero = 1;

                    QVector<quint16> values =
                    {
                        static_cast<quint16>(motorId),
                        0, // ����λ���ض�����
                        static_cast<quint16>(code),
                        0, // ����λ���ض�����
                        0, // ����λ���ض�����
                        0, // ����λ���ض�����
                        0, // ����λ���ض�����
                        static_cast<quint16>(zero), // ����λ���ض�����
                        static_cast<quint16>(posHigh),
                        static_cast<quint16>(posLow),

                    };

                    int startindex = 2020 + 20*idx;

                    // ʹ�ø�����������д�����Ĵ����������ֽ�����
                    QByteArray pduData = ModbusHelper::createWriteMultipleRegistersRequest(
                                startindex,
                                values
                                );

                    _sequencers[key]->enqueueRawRequest(1, pduData, QModbusPdu::WriteMultipleRegisters);
                }
            }
            else if (plc->index() == _crtSelectIndex)
            {
                if (plc && !plc->isConnect())
                {
                    emit signShowInfoToData(0,QString::fromLocal8Bit("%1#������λ����ʧ��,PLC�豸�����쳣").arg(_plcDevices.key(plc)+1));
                    continue;
                }

                int key = _plcDevices.key(plc);
                QVector<MotorDevice*> motors = plc->motors();

                MotorStatus state = this->checkMotorsCanWork (motors);

                if (state != MOTOR_STATUS_IDEL)
                {
                    emit signCannotWorkMessage(state);
                    return;
                }

                for (int i = 0; i < motors.size(); i++)
                {
                    int position = motors[i]->responPosition();
                    motors[i]->setZeroPosition(position);

                    int idx = motors[i]->response()._index;
                    int motorId = motors[i]->response()._id;

                    int16_t posLow = (position & 0xFFFF);
                    int16_t posHigh = ((position >> 16)& 0xFFFF);


                    QVector<quint16> values =
                    {
                        static_cast<quint16>(motorId),
                        0, // ����λ���ض�����
                        static_cast<quint16>(code),
                        0, // ����λ���ض�����
                        0, // ����λ���ض�����
                        0, // ����λ���ض�����
                        0, // ����λ���ض�����
                        1, // ����λ���ض�����
                        static_cast<quint16>(posHigh),
                        static_cast<quint16>(posLow),

                    };

                    int startindex = 2020 + 20*idx;

                    // ʹ�ø�����������д�����Ĵ����������ֽ�����
                    QByteArray pduData = ModbusHelper::createWriteMultipleRegistersRequest(
                                startindex,
                                values
                                );

                    _sequencers[key]->enqueueRawRequest(1, pduData, QModbusPdu::WriteMultipleRegisters);
                }
            }
        }

        static QMetaObject::Connection conn = connect(&_settingTimer, &QTimer::timeout, this,
                                                      [=]()
        {
            slotTakeSettingStatus(_crtSelectIndex, code);
            slotTaskSettingTime (code);
        });

        if(!_settingTimer.isActive())
            _settingTimer.start(1000);

        else
        {
            _settingTimer.stop();
            _settingTimer.start(1000);
        }

        return;
    }

    else if (name == QString("toZero")) //����
    {
        //plc�����е������
        for (PlcDevice* plc : _plcDevices)
        {
            if(_crtSelectIndex == 20) //���й���
            {
                if (plc && !plc->isConnect())
                {
                    emit signShowInfoToData(0,QString::fromLocal8Bit("%1#���ŵ������ʧ��,PLC�豸�����쳣").arg(_plcDevices.key(plc)+1));
                    continue;
                }
                int key = _plcDevices.key(plc);

                QVector<MotorDevice*> motors = plc->motorsOnManuIndex(5,QVector<int>{0,2});//���㲻�漰����
                for (int i = 0; i < motors.size(); i++)
                {
                    int idx = motors[i]->response()._index;
                    int motorId = motors[i]->response()._id;
                    int code = Motor_Ctrl_ZERO;
                    //�ص�0��
                    //                    motors[i]->setTargetPosition(motors[i]->zeroPosition());
                    int position = motors[i]->responPosition() - motors[i]->zeroPosition();//���λ��
                    int16_t posLow = (position & 0xFFFF);
                    int16_t posHigh = ((position >> 16)& 0xFFFF);

                    int speed = motors[i]->getCoarseSpeed();
                    int16_t speedLow = (speed & 0xFFFF);
                    int16_t speedHig = (speed >> 16 & 0xFFFF);

                    QVector<quint16> values =
                    {
                        static_cast<quint16>(motorId),
                        0, // ����λ���ض�����
                        static_cast<quint16>(code),
                        static_cast<quint16>(posHigh),
                        static_cast<quint16>(posLow),
                        static_cast<quint16>(speedHig),
                        static_cast<quint16>(speedLow),
                        0,
                    };

                    int startindex = 2020 + 20*idx;

                    // ʹ�ø�����������д�����Ĵ����������ֽ�����
                    QByteArray pduData = ModbusHelper::createWriteMultipleRegistersRequest(
                                startindex,
                                values
                                );

                    _sequencers[key]->enqueueRawRequest(1, pduData, QModbusPdu::WriteMultipleRegisters);
                }
            }

            else if (plc->index() == _crtSelectIndex)
            {
                if (plc && !plc->isConnect())
                {
                    emit signShowInfoToData(0,QString::fromLocal8Bit("%1#���ŵ������ʧ��,PLC�豸�����쳣").arg(_plcDevices.key(plc)+1));
                    continue;
                }
                int key = _plcDevices.key(plc);

                QVector<MotorDevice*> motors = plc->motorsOnManuIndex(5,QVector<int>{0,2});
                for (int i = 0; i < motors.size(); i++)
                {
                    int idx = motors[i]->response()._index;
                    int motorId = motors[i]->response()._id;
                    int code = Motor_Ctrl_ZERO;
                    //�ص�0��
                    //                    motors[i]->setTargetPosition(motors[i]->zeroPosition());
                    int position = motors[i]->responPosition() - motors[i]->zeroPosition();//���λ��
                    int16_t posLow  = (position & 0xFFFF);
                    int16_t posHigh = ((position >> 16)& 0xFFFF);

                    int speed = motors[i]->getCoarseSpeed();;
                    int16_t speedLow = (speed & 0xFFFF);
                    int16_t speedHig = (speed >> 16 & 0xFFFF);

                    QVector<quint16> values =
                    {
                        static_cast<quint16>(motorId),
                        0, // ����λ���ض�����
                        static_cast<quint16>(code),
                        static_cast<quint16>(posHigh),
                        static_cast<quint16>(posLow),
                        static_cast<quint16>(speedHig),
                        static_cast<quint16>(speedLow),
                        0,
                    };

                    int startindex = 2020 + 20*idx;

                    // ʹ�ø�����������д�����Ĵ����������ֽ�����
                    QByteArray pduData = ModbusHelper::createWriteMultipleRegistersRequest(
                                startindex,
                                values
                                );

                    _sequencers[key]->enqueueRawRequest(1, pduData, QModbusPdu::WriteMultipleRegisters);
                }
            }
        }
        return;
    }

    else if (name == QString("clear")) //
    {
        //plc�����е������
        for (PlcDevice* plc : _plcDevices)
        {
            if(_crtSelectIndex == 20) //���й���
            {
                if (plc && !plc->isConnect())
                {
                    emit signShowInfoToData(0,QString::fromLocal8Bit("%1#���ŵ������ʧ��,PLC�豸�����쳣").arg(_plcDevices.key(plc)+1));
                    continue;
                }
                int key = _plcDevices.key(plc);

                QVector<MotorDevice*> motors = plc->motors();
                for (int i = 0; i < motors.size(); i++)
                {
                    int idx = motors[i]->response()._index;
                    int motorId = motors[i]->response()._id;
                    int code = Motor_Ctrl_Folded;
                    //�ص�0��
                    //                    motors[i]->setTargetPosition(motors[i]->zeroPosition());
                    int position =0;//���λ��
                    int16_t posLow = (position & 0xFFFF);
                    int16_t posHigh = ((position >> 16)& 0xFFFF);

                    int speed = motors[i]->getCoarseSpeed();
                    int16_t speedLow = (speed & 0xFFFF);
                    int16_t speedHig = (speed >> 16 & 0xFFFF);

                    QVector<quint16> values =
                    {
                        static_cast<quint16>(motorId),
                        0, // ����λ���ض�����
                        static_cast<quint16>(code),
                        static_cast<quint16>(posHigh),
                        static_cast<quint16>(posLow),
                        static_cast<quint16>(speedHig),
                        static_cast<quint16>(speedLow),
                        0,
                    };

                    int startindex = 2020 + 20*idx;

                    // ʹ�ø�����������д�����Ĵ����������ֽ�����
                    QByteArray pduData = ModbusHelper::createWriteMultipleRegistersRequest(
                                startindex,
                                values
                                );

                    _sequencers[key]->enqueueRawRequest(1, pduData, QModbusPdu::WriteMultipleRegisters);
                }
            }

            else if (plc->index() == _crtSelectIndex)
            {
                if (plc && !plc->isConnect())
                {
                    emit signShowInfoToData(0,QString::fromLocal8Bit("%1#���ŵ������ʧ��,PLC�豸�����쳣").arg(_plcDevices.key(plc)+1));
                    continue;
                }
                int key = _plcDevices.key(plc);

                QVector<MotorDevice*> motors = plc->motors();
                for (int i = 0; i < motors.size(); i++)
                {
                    int idx = motors[i]->response()._index;
                    int motorId = motors[i]->response()._id;
                    int code = Motor_Ctrl_Folded;
                    //�ص�0��
                    //                    motors[i]->setTargetPosition(motors[i]->zeroPosition());
                    int position = 0;//���λ��
                    int16_t posLow  = (position & 0xFFFF);
                    int16_t posHigh = ((position >> 16)& 0xFFFF);

                    int speed = motors[i]->getCoarseSpeed();
                    int16_t speedLow = (speed & 0xFFFF);
                    int16_t speedHig = (speed >> 16 & 0xFFFF);

                    QVector<quint16> values =
                    {
                        static_cast<quint16>(motorId),
                        0, // ����λ���ض�����
                        static_cast<quint16>(code),
                        static_cast<quint16>(posHigh),
                        static_cast<quint16>(posLow),
                        static_cast<quint16>(speedHig),
                        static_cast<quint16>(speedLow),
                        0,
                    };

                    int startindex = 2020 + 20*idx;

                    // ʹ�ø�����������д�����Ĵ����������ֽ�����
                    QByteArray pduData = ModbusHelper::createWriteMultipleRegistersRequest(
                                startindex,
                                values
                                );

                    _sequencers[key]->enqueueRawRequest(1, pduData, QModbusPdu::WriteMultipleRegisters);
                }
            }
        }
        return;
    }


    else if (name == QString("warnRet")) //������λ
    {
        //plc ����
        for (PlcDevice* plc : _plcDevices)
        {
            if(_crtSelectIndex == 20) //���й���
            {
                if (plc && !plc->isConnect())
                {
                    emit signShowInfoToData(0,QString::fromLocal8Bit("%1#���ű�����λʧ��,PLC�豸�����쳣").arg(_plcDevices.key(plc)+1));
                    continue;
                }
                int key = _plcDevices.key(plc);

                int plcId = plc->id();

                QVector<MotorDevice*> motors = plc->motorsOnSection(_crtSelectSection);

                if(motors.size() == 0)
                    return;
                int idx = motors[0]->response()._index;
                int motorId = motors[0]->response()._id;
                int code = Motor_Ctrl_Urgen; //������λtodo 5

                int position = motors[0]->responPosition() + potCha;
                int16_t posLow = (position & 0xFFFF);
                int16_t posHigh = ((position >> 16)& 0xFFFF);

                int speed = motors[0]->getCoarseSpeed();
                int16_t speedLow = (speed & 0xFFFF);
                int16_t speedHig = (speed >> 16 & 0xFFFF);

                QVector<quint16> values =
                {
                    static_cast<quint16>(motorId),
                    0, // ����λ���ض�����
                    static_cast<quint16>(code),
                    static_cast<quint16>(posHigh),
                    static_cast<quint16>(posLow),
                    static_cast<quint16>(speedHig),
                    static_cast<quint16>(speedLow),
                };

                int startindex = 2020 + 20*idx;

                // ʹ�ø�����������д�����Ĵ����������ֽ�����
                QByteArray pduData = ModbusHelper::createWriteMultipleRegistersRequest(
                            startindex,
                            values
                            );

                _sequencers[key]->enqueueRawRequest(1, pduData, QModbusPdu::WriteMultipleRegisters);

            }

            else if (plc->index() == _crtSelectIndex)
            {
                if (plc && !plc->isConnect())
                {
                    emit signShowInfoToData(0,QString::fromLocal8Bit("%1#���ű�����λʧ��,PLC�豸�����쳣").arg(_plcDevices.key(plc)+1));
                    continue;
                }
                int key = _plcDevices.key(plc);

                int plcId = plc->id();
                //                int code = Plc_Ctrl_WarnRet; //todo ����plc

                //                QVector<quint16> values =
                //                {
                //                    static_cast<quint16>(plcId),
                //                    static_cast<quint16>(code),
                //                };


                //                int startindex = 4020 + 20*plc->index();
                //                QByteArray pduData = ModbusHelper::createWriteMultipleRegistersRequest(
                //                            startindex,
                //                            values
                //                            );
                //                _sequencers[key]->enqueueRawRequest(1, pduData, QModbusPdu::WriteMultipleRegisters);

                QVector<MotorDevice*> motors = plc->motorsOnSection(_crtSelectSection);

                if(motors.size() == 0)
                    return;
                int idx = motors[0]->response()._index;//����һ�����ɸ�λ
                int motorId = motors[0]->response()._id;
                int code = Motor_Ctrl_Urgen; //������λtodo 5

                int position = motors[0]->responPosition() + potCha;
                int16_t posLow = (position & 0xFFFF);
                int16_t posHigh = ((position >> 16)& 0xFFFF);

                int speed = motors[0]->getCoarseSpeed();
                int16_t speedLow = (speed & 0xFFFF);
                int16_t speedHig = (speed >> 16 & 0xFFFF);

                QVector<quint16> values =
                {
                    static_cast<quint16>(motorId),
                    0, // ����λ���ض�����
                    static_cast<quint16>(code),
                    static_cast<quint16>(posHigh),
                    static_cast<quint16>(posLow),
                    static_cast<quint16>(speedHig),
                    static_cast<quint16>(speedLow),
                };

                int startindex = 2020 + 20*idx;

                // ʹ�ø�����������д�����Ĵ����������ֽ�����
                QByteArray pduData = ModbusHelper::createWriteMultipleRegistersRequest(
                            startindex,
                            values
                            );

                _sequencers[key]->enqueueRawRequest(1, pduData, QModbusPdu::WriteMultipleRegisters);

            }
        }
        return;
    }

    else if (name == "stop")
    {
        for (PlcDevice* plc : _plcDevices)
        {

            if(_crtSelectIndex == 20) //���й���
            {
                if (plc && !plc->isConnect())
                {
                    emit signShowInfoToData(0,QString::fromLocal8Bit("%1#����ֹͣʧ��,PLC�豸�����쳣").arg(_plcDevices.key(plc)+1));
                    continue;
                }
                int key = _plcDevices.key(plc);
                QVector<MotorDevice*> motors = plc->motorsOnSection(_crtSelectSection);

                for (int k = 0; k < motors.size(); k++)
                {
                    int idx = motors[k]->response()._index;
                    int motorId = motors[k]->response()._id;
                    int code = Motor_Ctrl_STOP;

                    int position = motors[k]->responPosition() + potCha;
                    int16_t posLow = (position & 0xFFFF);
                    int16_t posHigh = ((position >> 16)& 0xFFFF);

                    int speed = motors[k]->getCoarseSpeed();
                    int16_t speedLow = (speed & 0xFFFF);
                    int16_t speedHig = (speed >> 16 & 0xFFFF);

                    QVector<quint16> values =
                    {
                        static_cast<quint16>(motorId),
                        0, // ����λ���ض�����
                        static_cast<quint16>(code),
                        static_cast<quint16>(posHigh),
                        static_cast<quint16>(posLow),
                        static_cast<quint16>(speedHig),
                        static_cast<quint16>(speedLow),
                    };

                    int startindex = 2020 + 20*idx;

                    // ʹ�ø�����������д�����Ĵ����������ֽ�����
                    QByteArray pduData = ModbusHelper::createWriteMultipleRegistersRequest(
                                startindex,
                                values
                                );

                    _sequencers[key]->enqueueRawRequest(1, pduData, QModbusPdu::WriteMultipleRegisters);
                }
            }
            else if (plc->index() == _crtSelectIndex)
            {
                if (plc && !plc->isConnect())
                {
                    emit signShowInfoToData(0,QString::fromLocal8Bit("%1#����ֹͣʧ��,PLC�豸�����쳣").arg(_plcDevices.key(plc)+1));
                    continue;
                }
                int key = _plcDevices.key(plc);
                QVector<MotorDevice*> motors = plc->motorsOnSection(_crtSelectSection);

                for (int k = 0; k < motors.size(); k++)
                {
                    int idx = motors[k]->response()._index;
                    int motorId = motors[k]->response()._id;
                    int code = Motor_Ctrl_STOP;

                    int position = motors[k]->responPosition() + potCha;
                    int16_t posLow = (position & 0xFFFF);
                    int16_t posHigh = ((position >> 16)& 0xFFFF);

                    int speed = motors[k]->getCoarseSpeed();
                    int16_t speedLow = (speed & 0xFFFF);
                    int16_t speedHig = (speed >> 16 & 0xFFFF);

                    QVector<quint16> values =
                    {
                        static_cast<quint16>(motorId),
                        0, // ����λ���ض�����
                        static_cast<quint16>(code),
                        static_cast<quint16>(posHigh),
                        static_cast<quint16>(posLow),
                        static_cast<quint16>(speedHig),
                        static_cast<quint16>(speedLow),
                    };

                    int startindex = 2020 + 20*idx;

                    // ʹ�ø�����������д�����Ĵ����������ֽ�����
                    QByteArray pduData = ModbusHelper::createWriteMultipleRegistersRequest(
                                startindex,
                                values
                                );

                    _sequencers[key]->enqueueRawRequest(1, pduData, QModbusPdu::WriteMultipleRegisters);
                }
            }
        }
        return;
    }

    //�������
    if (motorIndex > -1)
    {
        qDebug()<<_plcDevices.size();
        for (PlcDevice* plc : _plcDevices)
        {
            //            int aa = 0;
            if(_crtSelectIndex == 20) //���й���
            {
                if (plc && !plc->isConnect())
                {
                    emit signShowInfoToData(0,QString::fromLocal8Bit("%1#���ŵ������ʧ��,PLC�豸�����쳣").arg(_plcDevices.key(plc)+1));
                    continue;
                }

                int key = _plcDevices.key(plc);

                QVector<MotorDevice*> motors = plc->motorsOnManuIndex(_crtSelectSection,motorIndex);
                //                int time = motors.size() / 3;
                for (int j = 0; j < motors.size(); j++)
                {
                    int idx = motors[j]->response()._index;
                    int motorId = motors[j]->response()._id;
                    int code = Motor_Ctrl_FWD;

                    int position = motors[j]->responPosition() + potCha;
                    int16_t posLow = (position & 0xFFFF);
                    int16_t posHigh = ((position >> 16)& 0xFFFF);

                    int speed = motors[j]->getCoarseSpeed();
                    int16_t speedLow = (speed & 0xFFFF);
                    int16_t speedHig = (speed >> 16 & 0xFFFF);

                    QVector<quint16> values =
                    {
                        static_cast<quint16>(motorId),
                        0, // ����λ���ض�����
                        static_cast<quint16>(code),
                        static_cast<quint16>(posHigh),
                        static_cast<quint16>(posLow),
                        static_cast<quint16>(speedHig),
                        static_cast<quint16>(speedLow),
                        0,
                    };


                    int startindex = 2020 + 20*idx;

                    // ʹ�ø�����������д�����Ĵ����������ֽ�����
                    QByteArray pduData = ModbusHelper::createWriteMultipleRegistersRequest(
                                startindex,
                                values
                                );

                    _sequencers[key]->enqueueRawRequest(1, pduData, QModbusPdu::WriteMultipleRegisters);
                }
            }

            else if (plc->index() == _crtSelectIndex)
            {
                if (plc && !plc->isConnect())
                {
                    emit signShowInfoToData(0,QString::fromLocal8Bit("%1#���ŵ������ʧ��,PLC�豸�����쳣").arg(_plcDevices.key(plc)+1));
                    continue;
                }
                int key = _plcDevices.key(plc);

                QVector<MotorDevice*> motors = plc->motorsOnManuIndex(_crtSelectSection,motorIndex);
                //                int time = motors.size() / 3;
                for (int j = 0; j < motors.size(); j++)
                {
                    int idx = motors[j]->response()._index;
                    int motorId = motors[j]->response()._id;
                    int code = Motor_Ctrl_FWD;

                    int position = motors[j]->responPosition() + potCha;
                    int16_t posLow = (position & 0xFFFF);
                    int16_t posHigh = ((position >> 16)& 0xFFFF);

                    int speed = motors[j]->getCoarseSpeed();
                    int16_t speedLow = (speed & 0xFFFF);
                    int16_t speedHig = (speed >> 16 & 0xFFFF);

                    QVector<quint16> values =
                    {
                        static_cast<quint16>(motorId),
                        0, // ����λ���ض�����
                        static_cast<quint16>(code),
                        static_cast<quint16>(posHigh),
                        static_cast<quint16>(posLow),
                        static_cast<quint16>(speedHig),
                        static_cast<quint16>(speedLow),
                        0,
                    };

                    int startindex = 2020 + 20*idx;

                    // ʹ�ø�����������д�����Ĵ����������ֽ�����
                    QByteArray pduData = ModbusHelper::createWriteMultipleRegistersRequest(
                                startindex,
                                values
                                );

                    _sequencers[key]->enqueueRawRequest(1, pduData, QModbusPdu::WriteMultipleRegisters);
                }
            }
        }
    }
}
/**
 * @brief slotCtrlClicked ͨ�����ư�ť���õ��֧������λ��
 * @param name
 */
void DeviceManager::slotCtrlClicked (QString name,int plcIndex, int sectionIndex)
{
    if (name == QString("SetFoldedPosition")) //����λ��
    {
        int code = Motor_Ctrl_Folded_POS_Set;

        //plc���е������
        for (PlcDevice* plc : _plcDevices)
        {
            if(plcIndex == 20)
            {
                if (plc && !plc->isConnect())
                {
                    emit signShowInfoToSys(0,QString::fromLocal8Bit("%1#��������λ������ʧ��,PLC�豸�����쳣").arg(_plcDevices.key(plc)+1));
                    continue;
                }

                else if (!plc)
                    continue;

                int key = _plcDevices.key(plc);
                QVector<int> motorIndexs = {0,1,2};
                QVector<MotorDevice*> motors = plc->motorsOnManuIndex(5,motorIndexs);
                for (int i = 0; i < motors.size(); i++)
                {
                    int position = 0;

                    int idx = motors[i]->response()._index;
                    int motorId = motors[i]->response()._id;

                    int16_t posLow = (position & 0xFFFF);
                    int16_t posHigh = ((position >> 16)& 0xFFFF);


                    QVector<quint16> values =
                    {
                        static_cast<quint16>(motorId),
                        0, // ����λ���ض�����
                        static_cast<quint16>(code),
                        0, // ����λ���ض�����
                        0, // ����λ���ض�����
                        0, // ����λ���ض�����
                        0, // ����λ���ض�����
                        1, // ����λ���ض�����
                        static_cast<quint16>(posHigh),
                        static_cast<quint16>(posLow),

                    };

                    int startindex = 2020 + 20*idx;

                    // ʹ�ø�����������д�����Ĵ����������ֽ�����
                    QByteArray pduData = ModbusHelper::createWriteMultipleRegistersRequest(
                                startindex,
                                values
                                );

                    _sequencers[key]->enqueueRawRequest(1, pduData, QModbusPdu::WriteMultipleRegisters);
                }
            }
            else if (plc->index() == plcIndex)
            {
                if (plc && !plc->isConnect())
                {
                    emit signShowInfoToData(0,QString::fromLocal8Bit("%1#��������λ������ʧ��,PLC�豸�����쳣").arg(_plcDevices.key(plc)+1));
                    continue;
                }

                int key = _plcDevices.key(plc);
                QVector<int> motorIndexs = {0,1,2};
                QVector<MotorDevice*> motors = plc->motorsOnManuIndex(5,motorIndexs);
                for (int i = 0; i < motors.size(); i++)
                {
                    int position = motors[i]->responPosition();
                    motors[i]->setZeroPosition(position);

                    int idx = motors[i]->response()._index;
                    int motorId = motors[i]->response()._id;

                    int16_t posLow = (position & 0xFFFF);
                    int16_t posHigh = ((position >> 16)& 0xFFFF);


                    QVector<quint16> values =
                    {
                        static_cast<quint16>(motorId),
                        0, // ����λ���ض�����
                        static_cast<quint16>(code),
                        0, // ����λ���ض�����
                        0, // ����λ���ض�����
                        0, // ����λ���ض�����
                        0, // ����λ���ض�����
                        1, // ����λ���ض�����
                        static_cast<quint16>(posHigh),
                        static_cast<quint16>(posLow),

                    };

                    int startindex = 2020 + 20*idx;

                    // ʹ�ø�����������д�����Ĵ����������ֽ�����
                    QByteArray pduData = ModbusHelper::createWriteMultipleRegistersRequest(
                                startindex,
                                values
                                );

                    _sequencers[key]->enqueueRawRequest(1, pduData, QModbusPdu::WriteMultipleRegisters);
                }
            }
        }

        int timeNum = 20;
        static QMetaObject::Connection conn = connect(&_settingTimer, &QTimer::timeout, this,
                                                      [=]()
        {
            slotTakeSettingStatus(plcIndex, code);
        });

        //        connect (&_settingTimer, SIGNAL(timeout()), this, SLOT(slotTakeSettingStatus()));

        if(!_settingTimer.isActive())
            _settingTimer.start(1000);

        else
        {
            _settingTimer.stop();
            _settingTimer.start(1000);
        }

        return;
    }

    else if (name == QString("toFoldedPosition")) //����
    {
        //plc�����е������
        for (PlcDevice* plc : _plcDevices)
        {
            if(plcIndex == 20) //���й���
            {
                if (plc && !plc->isConnect())
                {
                    emit signShowInfoToData(0,QString::fromLocal8Bit("%1#��������ʧ��,PLC�豸�����쳣").arg(_plcDevices.key(plc)+1));
                    continue;
                }
                int key = _plcDevices.key(plc);
                QVector<int> motorIndexs = {0,2};
                QVector<MotorDevice*> motors = plc->motorsOnManuIndex(5,motorIndexs);
                for (int i = 0; i < motors.size(); i++)
                {
                    int idx = motors[i]->response()._index;
                    int motorId = motors[i]->response()._id;
                    int code = Motor_Ctrl_Folded;
                    //�ص�0��
                    int position = 0;//���λ��
                    int16_t posLow = (position & 0xFFFF);
                    int16_t posHigh = ((position >> 16)& 0xFFFF);

                    int speed = motors[i]->getCoarseSpeed();
                    int16_t speedLow = (speed & 0xFFFF);
                    int16_t speedHig = (speed >> 16 & 0xFFFF);

                    QVector<quint16> values =
                    {
                        static_cast<quint16>(motorId),
                        0, // ����λ���ض�����
                        static_cast<quint16>(code),
                        static_cast<quint16>(posHigh),
                        static_cast<quint16>(posLow),
                        static_cast<quint16>(speedHig),
                        static_cast<quint16>(speedLow),
                        0,
                    };

                    int startindex = 2020 + 20*idx;

                    // ʹ�ø�����������д�����Ĵ����������ֽ�����
                    QByteArray pduData = ModbusHelper::createWriteMultipleRegistersRequest(
                                startindex,
                                values
                                );

                    _sequencers[key]->enqueueRawRequest(1, pduData, QModbusPdu::WriteMultipleRegisters);
                }
            }

            else if (plc->index() == plcIndex)
            {
                if (plc && !plc->isConnect())
                {
                    emit signShowInfoToData(0,QString::fromLocal8Bit("%1#��������ʧ��,PLC�豸�����쳣").arg(_plcDevices.key(plc)+1));
                    continue;
                }
                int key = _plcDevices.key(plc);

                QVector<int> motorIndexs = {0,2};
                QVector<MotorDevice*> motors = plc->motorsOnManuIndex(5,motorIndexs);
                for (int i = 0; i < motors.size(); i++)
                {
                    int idx = motors[i]->response()._index;
                    int motorId = motors[i]->response()._id;
                    int code = Motor_Ctrl_Folded;
                    //�ص�0��
                    int position = 0;//���λ��
                    int16_t posLow  = (position & 0xFFFF);
                    int16_t posHigh = ((position >> 16)& 0xFFFF);

                    int speed = motors[i]->getCoarseSpeed();
                    int16_t speedLow = (speed & 0xFFFF);
                    int16_t speedHig = (speed >> 16 & 0xFFFF);

                    QVector<quint16> values =
                    {
                        static_cast<quint16>(motorId),
                        0, // ����λ���ض�����
                        static_cast<quint16>(code),
                        static_cast<quint16>(posHigh),
                        static_cast<quint16>(posLow),
                        static_cast<quint16>(speedHig),
                        static_cast<quint16>(speedLow),
                        0,
                    };

                    int startindex = 2020 + 20*idx;

                    // ʹ�ø�����������д�����Ĵ����������ֽ�����
                    QByteArray pduData = ModbusHelper::createWriteMultipleRegistersRequest(
                                startindex,
                                values
                                );

                    _sequencers[key]->enqueueRawRequest(1, pduData, QModbusPdu::WriteMultipleRegisters);
                }
            }
        }
        return;
    }
}

void DeviceManager::slotFoldDoClicked(QString name, double value)
{
    if (name == QString("foldDo")) //֧��ѹʵ
    {
        int code = Motor_Ctrl_FoldDo;
        //plc�����е������
        for (PlcDevice* plc : _plcDevices)
        {
            if(_crtSelectIndex == 20) //���й���
            {
                if (plc && !plc->isConnect())
                {
                    emit signShowInfoToData(0,QString::fromLocal8Bit("%1#����֧��ѹʵʧ��,PLC�豸�����쳣").arg(_plcDevices.key(plc)+1));
                    continue;
                }

                int key = _plcDevices.key(plc);
                QVector<int> motorIndexs = {0,2};
                QVector<MotorDevice*> motors = plc->motorsOnManuIndex(5,motorIndexs);

                MotorStatus state = this->checkMotorsCanWork (motors);

                if (state != MOTOR_STATUS_IDEL)
                {
                    emit signCannotWorkMessage(state);
                    return;
                }

                for (int i = 0; i < motors.size(); i++)
                {
                    int idx = motors[i]->response()._index;
                    int motorId = motors[i]->response()._id;
                    //                    int code = Motor_Ctrl_FoldDo;
                    //�ص�0��
                    int position = value;//���λ��
                    int16_t posLow = (position & 0xFFFF);
                    int16_t posHigh = ((position >> 16)& 0xFFFF);

                    int speed = motors[i]->getCoarseSpeed();
                    int16_t speedLow = (speed & 0xFFFF);
                    int16_t speedHig = (speed >> 16 & 0xFFFF);

                    QVector<quint16> values =
                    {
                        static_cast<quint16>(motorId),
                        0, // ����λ���ض�����
                        static_cast<quint16>(code),
                        static_cast<quint16>(posHigh),
                        static_cast<quint16>(posLow),
                        static_cast<quint16>(speedHig),
                        static_cast<quint16>(speedLow),
                        0,
                    };

                    int startindex = 2020 + 20*idx;

                    // ʹ�ø�����������д�����Ĵ����������ֽ�����
                    QByteArray pduData = ModbusHelper::createWriteMultipleRegistersRequest(
                                startindex,
                                values
                                );

                    _sequencers[key]->enqueueRawRequest(1, pduData, QModbusPdu::WriteMultipleRegisters);
                }
            }

            else if (plc->index() == _crtSelectIndex)
            {
                if (plc && !plc->isConnect())
                {
                    emit signShowInfoToData(0,QString::fromLocal8Bit("%1#����֧��ѹʵʧ��,PLC�豸�����쳣").arg(_plcDevices.key(plc)+1));
                    continue;
                }
                int key = _plcDevices.key(plc);

                QVector<int> motorIndexs = {0,2};
                QVector<MotorDevice*> motors = plc->motorsOnManuIndex(5,motorIndexs);

                MotorStatus state = this->checkMotorsCanWork (motors);

                if (state != MOTOR_STATUS_IDEL)
                {
                    emit signCannotWorkMessage(state);
                    return;
                }

                for (int i = 0; i < motors.size(); i++)
                {
                    int idx = motors[i]->response()._index;
                    int motorId = motors[i]->response()._id;
                    //                    int code = Motor_Ctrl_FoldDo;
                    //�ص�0��
                    int position = value;//���λ��
                    int16_t posLow  = (position & 0xFFFF);
                    int16_t posHigh = ((position >> 16)& 0xFFFF);

                    int speed = motors[i]->getCoarseSpeed();
                    int16_t speedLow = (speed & 0xFFFF);
                    int16_t speedHig = (speed >> 16 & 0xFFFF);

                    QVector<quint16> values =
                    {
                        static_cast<quint16>(motorId),
                        0, // ����λ���ض�����
                        static_cast<quint16>(code),
                        static_cast<quint16>(posHigh),
                        static_cast<quint16>(posLow),
                        static_cast<quint16>(speedHig),
                        static_cast<quint16>(speedLow),
                        0,
                    };

                    int startindex = 2020 + 20*idx;

                    // ʹ�ø�����������д�����Ĵ����������ֽ�����
                    QByteArray pduData = ModbusHelper::createWriteMultipleRegistersRequest(
                                startindex,
                                values
                                );

                    _sequencers[key]->enqueueRawRequest(1, pduData, QModbusPdu::WriteMultipleRegisters);
                }
            }
        }

        static QMetaObject::Connection conn = connect(&_settingTimer, &QTimer::timeout, this,
                                                      [=]()
        {
            slotTakeSettingStatus(_crtSelectIndex, code);
        });

        //        connect (&_settingTimer, SIGNAL(timeout()), this, SLOT(slotTakeSettingStatus()));

        if(!_settingTimer.isActive())
            _settingTimer.start(1000);

        else
        {
            _settingTimer.stop();
            _settingTimer.start(1000);
        }

        return;
    }
}


// ���� ͬ���ʱ�̶�˳������(L) < ��(ˮƽ) < ����(R)�����������ȶ� ����
//�����������ȶ�����Ҳͬ���� TrackLevelingSolver::sortActs �
static int actTypeRank(TrackLevelingSolver::ActType t){
    using T = TrackLevelingSolver::ActType;
    return (t==T::VertL)?0 : (t==T::HorizCenter?1:2);
}

QVector<TrackLevelingSolver::Actuator>
DeviceManager::buildActsFromMeasurement(const QVector<TrackLevelingSolver::Measure>& rows)
{
    QVector<TrackLevelingSolver::Actuator> acts;
    acts.reserve(rows.size()*3);
    for (const auto& m : rows) {
        TrackLevelingSolver::Actuator a;
        a.s = m.s; a.hLeak=0.0; a.a=1.0; a.b=0.0; a.id=-1;

        a.type = TrackLevelingSolver::ActType::VertL;       acts.append(a);
        a.type = TrackLevelingSolver::ActType::HorizCenter; acts.append(a);
        a.type = TrackLevelingSolver::ActType::VertR;       acts.append(a);
    }
    // �ȶ�����������ȣ��������˳�� L<C<R��
    std::sort(acts.begin(), acts.end(), [&](const TrackLevelingSolver::Actuator& A,
              const TrackLevelingSolver::Actuator& B){
        if (A.s < B.s) return true;
        if (A.s > B.s) return false;
        return actTypeRank(A.type) < actTypeRank(B.type);
    });
    return acts;
}

void DeviceManager::ensureSession(const QVector<TrackLevelingSolver::Measure>& firstMeas)
{
    const int rowCount = firstMeas.size();

    // ��һ�λ�ά�ȱ��� �� �ؽ����� run �� session/acts/params
    if (m_session.isNull() || m_lastRowCount != rowCount)
    {
        m_acts = buildActsFromMeasurement(firstMeas);

        // Ŀ�꣺���Գ��ߣ�����/����ȫ�������㡱
        m_params = TrackLevelingSolver::Params{};
        // ������õ��ǡ��� useSuperelevation ���ء�����ͷ�ļ���
        // m_params.useSuperelevation = false;
        m_params.keepMean   = false;
        m_params.targetMean = 0.0;
        m_params.tol        = m_tolForStop;

        m_session.reset(new LevelingSession(m_acts, m_params, /*tol=*/m_tolForStop, /*firstScale=*/m_firstScale));
        m_lastRowCount = rowCount;
        qDebug().noquote() << "[DeviceManager] session created. rows =" << rowCount;
    }
}

void DeviceManager::resetSession()
{
    m_session.reset(nullptr);
    m_acts.clear();
    m_lastRowCount = -1;
    qDebug().noquote() << "[DeviceManager] session reset.";
}

void DeviceManager::slotDataCheck()
{
    // 1) ׼����һ�ֵĲ��� y0�����л�����������ת�� solver �� Measure �б�
    QVector<TrackLevelingSolver::Measure> y0;
    if (_crtMesureSelectIndex != -1)
    {
        // ���� ���о��� ����
        TrackMesure mesure = _mesures.at(_crtMesureSelectIndex);
        y0 = acquireMeasurement_pass(QList<TrackMesure>{mesure});
    }

    else
    {
        // ���� �������� ����
        y0 = acquireMeasurement_pass(_mesures); // �����еķ�װ
    }

    if (y0.isEmpty())
        return;

    // 2) ����ʼ����ά�ȱ仯ʱ�ؽ� session���״λṹ�� acts/params��
    ensureSession(y0);

    // 3) �ûỰ�㱾������ڲ��Զ��жϣ���һ���� planStep1���ڶ����������+planStep2��
    QVector<double> cmd = m_session->next(y0);

    // 4) ������д�� UI/���ݽṹ��ע��˳��ÿ�й̶� [��,��,��]��
    if (_crtMesureSelectIndex != -1)
    {
        if (cmd.size() >= 3)
        {
            _mesures[_crtMesureSelectIndex]._lGuiAdjustmentn = cmd[0];
            _mesures[_crtMesureSelectIndex]._cLinedjustmentn = cmd[1];
            _mesures[_crtMesureSelectIndex]._rGuiAdjustmentn = cmd[2];

            emit signMesureUpdated(_crtMesureSelectIndex, _mesures[_crtMesureSelectIndex]);
        }
        printCommands(y0, cmd, (m_session->passes()==1) ? "PASS #1" : "PASS #2+");
    }
    else
    {
        const int n = _mesures.size();
        if (cmd.size() != 3*n)
        {
            qWarning() << "[DeviceManager] command size mismatch:" << cmd.size() << "expected" << 3*n;
            return;
        }
        for (int i = 0; i < n; ++i)
        {
            _mesures[i]._lGuiAdjustmentn = cmd[3*i + 0];
            _mesures[i]._cLinedjustmentn = cmd[3*i + 1];
            _mesures[i]._rGuiAdjustmentn = cmd[3*i + 2];
            emit signMesureUpdated(i, _mesures[i]);
        }

        printCommands(y0, cmd, (m_session->passes()==1) ? "PASS #1" : "PASS #2+");
    }

    // 5) ����ѡ������Ѿ����ݲ��ʾ�ϲ������ǰ���� reset
    if (m_session->inTolerance(y0) && m_session->passes() > 0)
    {
        qDebug().noquote() << "[DeviceManager] in tolerance now. call resetSession() when this run is done.";
        // ��������ں�ʱ reset��������ִ�в�ȷ����ɡ����û��㡰�������񡱣�
        // resetSession();
    }
}

void DeviceManager::slotUdpRecvd(QByteArray data)
{
    //���ݵ�����ֽ��ж��������� ��id��
    int id = (int)data.at(4);
    static int flag1 = 0;
    static int flag2 = 0;
    static TrackMesure mesure;
    static int oldma = -1;
    static int newma = -1;
    switch (id)
    {
    case 1:
    {
        qDebug () << "data 1= " << data;
        quint8 ma = qFromBigEndian<quint8>(reinterpret_cast<const uchar*>(data.data() + 5));
        qint32 me = qFromBigEndian<qint32>(reinterpret_cast<const uchar*>(data.data() + 7));

        qint16 gjpc = qFromBigEndian<qint16>(reinterpret_cast<const uchar*>(data.data() + 11));
        //        TrackMesure mesure;
        mesure._mileage = me/100.0;
        mesure._guiDeviationn = gjpc/100.0;
        mesure._id = ma;
        newma = ma;
        //        _mesures.append(mesure);

        //        if (gjpc > 0)
        {
            flag1++;
            //emit signMesureUpdate(mesure);
            //_mesures.append(mesure);
        }

        break;
    }

    case 2:
    {
        qDebug () << "data 2= " << data;
        //        flag ++;
        qint16 zuo = qFromBigEndian<qint16>(reinterpret_cast<const uchar*>(data.data() + 5));
        qint16 mid = qFromBigEndian<qint16>(reinterpret_cast<const uchar*>(data.data() + 7));
        qint16 you = qFromBigEndian<qint16>(reinterpret_cast<const uchar*>(data.data() + 9));
        qint16 chao = qFromBigEndian<qint16>(reinterpret_cast<const uchar*>(data.data() + 11));

        mesure._lRailElevationn = zuo/100.0;
        mesure._cLinePlann = mid/100.0;
        mesure._rRailElevationn = you/100.0;
        mesure._supDeviationn = chao/100.0;

        //        if (zuo > 0)
        {
            flag2 ++;
        }

        break;
    }

    case 3:
    {
        qDebug () << "data 2= " << data;
        //        flag ++;
        qint16 zuo = qFromBigEndian<qint16>(reinterpret_cast<const uchar*>(data.data() + 5));
        qint16 you = qFromBigEndian<qint16>(reinterpret_cast<const uchar*>(data.data() + 7));



        break;
    }
    default:
        break;
    }

    if (flag1 >= 1 && flag2 >= 1)
    {
        qDebug () << "flag1 >= 1 && flag2 >= 1" ;
        emit signMesureUpdate(mesure);
        //_mesures.append(mesure);
        flag1 = 0;
        flag2 = 0;
        oldma = newma;
    }
}

void DeviceManager::moveToTarget (PlcDevice* plc, QVector<MotorDevice*> motors)
{
    int key = _plcDevices.key(plc);

    for (int i = 0; i < motors.size(); i++)
    {
        int idx = motors[i]->response()._index;
        int motorId = motors[i]->response()._id;
        int code = Motor_Ctrl_FWD;

        int position = motors[i]->targetPosition ();
        int16_t posLow = (position & 0xFFFF);
        int16_t posHigh = ((position >> 16)& 0xFFFF);

        int speed = motors[i]->getFineSpeed();
        int16_t speedLow = (speed & 0xFFFF);
        int16_t speedHig = (speed >> 16 & 0xFFFF);

        QVector<quint16> values =
        {
            static_cast<quint16>(motorId),
            0, // ����λ���ض�����
            static_cast<quint16>(code),
            static_cast<quint16>(posHigh),
            static_cast<quint16>(posLow),
            static_cast<quint16>(speedHig),
            static_cast<quint16>(speedLow),
        };

        int startindex = 2020 + 20*idx;

        // ʹ�ø�����������д�����Ĵ����������ֽ�����
        QByteArray pduData = ModbusHelper::createWriteMultipleRegistersRequest(
                    startindex,
                    values
                    );

        // �������󣨷�����ID=1��������=16��
        _sequencers[key]->enqueueRawRequest(1, pduData, QModbusPdu::WriteMultipleRegisters);
    }
}

QString DeviceManager::getMotorName(int index)
{
    char name[32];
    //index����-0��2��4��6����-1��3��5��7����-11��10��9��8
    if(index < 8)
    {
        sprintf(name,"����%d���%d",index/2+1,index%2+1);
    }
    else
    {
        sprintf(name,"����%d���3",12-index);
    }
    return QString::fromLocal8Bit(name);
}

void DeviceManager::ensureCsvHeader(QFile &file, QTextStream &ts)
{
    if (file.size() == 0) {
        // ��ѡ��д�� UTF-8 BOM������ Excel ֱ��ʶ������
        const unsigned char bom[3] = {0xEF, 0xBB, 0xBF};
        file.write(reinterpret_cast<const char*>(bom), 3);

        ts << "serialNumber"
           << "," << "id"
           << "," << "mileage"
           << "," << "lRailElevation"
           << "," << "cLinePlan"
           << "," << "rRailElevation"
           << "," << "supDeviation"
           << "," << "gaugeDeviation"
           << "," << "lGaugeAdjustment"
           << "," << "cLineAdjustment"
           << "," << "rGaugeAdjustment"
           << "\n";
        ts.flush();
    }
}

bool DeviceManager::appendTrackMesureCsv(const QString &filePath, const TrackMesure &m,
                                         int precision)
{
    // ȷ��Ŀ¼���ڣ���ѡ��
    QDir dir = QFileInfo(filePath).dir();
    if (!dir.exists())
        dir.mkpath(".");

    QFile file(filePath);
    if (!file.open(QIODevice::ReadWrite | QIODevice::Append)) {
        // ���ļ������ڵ��� Append �򲻿����ɳ��� ReadWrite ��������
        if (!file.open(QIODevice::ReadWrite)) return false;
    }

    // ��дָ���Ƶ��ļ�ĩβ��Append ͨ������ˣ����������
    file.seek(file.size());

    QTextStream ts(&file);
    // Qt4����ʽ�趨������С����ΪӢ�ľ��
    ts.setCodec(QTextCodec::codecForName("UTF-8"));
    ts.setLocale(QLocale::c()); // ��֤С����Ϊ'.'������ϵͳ����Ӱ��
    ts.setRealNumberNotation(QTextStream::FixedNotation);
    ts.setRealNumberPrecision(precision); // С��λ���ɵ�

    // �״�д���ͷ
    ensureCsvHeader(file, ts);

    // ���ֶ��������Ϊ double������ת�壩���Զ��ŷָ���ĩβ����
    ts << m._serialNumber
       << "," << m._id
       << "," << m._mileage
       << "," << m._lRailElevationn
       << "," << m._cLinePlann
       << "," << m._rRailElevationn
       << "," << m._supDeviationn
       << "," << m._guiDeviationn
       << "," << m._lGuiAdjustmentn
       << "," << m._cLinedjustmentn
       << "," << m._rGuiAdjustmentn
       << "\n";

    ts.flush();
    file.flush();
    file.close();
    return true;
}


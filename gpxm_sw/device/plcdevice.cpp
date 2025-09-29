#include "plcdevice.h"
#include <QVariant>
#include <QDebug>
#include <QThread>
int g_FineSpeed = 40;//精调速度
int g_CoarseSpeed = 80;//粗调速度
int g_CenterLineSpeed = 30;//中线速度

PlcDevice::PlcDevice(int index):
    _index (index),
    _valid (false),
    _isPolling (false),
    _pendingRequests(0),
    _currentMotorIndex(0)
{
    _plcResponse._id = 0;
    _plcResponse._index = index;
    _motorRetryCount.fill(0, PerMotorCount);
    _client = NULL;
    _client= new QModbusTcpClient(this);

    for(int i = 0; i < PerMotorCount;i++)
    {
        if(i ==1 || i== 3||i == 5|| i==7)
        {
            //中线电机
            _motorDevice[i].setCoarseSpeed(g_CenterLineSpeed);
            _motorDevice[i].setFineSpeed(g_CenterLineSpeed);
        }
        else
        {
            //高低程电机
            _motorDevice[i].setCoarseSpeed(g_CoarseSpeed);
            _motorDevice[i].setFineSpeed(g_FineSpeed);
        }
    }

}
void PlcDevice::init()
{

    connect(_client, &QModbusTcpClient::stateChanged, [=](QModbusDevice::State s)
    {
        slotPlcStateChanged(s);
    });

    connect(&_connTimer, SIGNAL(timeout()), this, SLOT(tryConnect()));
    _connTimer.start(1000);

    //    connect(&_pollTimer, SIGNAL(timeout()), this, SLOT(pollAll()));
    //    _pollTimer.start(800);

    this->defaultSection();
}
PlcDevice::~PlcDevice()
{
    if(_client->state()==QModbusDevice::ConnectedState)
        _client->disconnectDevice();
}

void PlcDevice::defaultSection()
{
    // 断面1对应0,1,11号电机
    _sections.insert(1, {0, 1, 11});

    // 断面2对应2,3,10号电机
    _sections.insert(2, {2, 3, 10});

    // 断面3对应4,5,9号电机
    _sections.insert(3, {4, 5, 9});

    // 断面4对应6,7,8号电机
    _sections.insert(4, {6, 7, 8});
}

int PlcDevice::id() const
{
    return this->_id;
}

int PlcDevice::port() const
{
    return this->_port;
}

QString PlcDevice::name() const
{
    return this->_name;
}

QString PlcDevice::ip() const
{
    return this->_ipAddress;
}

int PlcDevice::index() const
{
    return _index;
}

QModbusTcpClient *PlcDevice::client() const
{
    if (_client)
        return _client;
    else
        return NULL;
}

bool PlcDevice::isConnect() const
{
    return _connect;
}

bool PlcDevice::valid() const
{
    return _valid;
}

double PlcDevice::totalHeight() const
{
    double totalHeight = 0;
    for(int i = 0 ; i < PerMotorCount; i++)
    {
        MotorDevice motor = _motorDevice[i];
        if (motor.response()._index != 2 || motor.response()._index != 4 || motor.response()._index != 6 || motor.response()._index != 8)
            totalHeight += (motor.response()._positionH <<16 |(unsigned short)motor.response()._positionL)/100.0;
    }

    return totalHeight;
}

void PlcDevice::setId(const int id)
{
    this->_id = id;
}

void PlcDevice::setPort(const int port)
{
    this->_port = port;
}

void PlcDevice::setName(const QString &name)
{
    this->_name = name;
}

void PlcDevice::setIp(const QString &ip)
{
    this->_ipAddress = ip;
}

QMap<int, QVector<int> > PlcDevice::sectionMap()
{
    return this->_sections;
}

QMap<int, int> PlcDevice::sectionIDMap()
{
    return this->_sectionsID;
}

QVector<int> PlcDevice::sectionVector(int section)
{
    return this->_sections[section];
}

void PlcDevice::appendSection(int section, QVector<int> motors)
{
    this->_sections.insert(section, motors);
}

void PlcDevice::appendSectionID (int sectionID, int sectionSn)
{
    this->_sectionsID.insert(sectionID, sectionSn);
}

PlcResponse PlcDevice::response() const
{
    return this->_plcResponse;
}

QVector<MotorDevice*> PlcDevice::motors()
{
    QVector<MotorDevice*> motors;

    for (int i = 0; i < PerMotorCount; i++)
    {
        motors.append(&_motorDevice[i]);
    }

    return motors;
}

QVector<MotorDevice*> PlcDevice::motorsOnSection(int section)
{
    QVector<MotorDevice*> motors;
    QVector<int> idxs = _sections.value(section);

    if(section == 5) //全部断面
    {
        for (int i = 0; i < PerMotorCount; i++)
            motors.append(&_motorDevice[i]);
    }

    else
    {
        for (int i = 0; i < idxs.size(); i++)
        {
            motors.append(&_motorDevice[idxs[i]]);
        }
    }

    return motors;
}

QVector<MotorDevice *> PlcDevice::motorsOnManuIndex(int section, int manuIndex)
{
    QVector<MotorDevice*> motors;

    if(section == 5) //全部断面
    {
        QList<QVector<int>> idxs = _sections.values();
        for (int i = 0; i < idxs.size(); i++)
        {
            QVector<int> singleSection = idxs[i];
            if (manuIndex == 0) //h1
            {
                motors.append(&_motorDevice[singleSection.at(0)]);
            }

            else if (manuIndex == 1)
            {
                motors.append(&_motorDevice[singleSection.at(1)]);
            }

            else if (manuIndex == 2)
            {
                motors.append(&_motorDevice[singleSection.at(2)]);
            }

        }
    }

    else
    {
        QVector<int> idxs = _sections.value(section);

        if (manuIndex == 0) //h1
        {
            motors.append(&_motorDevice[idxs[0]]);
        }

        else if (manuIndex == 1)
        {
            motors.append(&_motorDevice[idxs[1]]);
        }

        else if (manuIndex == 2)
        {
            motors.append(&_motorDevice[idxs[2]]);
        }
    }

    return motors;
}

QVector<MotorDevice *> PlcDevice::motorsOnManuIndex(int section, QVector<int> manuIndexs)
{
    QVector<MotorDevice*> motors;

    if(section == 5) //全部断面
    {
        foreach (int manuIndex, manuIndexs)
        {
            QList<QVector<int>> idxs = _sections.values();
            for (int i = 0; i < idxs.size(); i++)
            {
                QVector<int> singleSection = idxs[i];
                if (manuIndex == 0) //h1
                {
                    motors.append(&_motorDevice[singleSection.at(0)]);
                }

                else if (manuIndex == 1)
                {
                    motors.append(&_motorDevice[singleSection.at(1)]);
                }

                else if (manuIndex == 2)
                {
                    motors.append(&_motorDevice[singleSection.at(2)]);
                }

            }
        }
    }

    else
    {
        QVector<int> idxs = _sections.value(section);

        foreach (int manuIndex, manuIndexs)
        {
            if (manuIndex == 0) //h1
            {
                motors.append(&_motorDevice[idxs[0]]);
            }

            else if (manuIndex == 1)
            {
                motors.append(&_motorDevice[idxs[1]]);
            }

            else if (manuIndex == 2)
            {
                motors.append(&_motorDevice[idxs[2]]);
            }
        }
    }

    return motors;
}

void PlcDevice::slotPlcStateChanged(QModbusDevice::State s)
{
    this->_connect = (s == QModbusDevice::ConnectedState);
    //        if(s == QModbusDevice::ConnectedState)
    //            qDebug () << (QString::fromLocal8Bit("PLC %1 连接成功").arg(_name));
    //        else if(s == QModbusDevice::UnconnectedState)
    //            qDebug () << (QString::fromLocal8Bit("PLC %1 已断开").arg(_name));
    //        else
    //            qDebug () << (QString::fromLocal8Bit("PLC %1 已断开").arg(_name));

}

void PlcDevice::tryConnect()
{
    if(_client->state()!=QModbusDevice::ConnectedState)
    {
        _client->setConnectionParameter(QModbusDevice::NetworkAddressParameter, QVariant(this->_ipAddress));
        _client->setConnectionParameter(QModbusDevice::NetworkPortParameter, QVariant(this->_port));
        _client->setTimeout(15000);
        _client->setNumberOfRetries(1);
        _client->connectDevice();
    }
}

/**
void PlcDevice::pollAll()
{
    if(_client->state() == QModbusDevice::ConnectedState)
    {
        // 1. 读PLC状态区：4000~4039
        QModbusDataUnit req(QModbusDataUnit::HoldingRegisters, 4000, 20);
        QModbusReply* reply = _client->sendReadRequest(req, 1);
        if(reply)
        {
            connect(reply, &QModbusReply::finished, [=]()
            {
                if(reply->error()==QModbusDevice::NoError)
                {
                    auto unit = reply->result();
                    //                    qDebug () << "plc values" << unit.values() << "size = " << unit.values().size();
                    PlcResponse& st = this->_plcResponse;
                    st._id  = unit.value(0);
                    st._status   = 1; //todo 好像没有发对
                    st._index = this->_index;
                    this->_connect = true;
                    this->_valid = true;
                }

                else
                {
                    PlcResponse& st = this->_plcResponse;
                    st._status = PLC_STATUS_ERROR;
                    this->_connect = false;
                }

                reply->deleteLater();
            });

            QThread::currentThread()->msleep(20);

        }
        //        // 2. 每台PLC下所有电机，分别读4040/4080/4120...
        if (this->_plcResponse._status == PLC_STATUS_NORMAL)
        {
            for(int m=0;m<PerMotorCount;m++)
            {
                int base = 4020 + 20 * m;
                QModbusDataUnit reqM(QModbusDataUnit::HoldingRegisters, base, 20);
                QModbusReply* reply2 = _client->sendReadRequest(reqM, 1);
                if(reply2)
                {
                    connect(reply2, &QModbusReply::finished, [=]()
                    {
                        if(reply2->error()==QModbusDevice::NoError)
                        {
                            auto unit = reply2->result();
                            //                            qDebug () << "ip= " << _ipAddress << "m = " << m;
                            MotorResponse ms;
                            ms._id           = unit.value(0);
                            ms._status       = unit.value(1);
                            ms._positionH     = unit.value(2);
                            ms._positionL     = unit.value(3);
                            ms._index = m; //m即为motor的index
                            //                            _motorDevice[m].setResponse (unit);
                            _motorDevice[m].setResponse(ms);
                        }

                        else
                        {
                            _motorDevice[m].setStatus(MOTOR_STATUS_ERROR);
                        }


                        reply2->deleteLater();
                    });
                }

                QThread::currentThread()->msleep(150);
            }
        }

        else
        {
            //plc 状态不在线，motor都无法使用
            for (int i = 0 ; i < PerMotorCount; i++)
            {
                _motorDevice[i].setStatus (MOTOR_STATUS_ERROR);
            }
        }
    }

    else
    {
        //        qDebug () << QString::fromLocal8Bit("连接失败");
        PlcResponse& st = this->_plcResponse;
        st._status   = PLC_STATUS_ERROR;
    }
}
**/

void PlcDevice::startPolling()
{
    if(_isPolling || _pendingRequests > 0)
        return;

    _isPolling = true;

    if(_client->state() != QModbusDevice::ConnectedState) {
        PlcResponse& st = _plcResponse;
        st._status = PLC_STATUS_ERROR;
        _connect = false;
        _isPolling = false;
        return;
    }

    // 1. 首先读取PLC状态
    QModbusDataUnit req(QModbusDataUnit::HoldingRegisters, 4000, 20);
    QModbusReply* reply = _client->sendReadRequest(req, 1);

    if(!reply) {
        handleError("Failed to send PLC status request");
        _isPolling = false;
        return;
    }

    _pendingRequests++;
    connect(reply, &QModbusReply::finished, this, [=]() {
        handlePLCResponse(reply);
        reply->deleteLater();
        _pendingRequests--;
    });
}

void PlcDevice::handlePLCResponse(QModbusReply* reply)
{
    if(reply->error() == QModbusDevice::NoError) {
        auto unit = reply->result();
        PlcResponse& st = _plcResponse;
        st._id = unit.value(0);
        st._status = PLC_STATUS_NORMAL; // 设置为正常状态
        st._index = _index;
        _connect = true;
        _valid = true;

        // PLC状态正常，开始读取电机状态
        _currentMotorIndex = 0;
        readNextMotor();
    } else {
        handleError(QString("PLC read error: %1").arg(reply->errorString()));
        PlcResponse& st = _plcResponse;
        st._status = PLC_STATUS_ERROR;
        _connect = false;

        // PLC错误时设置所有电机为错误状态
        for (int i = 0; i < PerMotorCount; i++) {
            _motorDevice[i].setStatus(MOTOR_STATUS_ERROR);
        }

        _isPolling = false;
    }
}

void PlcDevice::readNextMotor()
{
    if(_currentMotorIndex >= PerMotorCount) {
        // 所有电机读取完成
        _isPolling = false;
        return;
    }

    int m = _currentMotorIndex++;
    int base = 4020 + 20 * m;
    QModbusDataUnit reqM(QModbusDataUnit::HoldingRegisters, base, 20);
    QModbusReply* reply2 = _client->sendReadRequest(reqM, 1);

    if(!reply2) {
        handleMotorError(m, "Failed to send motor request");
        QTimer::singleShot(10, this, [=] { readNextMotor(); });
        return;
    }

    _pendingRequests++;
    connect(reply2, &QModbusReply::finished, this, [=]() {
        handleMotorResponse(reply2, m);
        reply2->deleteLater();
        _pendingRequests--;

        // 短暂延时后读取下一个电机
        QTimer::singleShot(50, this, [=] { readNextMotor(); });
    });
}

void PlcDevice::handleMotorResponse(QModbusReply* reply, int motorIndex)
{
    if(reply->error() == QModbusDevice::NoError) {
        auto unit = reply->result();
        MotorResponse ms;
        ms._id          = unit.value(0);
        ms._status      = unit.value(1);
        ms._positionH   = unit.value(2);
        ms._positionL   = unit.value(3);
        ms._moment      = unit.value(6);
        ms._setingStatus = unit.value(7);
        ms._index       = motorIndex;

//        qDebug () << "index = " << ms._index << "moment = " << ms._moment;

        _motorDevice[motorIndex].setResponse(ms);
        _motorRetryCount[motorIndex] = 0; // 重置重试计数器
    } else {
        // 错误处理带重试机制
        if(++_motorRetryCount[motorIndex] < 3) {
            QTimer::singleShot(20, this, [=] {
                // 重试当前电机
                _currentMotorIndex--;
                readNextMotor();
            });
        } else {
            handleMotorError(motorIndex, QString("Motor read error: %1")
                             .arg(reply->errorString()));
        }
    }
}

void PlcDevice::handleMotorError(int motorIndex, const QString& errorMsg)
{
    qWarning() << "PLC" << _ipAddress << "Motor" << motorIndex << errorMsg;
    _motorDevice[motorIndex].setStatus(MOTOR_STATUS_ERROR);
}

void PlcDevice::handleError(const QString& errorMsg)
{
    qWarning() << "PLC" << _ipAddress << "Error:" << errorMsg;
    PlcResponse& st = _plcResponse;
    st._status = PLC_STATUS_ERROR;
    _connect = false;

    for (int i = 0; i < PerMotorCount; i++) {
        _motorDevice[i].setStatus(MOTOR_STATUS_ERROR);
    }

    _isPolling = false;
}

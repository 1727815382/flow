#ifndef PLCDEVICE_H
#define PLCDEVICE_H

#include <QString>
#include <QModbusTcpClient>
#include <QObject>
#include <QTimer>

#include "aixsim.h"
#include "motordevice.h"

#define PerMotorCount 12

class PlcDevice : public QObject
{
    Q_OBJECT

public:

    PlcDevice(int index);
    ~PlcDevice();

    void defaultSection();
    void init();

public:

    int id () const;
    int port () const;
    QString name () const;
    QString ip () const;
    int index () const;
    QModbusTcpClient* client () const;

    bool isConnect () const;
    bool valid ()const;
    /**
     * @brief totalHeight 当前plc所控电机的总高度(垂直电机)
     * @return
     */
    double totalHeight () const;

    void setId (const int id);
    void setPort (const int port);
    void setName (const QString& name);
    void setIp (const QString& ip);


    QMap<int, QVector<int>>  sectionMap ();
    QMap<int, int>  sectionIDMap ();

    QVector<int> sectionVector(int section);
    void appendSection (int section, QVector<int> motors);
    void appendSectionID (int sectionID, int sectionSn);


    PlcResponse response() const;
    QVector<MotorDevice*> motors();
    QVector<MotorDevice*> motorsOnSection (int section);
    QVector<MotorDevice*> motorsOnManuIndex (int section, int manuIndex);
    QVector<MotorDevice*> motorsOnManuIndex (int section, QVector<int> manuIndexs);


public slots:

    void slotPlcStateChanged(QModbusDevice::State s);
    void tryConnect();
    void startPolling();
    void handlePLCResponse(QModbusReply* reply);
    void readNextMotor();
    void handleMotorResponse(QModbusReply* reply, int motorIndex);
    void handleMotorError(int motorIndex, const QString& errorMsg);
    void handleError(const QString& errorMsg);
private:

    int _id;                     // PLC唯一ID
    QString _name;               // PLC名称
    QString _ipAddress;          // PLC IP地址
    int _port;                   // 通信端口    
    bool _connect;
    bool _valid;
    int _index;

    PlcResponse   _plcResponse;      //plc状态
//    MotorResponse _motorResponse[PerMotorCount]; //电机状态
    MotorDevice _motorDevice[PerMotorCount]; //电机设备
    QMap<int, QVector<int>> _sections;      // 该PLC管理的断面序号和对应的电机下标
    QMap<int, int>  _sectionsID; // 该PLC管理的断面ID和对应的断面序号

private:

    QModbusTcpClient*  _client;
    QTimer _connTimer;
    QTimer _pollTimer;
    bool _isPolling;
    int _pendingRequests;
    int _currentMotorIndex;
    QVector<int> _motorRetryCount; // 每个电机的重试计数器
};

#endif // PLCDEVICE_H

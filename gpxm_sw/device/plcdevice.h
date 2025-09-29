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
     * @brief totalHeight ��ǰplc���ص�����ܸ߶�(��ֱ���)
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

    int _id;                     // PLCΨһID
    QString _name;               // PLC����
    QString _ipAddress;          // PLC IP��ַ
    int _port;                   // ͨ�Ŷ˿�    
    bool _connect;
    bool _valid;
    int _index;

    PlcResponse   _plcResponse;      //plc״̬
//    MotorResponse _motorResponse[PerMotorCount]; //���״̬
    MotorDevice _motorDevice[PerMotorCount]; //����豸
    QMap<int, QVector<int>> _sections;      // ��PLC����Ķ�����źͶ�Ӧ�ĵ���±�
    QMap<int, int>  _sectionsID; // ��PLC����Ķ���ID�Ͷ�Ӧ�Ķ������

private:

    QModbusTcpClient*  _client;
    QTimer _connTimer;
    QTimer _pollTimer;
    bool _isPolling;
    int _pendingRequests;
    int _currentMotorIndex;
    QVector<int> _motorRetryCount; // ÿ����������Լ�����
};

#endif // PLCDEVICE_H

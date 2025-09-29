#ifndef MOTORDEVICE_H
#define MOTORDEVICE_H

#include "aixsim.h"
#include <QModbusDataUnit>

class MotorDevice
{
public:
    MotorDevice();

public:

    void setResponse (QModbusDataUnit unit);
    void setResponse (const MotorResponse& res);
    void setStatus (MotorStatus status);

    MotorResponse response () const;

public:

    //将当前位置设置为0位
    void setZeroPosition (int position);
    void setTargetPosition (int position);
    int responPosition ();
    int targetPosition () const;

    MotorSettingStatus settingStatus () const;

    int zeroPosition ()const;
    void setCoarseSpeed(unsigned int speed = 20);//粗调速度
    void setFineSpeed(unsigned int speed = 20);//精调速度
    int getCoarseSpeed();
    int getFineSpeed();
private:

    MotorResponse _motorResponse; //电机状态

    int _targetPosition;
    int _zeroPosition;
    int _coarseSpeed;
    int _fineSpeed;
};

#endif // MOTORDEVICE_H

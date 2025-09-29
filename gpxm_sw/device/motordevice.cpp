#include "motordevice.h"

MotorDevice::MotorDevice():
    _zeroPosition (0)
{
    this->_motorResponse._setingStatus = Motor_Setting_Idel;
   _coarseSpeed = 20;
   _fineSpeed = 20;
}

void MotorDevice::setResponse(QModbusDataUnit unit)
{
    MotorResponse& ms = this->_motorResponse;
    ms._id           = unit.value(0);
    ms._status       = unit.value(1);
    ms._positionH     = unit.value(2);
    ms._positionL     = unit.value(3);
}

void MotorDevice::setResponse(const MotorResponse& res)
{
    this->_motorResponse = res;
}

void MotorDevice::setStatus(MotorStatus status)
{
    this->_motorResponse._status = status;
}

MotorResponse MotorDevice::response() const
{
    return this->_motorResponse;
}

void MotorDevice::setZeroPosition(int position)
{
    _zeroPosition = position;
}

void MotorDevice::setTargetPosition(int position)
{
    _targetPosition = position;
}

int MotorDevice::responPosition()
{
    int dis = (this->_motorResponse._positionH <<16 |(unsigned short)_motorResponse._positionL);
    return dis;
}

int MotorDevice::targetPosition() const
{
    return _targetPosition;
}

MotorSettingStatus MotorDevice::settingStatus() const
{
    return (MotorSettingStatus)_motorResponse._setingStatus;
}

int MotorDevice::zeroPosition() const
{
    return _zeroPosition;
}

void MotorDevice::setCoarseSpeed(unsigned int speed)
{
    _coarseSpeed =  speed;
}

void MotorDevice::setFineSpeed(unsigned int speed)
{
    _fineSpeed =  speed;
}

int MotorDevice::getCoarseSpeed()
{
    return _coarseSpeed;
}

int MotorDevice::getFineSpeed()
{
    return _fineSpeed;
}


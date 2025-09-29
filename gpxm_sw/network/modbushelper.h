#ifndef MODBUSHELPER_H
#define MODBUSHELPER_H


// ModbusHelper.h �ļ�����
#pragma once
#include <QtEndian>
#include <QVector>
#include <QByteArray>
#include <QModbusPdu>

namespace ModbusHelper
{

// д�뵥���Ĵ��� (������ 6)
QByteArray createWriteSingleRegisterRequest(int address, quint16 value);

// д�뵥����Ȧ (������ 5)
QByteArray createWriteSingleCoilRequest(int address, bool on);

// ��ȡ����Ĵ��� (������ 3)
QByteArray createReadHoldingRegistersRequest(int startAddress, quint16 count);

// д�����Ĵ��� (������ 16) - ����Ҫ�ĺ��Ĺ���
QByteArray createWriteMultipleRegistersRequest(int startAddress, const QVector<quint16> &values);

// ��ȡ��Ȧ״̬ (������ 1)
QByteArray createReadCoilsRequest(int startAddress, quint16 count);

} // namespace ModbusHelper
#endif // MODBUSHELPER_H

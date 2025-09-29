#ifndef MODBUSHELPER_H
#define MODBUSHELPER_H


// ModbusHelper.h 文件内容
#pragma once
#include <QtEndian>
#include <QVector>
#include <QByteArray>
#include <QModbusPdu>

namespace ModbusHelper
{

// 写入单个寄存器 (功能码 6)
QByteArray createWriteSingleRegisterRequest(int address, quint16 value);

// 写入单个线圈 (功能码 5)
QByteArray createWriteSingleCoilRequest(int address, bool on);

// 读取多个寄存器 (功能码 3)
QByteArray createReadHoldingRegistersRequest(int startAddress, quint16 count);

// 写入多个寄存器 (功能码 16) - 您需要的核心功能
QByteArray createWriteMultipleRegistersRequest(int startAddress, const QVector<quint16> &values);

// 读取线圈状态 (功能码 1)
QByteArray createReadCoilsRequest(int startAddress, quint16 count);

} // namespace ModbusHelper
#endif // MODBUSHELPER_H

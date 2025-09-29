#include "modbushelper.h"

// ModbusHelper.cpp 文件内容
#include "ModbusHelper.h"

namespace ModbusHelper {

QByteArray createWriteSingleRegisterRequest(int address, quint16 value) {
    QByteArray data;

    // 地址（大端字节序）
    quint16 beAddr = qToBigEndian(static_cast<quint16>(address));
    data.append(reinterpret_cast<const char*>(&beAddr), sizeof(beAddr));

    // 值（大端字节序）
    quint16 beValue = qToBigEndian(value);
    data.append(reinterpret_cast<const char*>(&beValue), sizeof(beValue));

    return data;
}

QByteArray createWriteSingleCoilRequest(int address, bool on) {
    QByteArray data;

    // 地址（大端字节序）
    quint16 beAddr = qToBigEndian(static_cast<quint16>(address));
    data.append(reinterpret_cast<const char*>(&beAddr), sizeof(beAddr));

    // 值（ON: 0xFF00, OFF: 0x0000）
    quint16 beValue = on ? qToBigEndian(static_cast<quint16>(0xFF00))
                        : qToBigEndian(static_cast<quint16>(0x0000));
    data.append(reinterpret_cast<const char*>(&beValue), sizeof(beValue));

    return data;
}

QByteArray createReadHoldingRegistersRequest(int startAddress, quint16 count) {
    QByteArray data;

    // 起始地址（大端字节序）
    quint16 beAddr = qToBigEndian(static_cast<quint16>(startAddress));
    data.append(reinterpret_cast<const char*>(&beAddr), sizeof(beAddr));

    // 寄存器数量（大端字节序）
    quint16 beCount = qToBigEndian(count);
    data.append(reinterpret_cast<const char*>(&beCount), sizeof(beCount));

    return data;
}

QByteArray createWriteMultipleRegistersRequest(int startAddress, const QVector<quint16> &values) {
    QByteArray data;

    // 起始地址（大端字节序）
    quint16 beAddr = qToBigEndian(static_cast<quint16>(startAddress));
    data.append(reinterpret_cast<const char*>(&beAddr), sizeof(beAddr));

    // 寄存器数量（大端字节序）
    quint16 beCount = qToBigEndian(static_cast<quint16>(values.size()));
    data.append(reinterpret_cast<const char*>(&beCount), sizeof(beCount));

    // 字节计数（寄存器数 * 2）
    quint8 byteCount = static_cast<quint8>(values.size() * 2);
    data.append(static_cast<char>(byteCount));

    // 值（大端字节序）
    for (quint16 value : values) {
        quint16 beValue = qToBigEndian(value);
        data.append(reinterpret_cast<const char*>(&beValue), sizeof(beValue));
    }

    return data;
}

QByteArray createReadCoilsRequest(int startAddress, quint16 count) {
    QByteArray data;

    // 起始地址（大端字节序）
    quint16 beAddr = qToBigEndian(static_cast<quint16>(startAddress));
    data.append(reinterpret_cast<const char*>(&beAddr), sizeof(beAddr));

    // 线圈数量（大端字节序）
    quint16 beCount = qToBigEndian(count);
    data.append(reinterpret_cast<const char*>(&beCount), sizeof(beCount));

    return data;
}

} // namespace ModbusHelper

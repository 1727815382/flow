#include "modbushelper.h"

// ModbusHelper.cpp �ļ�����
#include "ModbusHelper.h"

namespace ModbusHelper {

QByteArray createWriteSingleRegisterRequest(int address, quint16 value) {
    QByteArray data;

    // ��ַ������ֽ���
    quint16 beAddr = qToBigEndian(static_cast<quint16>(address));
    data.append(reinterpret_cast<const char*>(&beAddr), sizeof(beAddr));

    // ֵ������ֽ���
    quint16 beValue = qToBigEndian(value);
    data.append(reinterpret_cast<const char*>(&beValue), sizeof(beValue));

    return data;
}

QByteArray createWriteSingleCoilRequest(int address, bool on) {
    QByteArray data;

    // ��ַ������ֽ���
    quint16 beAddr = qToBigEndian(static_cast<quint16>(address));
    data.append(reinterpret_cast<const char*>(&beAddr), sizeof(beAddr));

    // ֵ��ON: 0xFF00, OFF: 0x0000��
    quint16 beValue = on ? qToBigEndian(static_cast<quint16>(0xFF00))
                        : qToBigEndian(static_cast<quint16>(0x0000));
    data.append(reinterpret_cast<const char*>(&beValue), sizeof(beValue));

    return data;
}

QByteArray createReadHoldingRegistersRequest(int startAddress, quint16 count) {
    QByteArray data;

    // ��ʼ��ַ������ֽ���
    quint16 beAddr = qToBigEndian(static_cast<quint16>(startAddress));
    data.append(reinterpret_cast<const char*>(&beAddr), sizeof(beAddr));

    // �Ĵ�������������ֽ���
    quint16 beCount = qToBigEndian(count);
    data.append(reinterpret_cast<const char*>(&beCount), sizeof(beCount));

    return data;
}

QByteArray createWriteMultipleRegistersRequest(int startAddress, const QVector<quint16> &values) {
    QByteArray data;

    // ��ʼ��ַ������ֽ���
    quint16 beAddr = qToBigEndian(static_cast<quint16>(startAddress));
    data.append(reinterpret_cast<const char*>(&beAddr), sizeof(beAddr));

    // �Ĵ�������������ֽ���
    quint16 beCount = qToBigEndian(static_cast<quint16>(values.size()));
    data.append(reinterpret_cast<const char*>(&beCount), sizeof(beCount));

    // �ֽڼ������Ĵ����� * 2��
    quint8 byteCount = static_cast<quint8>(values.size() * 2);
    data.append(static_cast<char>(byteCount));

    // ֵ������ֽ���
    for (quint16 value : values) {
        quint16 beValue = qToBigEndian(value);
        data.append(reinterpret_cast<const char*>(&beValue), sizeof(beValue));
    }

    return data;
}

QByteArray createReadCoilsRequest(int startAddress, quint16 count) {
    QByteArray data;

    // ��ʼ��ַ������ֽ���
    quint16 beAddr = qToBigEndian(static_cast<quint16>(startAddress));
    data.append(reinterpret_cast<const char*>(&beAddr), sizeof(beAddr));

    // ��Ȧ����������ֽ���
    quint16 beCount = qToBigEndian(count);
    data.append(reinterpret_cast<const char*>(&beCount), sizeof(beCount));

    return data;
}

} // namespace ModbusHelper

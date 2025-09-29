#pragma once
#include <QByteArray>
#include <QtGlobal>

struct FieldCodec {
    static quint8  readU8 (const QByteArray& b, int off);
    static quint16 readU16LE(const QByteArray& b, int off);
    static quint16 readU16BE(const QByteArray& b, int off);
    static quint32 readU32LE(const QByteArray& b, int off);
    static quint32 readU32BE(const QByteArray& b, int off);

    static void writeU8 (QByteArray& b, quint8 v);
    static void writeU16LE(QByteArray& b, quint16 v);
    static void writeU16BE(QByteArray& b, quint16 v);
    static void writeU32LE(QByteArray& b, quint32 v);
    static void writeU32BE(QByteArray& b, quint32 v);
    static QByteArray readBytes(const QByteArray& b, int off, int len);
    static void writeBytes(QByteArray& b, const QByteArray& v);

    // 校验族
    static quint16 crc16_modbus(const QByteArray& b, int from, int to /* -1:到末尾 */);
    static quint16 crc16_ccitt(const QByteArray& b, int from, int to);
    static quint32 crc32_ieee(const QByteArray& b, int from, int to);
    static quint8  sum8(const QByteArray& b, int from, int to);
    static quint8  xor8(const QByteArray& b, int from, int to);
};

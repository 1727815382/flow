#include "modules/proto_engine/field_codec.hpp"

static int normTo(int to, int size){ return (to<0 || to>size)? size : to; }

quint8  FieldCodec::readU8(const QByteArray& b, int off){ return (off>=0 && off<b.size())? (quint8)(uchar)b[off] : 0; }
quint16 FieldCodec::readU16LE(const QByteArray& b, int off){
    if (off+1>=b.size()) return 0; return (quint16)((quint8)b[off] | ((quint16)(quint8)b[off+1]<<8));
}
quint16 FieldCodec::readU16BE(const QByteArray& b, int off){
    if (off+1>=b.size()) return 0; return (quint16)(((quint16)(quint8)b[off]<<8) | (quint8)b[off+1]);
}
quint32 FieldCodec::readU32LE(const QByteArray& b, int off){
    if (off+3>=b.size()) return 0;
    return (quint32)((quint8)b[off] | ((quint32)(quint8)b[off+1]<<8) |
            ((quint32)(quint8)b[off+2]<<16) | ((quint32)(quint8)b[off+3]<<24));
}
quint32 FieldCodec::readU32BE(const QByteArray& b, int off){
    if (off+3>=b.size()) return 0;
    return (quint32)(((quint32)(quint8)b[off]<<24) | ((quint32)(quint8)b[off+1]<<16) |
            ((quint32)(quint8)b[off+2]<<8) | (quint8)b[off+3]);
}

void FieldCodec::writeU8(QByteArray& b, quint8 v){ b.append((char)v); }
void FieldCodec::writeU16LE(QByteArray& b, quint16 v){ b.append((char)(v&0xFF)); b.append((char)((v>>8)&0xFF)); }
void FieldCodec::writeU16BE(QByteArray& b, quint16 v){ b.append((char)((v>>8)&0xFF)); b.append((char)(v&0xFF)); }
void FieldCodec::writeU32LE(QByteArray& b, quint32 v){
    b.append((char)(v&0xFF)); b.append((char)((v>>8)&0xFF)); b.append((char)((v>>16)&0xFF)); b.append((char)((v>>24)&0xFF));
}
void FieldCodec::writeU32BE(QByteArray& b, quint32 v){
    b.append((char)((v>>24)&0xFF)); b.append((char)((v>>16)&0xFF)); b.append((char)((v>>8)&0xFF)); b.append((char)(v&0xFF));
}
QByteArray FieldCodec::readBytes(const QByteArray& b, int off, int len){
    if (off<0 || off>=b.size() || len<0) return QByteArray();
    int to = qMin(off+len, b.size());
    return b.mid(off, to-off);
}
void FieldCodec::writeBytes(QByteArray& b, const QByteArray& v){ b.append(v); }

// CRC 与校验
quint16 FieldCodec::crc16_modbus(const QByteArray& b, int from, int to){
    quint16 crc=0xFFFF; int end=normTo(to, b.size());
    for (int i=qMax(0,from); i<end; ++i){
        crc ^= (quint8)b[i];
        for (int j=0;j<8;++j) crc = (crc & 1) ? (crc>>1) ^ 0xA001 : (crc>>1);
    }
    return crc;
}
quint16 FieldCodec::crc16_ccitt(const QByteArray& b, int from, int to){
    quint16 crc=0xFFFF; int end=normTo(to, b.size());
    for (int i=qMax(0,from); i<end; ++i){
        crc ^= ((quint16)(quint8)b[i])<<8;
        for (int j=0;j<8;++j) crc = (crc & 0x8000) ? (crc<<1) ^ 0x1021 : (crc<<1);
    }
    return crc;
}
quint32 FieldCodec::crc32_ieee(const QByteArray& b, int from, int to){
    quint32 crc = 0xFFFFFFFF; int end=normTo(to, b.size());
    for (int i=qMax(0,from); i<end; ++i){
        crc ^= (quint8)b[i];
        for (int j=0;j<8;++j) crc = (crc & 1) ? (crc>>1) ^ 0xEDB88320u : (crc>>1);
    }
    return ~crc;
}
quint8 FieldCodec::sum8(const QByteArray& b, int from, int to){
    int end=normTo(to, b.size()); quint32 s=0;
    for (int i=qMax(0,from); i<end; ++i) s += (quint8)b[i];
    return (quint8)(s & 0xFF);
}
quint8 FieldCodec::xor8(const QByteArray& b, int from, int to){
    int end=normTo(to, b.size()); quint8 x=0;
    for (int i=qMax(0,from); i<end; ++i) x ^= (quint8)b[i];
    return x;
}

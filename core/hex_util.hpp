// core/hex_utils.hpp
#pragma once
#include <QString>
#include <QByteArray>

inline QByteArray parseHexBytes(const QString& s){
    // 去空白
    QString h; h.reserve(s.size());
    for (int i=0;i<s.size();++i){ QChar c=s[i]; if (!c.isSpace()) h.append(c); }
    // 偶数字符校正
    if ((h.size() & 1) == 1) h.prepend('0');
    QByteArray out; out.reserve(h.size()/2);
    for (int i=0;i+1<h.size(); i+=2){
        bool ok=false; uchar v=(uchar)h.mid(i,2).toUInt(&ok,16);
        out.append((char)(ok? v : 0x00));
    }
    return out;
}

inline QString toHexString(const QByteArray& ba, bool withSpaces){
    static const char* H="0123456789ABCDEF";
    QString s; s.reserve(ba.size()*3);
    for (int i=0;i<ba.size();++i){
        uchar v=(uchar)ba[i];
        s.append(H[(v>>4)&0xF]); s.append(H[v&0xF]);
        if (withSpaces && i+1<ba.size()) s.append(' ');
    }
    return s;
}

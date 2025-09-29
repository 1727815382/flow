#pragma once
#include <QObject>
#include <QVector>
#include <QJsonObject>
#include <QDateTime>
#include <QtGlobal>

// ================= 协议结构：不改动 =================
#pragma pack(push,1)
struct PlcCtrl {
    short _id;      // PLC ID
    short _cmd;     // PLC 控制命令（如 使能）
    short _bat[18];
}; // 40B = 20 regs

struct PlcResponse {
    short _id;          // PLC ID
    short _status;      // 1-正常，0-故障
    short _slaveNum;    // 当前从机数量-12
    short _errorCode;   // PLC故障标识
    short _switchNum;   // 断面选择：0未选，1~4
    short _warningClear;// 面板报警清除：按下1
    short _clean;       // 面板归0：按下1
    short _h1;          // 面板H1： 1左高程（1正升2反）
    short _h2;          // 面板H2： 0中线，1左，2右
    short _h3;          // 面板H3： 2右高程
    short _slaveFlg;    // bit0~16: 设备在位标志
    int   _index;       // 本地字段（不参与modbus读写）
    short _bat[7];
};
#pragma pack(pop)

// ================ 编解码工具（16位寄存器序列） ================
namespace plc_codec {

inline QVector<quint16> encodePlcCtrl(const PlcCtrl& x) {
    QVector<quint16> r; r.reserve(20);
    r << quint16(x._id) << quint16(x._cmd);
    for (int i=0;i<18;i++) r << quint16(x._bat[i]);
    return r;
}

inline bool decodePlcResponse(const QVector<quint16>& regs, PlcResponse& out, int localIndex=-1) {
    if (regs.size() < 20) return false; // 20 个寄存器=40字节
    int i=0;
    out._id           = qint16(regs[i++]);
    out._status       = qint16(regs[i++]);
    out._slaveNum     = qint16(regs[i++]);
    out._errorCode    = qint16(regs[i++]);
    out._switchNum    = qint16(regs[i++]);
    out._warningClear = qint16(regs[i++]);
    out._clean        = qint16(regs[i++]);
    out._h1           = qint16(regs[i++]);
    out._h2           = qint16(regs[i++]);
    out._h3           = qint16(regs[i++]);
    out._slaveFlg     = qint16(regs[i++]);
    out._index        = localIndex; // 本地，不从寄存器读
    for (int j=0;j<7;j++) out._bat[j] = (i<regs.size()? qint16(regs[i++]):0);
    return true;
}

inline QJsonObject toJson(const PlcResponse& r){
    return QJsonObject{
        {"plc_id", r._id},
        {"status_ok", r._status==1},
        {"slave_num", r._slaveNum},
        {"error_code", r._errorCode},
        {"switch_num", r._switchNum},
        {"panel_warning_clear", r._warningClear==1},
        {"panel_clean", r._clean==1},
        {"panel_h1", r._h1},
        {"panel_h2", r._h2},
        {"panel_h3", r._h3},
        {"slave_flags", r._slaveFlg}
    };
}

} // namespace plc_codec

// ================ 设备类（协议不变；仅封装读写与解码） ================
class PlcDevice : public QObject {
    Q_OBJECT
public:
    explicit PlcDevice(int unitId,
                       int ctrlAddr, int ctrlRegs,
                       int respAddr, int respRegs,
                       QObject* parent=nullptr)
        : QObject(parent),
          _unitId(unitId),
          _addrCtrl(ctrlAddr), _lenCtrl(ctrlRegs),
          _addrResp(respAddr), _lenResp(respRegs) {}

    bool writeCtrl(const PlcCtrl& c) {
        auto regs = plc_codec::encodePlcCtrl(c);
        return writeRegs(_unitId, _addrCtrl, regs);
    }

    bool readResponse(int localIndex=-1) {
        _pendingIndex = localIndex;
        return readRegs(_unitId, _addrResp, _lenResp);
    }

    // 由集成方注入 Modbus 读写
    std::function<bool(int,int,const QVector<quint16>&)> writeRegs =
        [](int, int, const QVector<quint16>&){ return false; };
    std::function<bool(int,int,int)> readRegs =
        [](int, int, int){ return false; };

signals:
    void responseDecoded(const PlcResponse& r, QJsonObject json);
    void ioError(const QString& where, const QString& msg);

public slots:
    // 底层读完成后回调该槽，交给本类解码
    void onReadHoldingRegistersDone(int unitId, int addr, const QVector<quint16>& regs) {
        if (unitId!=_unitId || addr!=_addrResp) return;
        PlcResponse r{};
        if(!plc_codec::decodePlcResponse(regs, r, _pendingIndex)) {
            emit ioError("plc.decode", "decodePlcResponse failed (size)");
            return;
        }
        emit responseDecoded(r, plc_codec::toJson(r));
    }

private:
    int _unitId;
    int _addrCtrl, _lenCtrl;
    int _addrResp, _lenResp;
    int _pendingIndex{-1};
};

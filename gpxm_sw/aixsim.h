#pragma once
#include <QtMath>
#include <cstdint>
#include <QString>

//#define ON_PAD

enum PlcStatus
{
    PLC_STATUS_ERROR = 0,
    PLC_STATUS_NORMAL = 1,
};

enum MotorStatus
{
//    0.空闲//1.正转//2.反转//4.归0  故障码100及以后
    MOTOR_STATUS_IDEL = 0,
    MOTOR_STATUS_Ford,
    MOTOR_STATUS_BACK,
    MOTOR_STATUS_ZERO,
    MOTOR_STATUS_ERROR = 100,
};

struct PlcCtrl
{
    short _id;//PLC ID
    short _cmd;//PLC控制命令 plc 使能
    short _bat[18];
}; //40

//plc 4000+40 ,请求这块可以拿到plc状态。一个地址16位，占20字节
struct PlcResponse
{
    short _id;//PLC ID-192.168.6.101-200
    short _status;//PLC运行状态 //1-正常，0-故障
    short _slaveNum;//当前从机数量-12
    short _errorCode;//PLC故障标识
    short _switchNum;//（断面选择）手动旋钮，0-未选择，1~4
    short _warningClear;//PLC面板状态（报警清除）按下是1，默认为0
    short _clean;//PLC面板状态（归0）按下是1，默认为0
    short _h1;//PLC面板状态（H1） 1-左高程，1正升2反
    short _h2;//PLC面板状态（H2） 0-中线，1左，2佑
    short _h3;//PLC面板状态（H3） 2-右高程
    short _slaveFlg;// bit 0~16  bit0： 0 无 1：plc
                           //                bit1： 0 无 1：电机1
                           //                bit1： 0 无 1：电机1
                           //                bit1： 0 无 1：电机1
                           //                bit1： 0 无 1：电机1
                           //                bit1： 0 无 1：电机1
                           //                bit1： 0 无 1：电机1
                           //                bit1： 0 无 1：电机1
                           //                bit1： 0 无 1：电机1
                           //                bit1： 0 无 1：电机1
    int _index;
    short _bat[7];
};

//请求电机状态获取状态
struct MotorResponse
{
    short _id;
    short _status;//电机状态,  0.空闲//1.正转//2.反转//4.归0  5.设置参考点成功 6.设置收腿位置成功   故障码100及以后
    short _positionH;//电机位置反馈，正负
    short _positionL;//电机位置反馈
    short _speedH;//电机速度反馈
    short _speedL;//电机速度反馈
    short _moment;//电机压力反馈//
    short _setingStatus; //状态设置进度 //  4.设置参考点成功 7.设置收腿位置成功
    int _index;//下标，自己添加
    short _bat[10];
};

struct MotorCtrl //电机控制
{
    short _id;//电机ID 2020-2021
    short _urgenStop;//应急停止 2022-2023   1-停止
    short _cmd;//电机控制			0.空闲//1.正转启动//2.反转启动//3.停止//4.归0 2024-2025
    short _targetPosH;//电机位置设置 26-27 *0.01  -900000正负
    short _targetPosL;
    short _targetSpeedH;//电机速度设置 ，0-2000转
    short _targetSpeedL;
    short _zero;//电机归0			 1:0点设置
    short _zeroNumH;//0点值，正负
    short _zeroNumL;//0点值
    short _bat[10];
};

struct PlcFullData
{
    PlcResponse _plcStatus;
    MotorResponse _motorStatus[12];
    QString _ip;
    int _port;
    bool _connect;
};

enum MotorCtrlCMD
{
    Motor_Ctrl_IDEL = 0,
    Motor_Ctrl_FWD = 1,
    Motor_Ctrl_TURN,
    Motor_Ctrl_STOP,
    Motor_Ctrl_ZERO,
    Motor_Ctrl_Urgen,
    Motor_Ctrl_Folded_POS_Set,
    Motor_Ctrl_Folded, //支腿回收
    Motor_Ctrl_FoldDo, //支腿压实
};

enum MotorSettingStatus //工作设置状态，区分是否设置成功参考点和支腿回收点
{
    Motor_Setting_Idel = 0, //默认无
    Motor_Setting_Zero_Success = 1, //参考点设置成功
    Motor_Setting_Zero_Fail, //失败
    Motor_Setting_Fold_Success,
    Motor_Setting_Fold_Fail, //
};

enum PlcCtrlCMD
{
    Plc_Ctrl_WarnRet = 5, //报警复位
};

struct AxisSim
{
    qint64 pos = 0;
    int    rpm = 0;
    int    target = 0;
    quint16 ctrl = 0;           // 0 STOP 1 FWD 2 REV 3 HOME
    bool homing = false;
    bool bat;

    void step(int ms)
    {
        constexpr double ppr = 1000.0;          // 每转脉冲
        // 1) 根据目标转速加减速
        if(ctrl==1||ctrl==2)
        {
            int step = target > rpm ? 100 : (target <rpm ? -100 : 0);
            rpm = qBound(0, rpm + step, 3000);
        }

        else if(ctrl==0)
        {
            rpm = qMax(0, rpm-200);             // 快速减速到 0
        }
        // 2) 计算位移
        double rev = rpm/60.0*ms/1000.0;
        qint64 pulse = qint64(qRound(rev*ppr)) * (ctrl==2?-1:1);
        if(homing)
        {                             // HOME → 回到 0
            if(qAbs(pos) < qAbs(pulse))
            {
                pos=0; homing=false; ctrl=0; rpm=0;
            }

            else
                pos -= pulse;
        }

        else
            pos += pulse;
    }
};

struct CarCtrl
{
    int8_t _head1;
    int8_t _head2;
    int8_t _head3;
    int8_t _head4;
    int8_t _id;
    int8_t _cmd; //逐枕测量
    int8_t _fwd;//方向
    int8_t _count;//测量次数
    int8_t _bat[5];
};


struct TrackMesure //测量值
{
    int _serialNumber;        // 序号
    int _id;                 //id
    double _mileage;             //里程
    double _lRailElevationn;     //左轨高程6
    double _cLinePlann;          //中线平面
    double _rRailElevationn;     //右轨高程
    double _supDeviationn;       //超高偏差
    double _guiDeviationn;       //轨距偏差
    double _lGuiAdjustmentn;     //左轨调整值
    double _cLinedjustmentn;     //中线调整值
    double _rGuiAdjustmentn;     //右轨调整值
};

struct TrackPanel
{
    int _serialNumber;
    int _id;
    double _d1;
    double _d2;
    double _d;

    int _trackNum; //轨排数量
    double _heightDis; //高程行程
    double _midDis; //中线行程
    double _trackHeightCha; //轨枕高差
    double _motorXi; //电机系数

    int _dealTime; //精调时间

    double _a;
    double _b;
    double _c;

    double _heightLMaxCha;//高程L最大偏差
    double _midMaxCha; //中线最大偏差
    double _heightRMaxCha; //高程R最大偏差
    double _heightDealValue; //高程调整值
    double _midDealValue; //中线调整值
    double _heightDealXi; //高程调整加权系数
    double _midDealXi; //中线调整加权系数
    double _midIdemnity; //中线正反补偿


    double _leftTrackHeight; //左枕高度
    double _rightTrackHeight; //右枕高度
    double _leftTrackHeightCha; //左枕高差
    double _rightTrackHeightCha; //右枕高差


    double _carVar; //牵引车电压
    double _carInt; //牵引车电流
    double _startPos; //施工起始里程
    double _currentPos; //当前施工里程

    double _dayDealLength; //当日精调长度
    double _totalDealLength; //累计精调长度
};

//接收小车数据
struct AssembleBuf
{
    TrackMesure _mesure;
    bool _hasPart1 = false;
    bool _hasPart2 = false;
};

//轨枕调整配置参数
struct ConfigPara
{
    double d1;
    double d2;
    double dd;
    double sll;
    double maxh1;
    double maxs;
    double maxh2;
    double maxhet;
    double maxset;
    double fcset;
    double gzpcset;
    double gctzxs;
    double zxtzxs;
    double motos;
    double zxjtpc;
    double qslc;
    double dqlc;
    double dqcd;
    double ljcd;
    double motozfbc;
}; //40

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
//    0.����//1.��ת//2.��ת//4.��0  ������100���Ժ�
    MOTOR_STATUS_IDEL = 0,
    MOTOR_STATUS_Ford,
    MOTOR_STATUS_BACK,
    MOTOR_STATUS_ZERO,
    MOTOR_STATUS_ERROR = 100,
};

struct PlcCtrl
{
    short _id;//PLC ID
    short _cmd;//PLC�������� plc ʹ��
    short _bat[18];
}; //40

//plc 4000+40 ,�����������õ�plc״̬��һ����ַ16λ��ռ20�ֽ�
struct PlcResponse
{
    short _id;//PLC ID-192.168.6.101-200
    short _status;//PLC����״̬ //1-������0-����
    short _slaveNum;//��ǰ�ӻ�����-12
    short _errorCode;//PLC���ϱ�ʶ
    short _switchNum;//������ѡ���ֶ���ť��0-δѡ��1~4
    short _warningClear;//PLC���״̬�����������������1��Ĭ��Ϊ0
    short _clean;//PLC���״̬����0��������1��Ĭ��Ϊ0
    short _h1;//PLC���״̬��H1�� 1-��̣߳�1����2��
    short _h2;//PLC���״̬��H2�� 0-���ߣ�1��2��
    short _h3;//PLC���״̬��H3�� 2-�Ҹ߳�
    short _slaveFlg;// bit 0~16  bit0�� 0 �� 1��plc
                           //                bit1�� 0 �� 1�����1
                           //                bit1�� 0 �� 1�����1
                           //                bit1�� 0 �� 1�����1
                           //                bit1�� 0 �� 1�����1
                           //                bit1�� 0 �� 1�����1
                           //                bit1�� 0 �� 1�����1
                           //                bit1�� 0 �� 1�����1
                           //                bit1�� 0 �� 1�����1
                           //                bit1�� 0 �� 1�����1
    int _index;
    short _bat[7];
};

//������״̬��ȡ״̬
struct MotorResponse
{
    short _id;
    short _status;//���״̬,  0.����//1.��ת//2.��ת//4.��0  5.���òο���ɹ� 6.��������λ�óɹ�   ������100���Ժ�
    short _positionH;//���λ�÷���������
    short _positionL;//���λ�÷���
    short _speedH;//����ٶȷ���
    short _speedL;//����ٶȷ���
    short _moment;//���ѹ������//
    short _setingStatus; //״̬���ý��� //  4.���òο���ɹ� 7.��������λ�óɹ�
    int _index;//�±꣬�Լ����
    short _bat[10];
};

struct MotorCtrl //�������
{
    short _id;//���ID 2020-2021
    short _urgenStop;//Ӧ��ֹͣ 2022-2023   1-ֹͣ
    short _cmd;//�������			0.����//1.��ת����//2.��ת����//3.ֹͣ//4.��0 2024-2025
    short _targetPosH;//���λ������ 26-27 *0.01  -900000����
    short _targetPosL;
    short _targetSpeedH;//����ٶ����� ��0-2000ת
    short _targetSpeedL;
    short _zero;//�����0			 1:0������
    short _zeroNumH;//0��ֵ������
    short _zeroNumL;//0��ֵ
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
    Motor_Ctrl_Folded, //֧�Ȼ���
    Motor_Ctrl_FoldDo, //֧��ѹʵ
};

enum MotorSettingStatus //��������״̬�������Ƿ����óɹ��ο����֧�Ȼ��յ�
{
    Motor_Setting_Idel = 0, //Ĭ����
    Motor_Setting_Zero_Success = 1, //�ο������óɹ�
    Motor_Setting_Zero_Fail, //ʧ��
    Motor_Setting_Fold_Success,
    Motor_Setting_Fold_Fail, //
};

enum PlcCtrlCMD
{
    Plc_Ctrl_WarnRet = 5, //������λ
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
        constexpr double ppr = 1000.0;          // ÿת����
        // 1) ����Ŀ��ת�ټӼ���
        if(ctrl==1||ctrl==2)
        {
            int step = target > rpm ? 100 : (target <rpm ? -100 : 0);
            rpm = qBound(0, rpm + step, 3000);
        }

        else if(ctrl==0)
        {
            rpm = qMax(0, rpm-200);             // ���ټ��ٵ� 0
        }
        // 2) ����λ��
        double rev = rpm/60.0*ms/1000.0;
        qint64 pulse = qint64(qRound(rev*ppr)) * (ctrl==2?-1:1);
        if(homing)
        {                             // HOME �� �ص� 0
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
    int8_t _cmd; //�������
    int8_t _fwd;//����
    int8_t _count;//��������
    int8_t _bat[5];
};


struct TrackMesure //����ֵ
{
    int _serialNumber;        // ���
    int _id;                 //id
    double _mileage;             //���
    double _lRailElevationn;     //���߳�6
    double _cLinePlann;          //����ƽ��
    double _rRailElevationn;     //�ҹ�߳�
    double _supDeviationn;       //����ƫ��
    double _guiDeviationn;       //���ƫ��
    double _lGuiAdjustmentn;     //������ֵ
    double _cLinedjustmentn;     //���ߵ���ֵ
    double _rGuiAdjustmentn;     //�ҹ����ֵ
};

struct TrackPanel
{
    int _serialNumber;
    int _id;
    double _d1;
    double _d2;
    double _d;

    int _trackNum; //��������
    double _heightDis; //�߳��г�
    double _midDis; //�����г�
    double _trackHeightCha; //����߲�
    double _motorXi; //���ϵ��

    int _dealTime; //����ʱ��

    double _a;
    double _b;
    double _c;

    double _heightLMaxCha;//�߳�L���ƫ��
    double _midMaxCha; //�������ƫ��
    double _heightRMaxCha; //�߳�R���ƫ��
    double _heightDealValue; //�̵߳���ֵ
    double _midDealValue; //���ߵ���ֵ
    double _heightDealXi; //�̵߳�����Ȩϵ��
    double _midDealXi; //���ߵ�����Ȩϵ��
    double _midIdemnity; //������������


    double _leftTrackHeight; //����߶�
    double _rightTrackHeight; //����߶�
    double _leftTrackHeightCha; //����߲�
    double _rightTrackHeightCha; //����߲�


    double _carVar; //ǣ������ѹ
    double _carInt; //ǣ��������
    double _startPos; //ʩ����ʼ���
    double _currentPos; //��ǰʩ�����

    double _dayDealLength; //���վ�������
    double _totalDealLength; //�ۼƾ�������
};

//����С������
struct AssembleBuf
{
    TrackMesure _mesure;
    bool _hasPart1 = false;
    bool _hasPart2 = false;
};

//����������ò���
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

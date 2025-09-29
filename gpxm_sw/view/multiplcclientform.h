// MultiPlcClientForm.h
#pragma once
#include <QWidget>
#include <QtSerialBus/QModbusTcpClient>
#include <QTimer>
#include <QTableWidget>
#include <QPushButton>
#include <QLineEdit>
#include <QSpinBox>
#include <QLabel>
#include "aixsim.h"

const int kMaxPlc = 20;
const int kMotorsPerPlc = 12;

class MultiPlcClientForm : public QWidget
{
    Q_OBJECT
public:
    MultiPlcClientForm(QWidget* parent = 0);
    ~MultiPlcClientForm();
    PlcFullData           m_plcs[kMaxPlc];

private slots:
    void tryConnectAll();      // ������������PLC
    void pollAll();            // ���ڶ�ȡ����PLC����
    void slotPlcStateChanged(int idx, QModbusDevice::State s);
//    void slotReadReady(int idx, int type); // ������ص�
    void onTableCellChanged(int row, int col);
    void onBtnSaveConfig();
    void onBtnLoadConfig();
    // ��ť
    void onBtnFwd();
    void onBtnRev();
    void onBtnStop();
    void onBtnHome();
    void onBtnUrgen();
    void onBtnSetRpm();
    void onBtnEnable ();

    void sendPlcCtrl(int idx, quint16 id, quint16 code);
    void sendMotorCtrl(int idx, quint16 motorId, quint16 code);

    void sendMotorCtrl (int idx);

    void onBtnMotorCtrl();

    void onBtnPlcCtrl();

public: //�ⲿ�ӿ�

    static MultiPlcClientForm* getInstance ();
    void motorReset (int plcIndex, int idx, int id);
    void groupMotorReset (int plcIndex);

private:
    void updateTable();
//    void sendCtrl(int idx, quint16 code);
    void sendRpm(int idx, quint16 rpm);


    QModbusTcpClient*  m_clients[kMaxPlc];
    QTimer             m_connTimer, m_pollTimer;
    QTableWidget*      m_table;
    QSpinBox          *m_plcSpin, *m_motorSpin;
    QLineEdit         *m_plcIdEdit;
    QLineEdit         *m_plcCtrlEdit, *m_motorIdEdit, *m_motorCmdEdit, *m_motorSpeedEdit, * m_motorPosEdit;
    QPushButton       *m_btnPlcCtrl, *m_btnMotorCtrl;
    QLabel            *m_infoLabel;

    QPushButton* m_btnFwd;
    QPushButton* m_btnTurn;
    QPushButton* m_btnZero;
    QPushButton* m_btnStop;
    QPushButton* m_btnUrgen;

    QPushButton* m_btnEnable;
    static MultiPlcClientForm* _sClient;
};


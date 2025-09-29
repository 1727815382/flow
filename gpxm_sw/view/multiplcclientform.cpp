#include "multiplcclientform.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QHeaderView>
#include <QFile>
#include <QTextStream>
#include <QMessageBox>
#include <QDebug>
//plc ctrl pianyi  2000-2040
//dianji ctrl   2014-2080

//4000+40  //gong 502

// 构造函数
MultiPlcClientForm* MultiPlcClientForm::_sClient = NULL;
MultiPlcClientForm::MultiPlcClientForm(QWidget* parent) : QWidget(parent)
{
    for(int i=0; i<kMaxPlc; ++i)
    {
        m_clients[i] = NULL;
    }

#if 0
    //    memset(m_plcs, 0, sizeof(PlcFullData));

    setWindowTitle(QString::fromLocal8Bit("多PLC多电机Modbus客户端"));
    resize(1300, 600);
    for(int i=0; i<kMaxPlc; ++i)
    {
        memset(&m_plcs[i]._plcStatus, 0, sizeof (PlcStatus));
        for (int k = 0; k < kMotorsPerPlc; ++k)
        {
            memset(&m_plcs[i]._motorStatus, 0, sizeof (MotorStatus));
        }

        m_plcs[i]._ip = QString("192.168.6.%1").arg(6+i);
        m_plcs[i]._port = 502;
        m_plcs[i]._connect = false;
        m_clients[i] = new QModbusTcpClient(this);
        connect(m_clients[i], &QModbusTcpClient::stateChanged, [=](QModbusDevice::State s){
            slotPlcStateChanged(i, s);
        });
    }
    // 控制区
    m_plcSpin = new QSpinBox; m_plcSpin->setRange(1, kMaxPlc); m_plcSpin->setValue(1);
    m_plcCtrlEdit = new QLineEdit("1");
    m_btnPlcCtrl = new QPushButton(QString::fromLocal8Bit("PLC控制"));
    m_btnEnable = new QPushButton(QString::fromLocal8Bit(" 使能"));
    m_motorSpin = new QSpinBox;
    m_plcIdEdit = new QLineEdit;
    m_motorSpin->setRange(1, kMotorsPerPlc); m_motorSpin->setValue(1);
    m_motorIdEdit = new QLineEdit("1");
    m_motorCmdEdit = new QLineEdit("1");
    m_motorSpeedEdit = new QLineEdit("1500");
    m_motorPosEdit = new QLineEdit("1000");
    m_btnMotorCtrl = new QPushButton(QString::fromLocal8Bit("电机控制"));


    m_btnFwd = new QPushButton(QString::fromLocal8Bit("正转"));
    m_btnTurn = new QPushButton(QString::fromLocal8Bit("反转"));
    m_btnZero = new QPushButton(QString::fromLocal8Bit("回0"));
    m_btnStop = new QPushButton(QString::fromLocal8Bit("停止"));
    m_btnUrgen = new QPushButton(QString::fromLocal8Bit("急停"));


    m_infoLabel = new QLabel;

    QHBoxLayout* ctrl1 = new QHBoxLayout;
    ctrl1->addWidget(new QLabel(QString::fromLocal8Bit("PLC编号:")));
    ctrl1->addWidget(m_plcSpin);


    ctrl1->addWidget(new QLabel(QString::fromLocal8Bit("PLC ID")));
    ctrl1->addWidget(m_plcIdEdit);

    ctrl1->addWidget(new QLabel(QString::fromLocal8Bit("控制码:")));
    ctrl1->addWidget(m_plcCtrlEdit);
    ctrl1->addWidget(m_btnPlcCtrl);
    ctrl1->addWidget(m_btnEnable);

    QHBoxLayout* ctrl2 = new QHBoxLayout;
    ctrl2->addWidget(new QLabel(QString::fromLocal8Bit("PLC编号:")));
    ctrl2->addWidget(m_plcSpin);
    ctrl2->addWidget(new QLabel(QString::fromLocal8Bit("电机编号:")));
    ctrl2->addWidget(m_motorSpin);
    ctrl2->addWidget(new QLabel(QString::fromLocal8Bit("目标电机ID:")));
    ctrl2->addWidget(m_motorIdEdit);
    ctrl2->addWidget(new QLabel(QString::fromLocal8Bit("指令码:")));
    ctrl2->addWidget(m_motorCmdEdit);
    ctrl2->addWidget(new QLabel(QString::fromLocal8Bit("目标转速:")));
    ctrl2->addWidget(m_motorSpeedEdit);
    ctrl2->addWidget(new QLabel(QString::fromLocal8Bit("目标位置")));
    ctrl2->addWidget(m_motorPosEdit);
    ctrl2->addWidget(m_btnMotorCtrl);
    ctrl2->addWidget(m_btnFwd);
    ctrl2->addWidget(m_btnTurn);
    ctrl2->addWidget(m_btnZero);
    ctrl2->addWidget(m_btnStop);
    ctrl2->addWidget(m_btnUrgen);

    // 状态表格
    m_table = new QTableWidget(kMaxPlc*kMotorsPerPlc, 8, this);
    QStringList headers; headers << QString::fromLocal8Bit("PLC编号") << QString::fromLocal8Bit("PLC ID") <<
                                    QString::fromLocal8Bit("电机编号") << QString::fromLocal8Bit("电机 ID") <<
                                    QString::fromLocal8Bit("PLC状态") << QString::fromLocal8Bit("报警") <<
                                    QString::fromLocal8Bit("模式") << QString::fromLocal8Bit("运行") << QString::fromLocal8Bit("电机状态") << QString::fromLocal8Bit("转速/位置");

    m_table->setHorizontalHeaderLabels(headers);
    m_table->verticalHeader()->setVisible(false);

    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->addLayout(ctrl1);
    mainLayout->addLayout(ctrl2);
    mainLayout->addWidget(m_table);
    mainLayout->addWidget(m_infoLabel);
    setLayout(mainLayout);

    connect(m_btnPlcCtrl, SIGNAL(clicked()), this, SLOT(onBtnPlcCtrl()));
    connect(m_btnMotorCtrl, SIGNAL(clicked()), this, SLOT(onBtnMotorCtrl()));

    connect(m_btnFwd, SIGNAL(clicked()), this, SLOT(onBtnFwd()));
    connect(m_btnTurn, SIGNAL(clicked()), this, SLOT(onBtnRev()));
    connect(m_btnStop, SIGNAL(clicked()), this, SLOT(onBtnStop()));
    connect(m_btnZero, SIGNAL(clicked()), this, SLOT(onBtnHome()));
    connect(m_btnUrgen, SIGNAL(clicked()), this, SLOT(onBtnUrgen()));
    connect(m_btnEnable, SIGNAL(clicked()), this, SLOT(onBtnEnable()));

    connect(&m_connTimer, SIGNAL(timeout()), this, SLOT(tryConnectAll()));
    m_connTimer.start(3000);
    connect(&m_pollTimer, SIGNAL(timeout()), this, SLOT(pollAll()));
    m_pollTimer.start(800);

#endif
}

MultiPlcClientForm::~MultiPlcClientForm()
{
    for(int i=0;i<kMaxPlc;++i)
    {

        if(m_clients[i]&&m_clients[i]->state()==QModbusDevice::ConnectedState)
        {
            m_clients[i]->disconnectDevice();
        }

    }
}

void MultiPlcClientForm::onTableCellChanged(int row, int col)
{
    // 编辑IP或端口时更新到结构体
    if(col==1)
        m_plcs[row]._ip = m_table->item(row, col)->text();
    else if(col==2)
        m_plcs[row]._port = m_table->item(row, col)->text().toInt();
}

void MultiPlcClientForm::tryConnectAll()
{
    for(int i=0;i<kMaxPlc;++i)
    {
        if(m_clients[i]->state()!=QModbusDevice::ConnectedState)
        {
            m_clients[i]->setConnectionParameter(QModbusDevice::NetworkAddressParameter, m_plcs[i]._ip);
            m_clients[i]->setConnectionParameter(QModbusDevice::NetworkPortParameter, m_plcs[i]._port);
            m_clients[i]->setTimeout(1500);
            m_clients[i]->setNumberOfRetries(1);
            m_clients[i]->connectDevice();
        }
    }
}

void MultiPlcClientForm::slotPlcStateChanged(int idx, QModbusDevice::State s)
{
    m_plcs[idx]._connect = (s == QModbusDevice::ConnectedState);
    updateTable();
    if(s == QModbusDevice::ConnectedState)
        m_infoLabel->setText(QString::fromLocal8Bit("PLC %1 连接成功").arg(idx+1));
    else if(s == QModbusDevice::UnconnectedState)
        m_infoLabel->setText(QString::fromLocal8Bit("PLC %1 已断开").arg(idx+1));
}

void MultiPlcClientForm::updateTable()
{
#if 1
    int row = 0;
    for(int plc=0; plc<kMaxPlc; ++plc)
    {
        for(int m=0; m<kMotorsPerPlc; ++m, ++row)
        {
            const PlcResponse& st = m_plcs[plc]._plcStatus;
            const MotorResponse& ms = m_plcs[plc]._motorStatus[m];
            m_table->setItem(row, 0, new QTableWidgetItem(QString::number(plc+1)));
            m_table->setItem(row, 1, new QTableWidgetItem(QString::number(st._id)));
            m_table->setItem(row, 2, new QTableWidgetItem(QString::number(m+1)));
            m_table->setItem(row, 3, new QTableWidgetItem(QString::number(ms._id)));
            m_table->setItem(row, 4, new QTableWidgetItem(QString("0x%1").arg(st._status,0,16)));
            m_table->setItem(row, 5, new QTableWidgetItem(QString::number(st._errorCode)));
            m_table->setItem(row, 6, new QTableWidgetItem(QString::number(st._errorCode)));
            m_table->setItem(row, 7, new QTableWidgetItem(QString::number(st._errorCode)));
            m_table->setItem(row, 8, new QTableWidgetItem(QString("0x%1").arg(ms._status,0,16)));
            m_table->setItem(row, 9, new QTableWidgetItem(QString("R:%1  P:%2").arg(ms._speedH<<16 | ms._speedL ).arg(ms._positionH<<16 | ms._positionL)));
        }
    }
#endif
}


// 保存/加载配置
void MultiPlcClientForm::onBtnSaveConfig()
{
    QFile f("plc_config.txt");
    if(!f.open(QFile::WriteOnly | QFile::Text)) return;
    QTextStream ts(&f);
    for(int i=0;i<kMaxPlc;++i)
        ts << m_plcs[i]._ip << "," << m_plcs[i]._port << "\n";
    f.close();
    m_infoLabel->setText(QString::fromLocal8Bit("配置已保存"));
}

void MultiPlcClientForm::onBtnLoadConfig()
{
    QFile f("plc_config.txt");
    if(!f.open(QFile::ReadOnly | QFile::Text)) return;
    QTextStream ts(&f);
    int i=0; QString line;
    while(ts.readLineInto(&line) && i<kMaxPlc)
    {
        QStringList sl = line.split(",");
        if(sl.size()==2)
        {
            m_plcs[i]._ip = sl[0];
            m_plcs[i]._port = sl[1].toInt();
        }

        ++i;
    }

    updateTable();
    m_infoLabel->setText(QString::fromLocal8Bit("配置已加载"));
    f.close();
}

//// 控制指令
//void MultiPlcClientForm::sendCtrl(int idx, quint16 code)
//{
//    if(idx<0||idx>=kMaxPlc)
//        return;
//    if(m_clients[idx]->state() != QModbusDevice::ConnectedState)
//    {
//        m_infoLabel->setText(QString::fromLocal8Bit("PLC未连接"));
//        return;
//    }

//    QModbusDataUnit u(QModbusDataUnit::HoldingRegisters, 0, 10);
//    u.setValue(0, code);
//    QModbusReply* r = m_clients[idx]->sendWriteRequest(u, 1);
//    if(r)
//        connect(r, &QModbusReply::finished, r, &QModbusReply::deleteLater);
//}

void MultiPlcClientForm::sendRpm(int idx, quint16 rpm)
{
    if(idx<0||idx>=kMaxPlc)
        return;
    if(m_clients[idx]->state() != QModbusDevice::ConnectedState)
    {
        m_infoLabel->setText(QString::fromLocal8Bit("PLC未连接"));
        return;
    }

    QModbusDataUnit u(QModbusDataUnit::HoldingRegisters, 1, 20); // 40002
    u.setValue(0, rpm);
    QModbusReply* r = m_clients[idx]->sendWriteRequest(u, 1);
    if(r)
        connect(r, &QModbusReply::finished, r, &QModbusReply::deleteLater);
}

void MultiPlcClientForm::onBtnFwd()
{
    int motorIdx = m_motorSpin->value() -1;
    quint16 motorId = m_motorIdEdit->text().toUShort();

    sendMotorCtrl(motorIdx, motorId,Motor_Ctrl_FWD);
}

void MultiPlcClientForm::onBtnRev()
{
    int motorIdx = m_motorSpin->value() - 1;
    quint16 motorId = m_motorIdEdit->text().toUShort();

    sendMotorCtrl(motorIdx, motorId,Motor_Ctrl_TURN);
}

void MultiPlcClientForm::onBtnStop()
{
    int motorIdx = m_motorSpin->value() - 1;
    quint16 motorId = m_motorIdEdit->text().toUShort();

    sendMotorCtrl(motorIdx, motorId,Motor_Ctrl_STOP);
}

void MultiPlcClientForm::onBtnHome()
{
    int motorIdx = m_motorSpin->value() - 1;
    quint16 motorId = m_motorIdEdit->text().toUShort();

    sendMotorCtrl(motorIdx, motorId,Motor_Ctrl_ZERO);
}

void MultiPlcClientForm::onBtnUrgen()
{
    int motorIdx = m_motorSpin->value() - 1;
    quint16 motorId = m_motorIdEdit->text().toUShort();

    sendMotorCtrl(motorIdx, motorId,Motor_Ctrl_Urgen);
}

void MultiPlcClientForm::onBtnSetRpm()
{
    //    int idx = m_idxSpin->value()-1;
    //    sendRpm(idx, m_rpmEdit->text().toInt());
}

void MultiPlcClientForm::onBtnEnable()
{
    int plcIdx = m_plcSpin->value() - 1;
    quint16 plcId = m_plcIdEdit->text().toUShort();
    quint16 ctrlCode = m_plcCtrlEdit->text().toUShort();
    sendPlcCtrl(plcIdx, plcId ,1);
}

// 写PLC控制指令（2000+PLC号），如idx=0代表2000
void MultiPlcClientForm::sendPlcCtrl(int idx, quint16 id, quint16 code)
{
    if(idx<0||idx>=kMaxPlc)
        return;
    if(m_clients[idx]->state()!=QModbusDevice::ConnectedState)
        return;
    int plcAddr = 2000; // PLC号作为偏移
    QModbusDataUnit u(QModbusDataUnit::HoldingRegisters, plcAddr, 2);
    u.setValue(0, id);
    u.setValue(1, code);

    QModbusReply* r = m_clients[idx]->sendWriteRequest(u, 1);
    if(r)
        connect(r, &QModbusReply::finished, r, &QModbusReply::deleteLater);
}

// 写电机控制（2080，填目标电机编号），可扩展多个命令
void MultiPlcClientForm::sendMotorCtrl(int idx, quint16 motorId, quint16 code)
{
    if(idx<0)
        return;

    int plcidx = m_plcSpin->value()-1;
    if(m_clients[plcidx]->state()!=QModbusDevice::ConnectedState)
        return;
    int startindex = 2020 + 20*idx;
    QModbusDataUnit u(QModbusDataUnit::HoldingRegisters, startindex, 7);
    //    int16_t motoridLow = motorId &0xFFFF;
    //    int16_t motoridHigh = (motorId >> 16) & 0xFFFF;

    //    u.setValue(0, motoridLow);
    //    u.setValue(1, motoridHigh);

    u.setValue(0, motorId);

    if (code == Motor_Ctrl_Urgen)
    {
        int urgen = 1;
        //        int16_t urgenLow = urgen &0xFFFF;
        //        int16_t urgenHigh = (urgen >> 16) & 0xFFFF;

        //        u.setValue(2,urgenLow);
        //        u.setValue(3,urgenHigh);
        u.setValue(1,urgen);
    }

    else
    {
        //        int urgen = 0;
        //        int16_t urgenLow = urgen &0xFFFF;
        //        int16_t urgenHigh = (urgen >> 16) & 0xFFFF;

        //        u.setValue(2,urgenLow);
        //        u.setValue(3,urgenHigh);
        int urgen = 0;
        u.setValue(1,urgen);

        //        int16_t codeLow = code &0xFFFF;
        //        int16_t codeHigh = (code >> 16) & 0xFFFF;

        //        u.setValue(4,codeLow);
        //        u.setValue(5,codeHigh);

        u.setValue(2, code);
    }

    int position = m_motorPosEdit->text().toInt();
    if (position < 0)
        return;
    //    int16_t posLow = position % 10000;
    //    int16_t posHigh = position / 10000;
    int16_t posLow = (position & 0xFFFF);
    int16_t posHigh = ((position >> 16)& 0xFFFF);

    u.setValue(3,posHigh);
    u.setValue(4,posLow);

    int speed = m_motorSpeedEdit->text().toInt();
    int16_t speedLow = (speed & 0xFFFF);
    int16_t speedHig = (speed >> 16 & 0xFFFF);

    u.setValue(5, speedHig);
    u.setValue(6, speedLow);

    QModbusReply* r = m_clients[plcidx]->sendWriteRequest(u, 1);
    if(r)
        connect(r, &QModbusReply::finished, r, &QModbusReply::deleteLater);
}

void MultiPlcClientForm::sendMotorCtrl(int idx)
{

}

void MultiPlcClientForm::pollAll()
{
    for(int i=0; i<kMaxPlc; ++i)
    {
        if(m_clients[i]->state()!=QModbusDevice::ConnectedState) continue;
        // 1. 读PLC状态区：4000~4039
        QModbusDataUnit req(QModbusDataUnit::HoldingRegisters, 4000, 20);
        QModbusReply* reply = m_clients[i]->sendReadRequest(req, 1);
        if(reply)
        {
            connect(reply, &QModbusReply::finished, [=]()
            {
                if(reply->error()==QModbusDevice::NoError)
                {
                    auto unit = reply->result();
                    qDebug () << "plc values" << unit.values() << "size = " << unit.values().size();
                    PlcResponse& st = m_plcs[i]._plcStatus;
                    st._id  = unit.value(0);
                    st._status   = unit.value(1);
                    //                    st.workingMode = unit.value(2);
                    //                    st.runningFlag = unit.value(3);
                }

                reply->deleteLater();
                updateTable();
            });
        }
        // 2. 每台PLC下所有电机，分别读4040/4080/4120...
        for(int m=0;m<kMotorsPerPlc;++m)
        {
            int base = 4020 + 20 * m;
            QModbusDataUnit reqM(QModbusDataUnit::HoldingRegisters, base, 20);
            QModbusReply* reply2 = m_clients[i]->sendReadRequest(reqM, 1);
            if(reply2)
            {
                connect(reply2, &QModbusReply::finished, [=]()
                {
                    if(reply2->error()==QModbusDevice::NoError)
                    {
                        auto unit = reply2->result();
//                        qDebug () << "motor values = " << unit.values();
                        MotorResponse& ms = m_plcs[i]._motorStatus[m];
                        ms._id           = unit.value(0);
                        ms._status       = unit.value(1);
                        ms._positionH    = unit.value(2);
                        ms._positionL    = unit.value(3);
                        //                        ms.speedFeedback = unit.value(3);
                        //                        ms.position      = unit.value(4);
                    }

                    reply2->deleteLater();
                    updateTable();
                });
            }
        }
    }
}

void MultiPlcClientForm::onBtnPlcCtrl()
{
    int plcIdx = m_plcSpin->value() - 1;
    quint16 ctrlCode = m_plcCtrlEdit->text().toUShort();
    if(plcIdx<0||plcIdx>=kMaxPlc) return;
    if(m_clients[plcIdx]->state()!=QModbusDevice::ConnectedState)
    {
        m_infoLabel->setText(QString::fromLocal8Bit("PLC未连接"));
        return;
    }

    int plcAddr = 2000 + plcIdx;
    QModbusDataUnit u(QModbusDataUnit::HoldingRegisters, plcAddr, 1);
    u.setValue(0, ctrlCode);
    QModbusReply* r = m_clients[plcIdx]->sendWriteRequest(u, 1);
    if(r)
        connect(r, &QModbusReply::finished, r, &QModbusReply::deleteLater);
}

MultiPlcClientForm *MultiPlcClientForm::getInstance()
{
    if (_sClient == NULL)
        _sClient = new MultiPlcClientForm;
    return _sClient;
}

void MultiPlcClientForm::motorReset(int plcIndex, int idx, int id)
{
    if(idx<0)
        return;

    //    int plcidx = m_plcSpin->value()-1;
    if(m_clients[plcIndex]->state()!=QModbusDevice::ConnectedState)
        return;
    int startindex = 2020 + 20*idx;
    QModbusDataUnit u(QModbusDataUnit::HoldingRegisters, startindex, 7);

    u.setValue(0, id);

    int urgen = 0;
    u.setValue(1,urgen);
    u.setValue(2, Motor_Ctrl_ZERO);

    QModbusReply* r = m_clients[plcIndex]->sendWriteRequest(u, 1);
    if(r)
        connect(r, &QModbusReply::finished, r, &QModbusReply::deleteLater);
}

void MultiPlcClientForm::groupMotorReset(int plcIndex)
{
    PlcFullData plcData = this->m_plcs[plcIndex];
    if(m_clients[plcIndex]->state()!=QModbusDevice::ConnectedState)
        return;
    for (int i = 0; i < 12; i++)
    {
#if 0
        MotorStatus motor = plcData._motorStatus[i];
        short status = motor._status;
        short id = motor._id;

        if (status == 0) //空闲  todo 确认是否需要
        {
            this->motorReset(plcIndex,i, id);
        }
#endif
    }
}

void MultiPlcClientForm::onBtnMotorCtrl()
{
    int plcIdx = m_plcSpin->value() - 1;
    int motorIdx = m_motorSpin->value() - 1;
    quint16 motorId = m_motorIdEdit->text().toUShort();
    quint16 ctrlCmd = m_motorCmdEdit->text().toUShort();
    quint16 speedSet = m_motorSpeedEdit->text().toUShort();
    if(plcIdx<0||plcIdx>=kMaxPlc) return;
    if(m_clients[plcIdx]->state()!=QModbusDevice::ConnectedState)
    {
        m_infoLabel->setText("PLC未连接");
        return;
    }
    // 2080+电机编号（如果2080不区分电机，用2080即可，填参数区分即可）
    int startIndex = 2020 + motorIdx * 20;
    QModbusDataUnit u(QModbusDataUnit::HoldingRegisters, startIndex, 3);
    u.setValue(0, motorId);
    u.setValue(1, 0);
    u.setValue(2, ctrlCmd);
    u.setValue(3, speedSet);
    QModbusReply* r = m_clients[plcIdx]->sendWriteRequest(u, 1);
    if(r)
        connect(r, &QModbusReply::finished, r, &QModbusReply::deleteLater);
}

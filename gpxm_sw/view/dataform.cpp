#include "DataForm.h"
#include "ui_DataForm.h"

#include <QKeyEvent>
#include <QTimer>
#include <QStringListIterator>
#include <QMessageBox>

DataForm::DataForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DataForm)
{
    ui->setupUi(this);

    ui->widget->setObjectName("subWidget");
    ui->widget_2->setObjectName("subWidget");
    ui->widget_3->setObjectName("subWidget");
    ui->widget_4->setObjectName("subWidget");
    ui->widget_6->setObjectName("subWidget");
    ui->widget_7->setObjectName("subWidget");
    ui->widget_8->setObjectName("subWidget");
    ui->widget_9->setObjectName("subWidget");
    ui->widget_10->setObjectName("subWidget");
    //    ui->widget_11->setObjectName("subWidget");
    ui->widget_12->setObjectName("subWidget");
    ui->widget_13->setObjectName("subWidget");
    ui->widget_14->setObjectName("subWidget");


    for (int i = 0; i < 30; i++)
    {
        QLabel* plcStatusLab = this->findChild<QLabel*>("StatusLed_"+QString::number(i + 1));
        plcStatusLab->setPixmap(QPixmap(":/res/img/LED3.png"));
    }

    connect (ui->comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(slotPlcSelected(int)));
    connect (ui->comboBox_2, SIGNAL(currentIndexChanged(int)), this, SLOT(slotSectionSelected(int)));

    //默认选中断面1
    ui->tvL1->setStyleSheet("background-color: green;");
    ui->tvS1->setStyleSheet("background-color: green;");
    ui->tvR1->setStyleSheet("background-color: green;");

    ui->doubleSpinBox->setRange(0.0, 9999);

    //    connect (ui->pushButton, SIGNAL(clicked()), this, SLOT (slotResetMotor ()));

    connect (ui->h1d, SIGNAL(clicked()), this, SLOT(slotCtrlClicked()));
    connect (ui->h1u, SIGNAL(clicked()), this, SLOT(slotCtrlClicked()));
    connect (ui->h3d, SIGNAL(clicked()), this, SLOT(slotCtrlClicked()));
    connect (ui->h3u, SIGNAL(clicked()), this, SLOT(slotCtrlClicked()));
    connect (ui->md, SIGNAL(clicked()),  this, SLOT(slotCtrlClicked()));
    connect (ui->mu, SIGNAL(clicked()),  this, SLOT(slotCtrlClicked()));
    connect (ui->zeroSet, SIGNAL(clicked()), this, SLOT(slotCtrlClicked()));
    connect (ui->toZero,  SIGNAL(clicked()), this, SLOT(slotCtrlClicked()));
    connect (ui->warnRet, SIGNAL(clicked()), this, SLOT(slotCtrlClicked()));
    connect (ui->stop,    SIGNAL(clicked()), this, SLOT(slotCtrlClicked()));
    connect (ui->clear,   SIGNAL(clicked()), this, SLOT(slotCtrlClicked()));
    connect (ui->foldDo,  SIGNAL(clicked()), this, SLOT(slotCtrlClicked()));


    {
        int col = ui->comboBox->model()->rowCount();
        for(int i =0; i <col; i++)
        {
            QModelIndex idx = ui->comboBox->model()->index(i,0, ui->comboBox->rootModelIndex());
            ui->comboBox->model()->setData(idx,QSize(0,40),Qt::SizeHintRole);
        }
        //        if(col<=11)
        //        {
        //            ui->groupBox_3->hide();
        //            ui->groupBox_2->hide();
        //        }
        //        else if(col<=21)
        //        {
        //            ui->groupBox_3->hide();
        //        }
    }


    {
        int col = ui->comboBox_2->model()->rowCount();
        for(int i =0; i <col; i++)
        {
            QModelIndex idx = ui->comboBox_2->model()->index(i,0, ui->comboBox_2->rootModelIndex());
            ui->comboBox_2->model()->setData(idx,QSize(0,40),Qt::SizeHintRole);
        }
    }
}

DataForm::~DataForm()
{
    delete ui;
}

void DataForm::keyPressEvent(QKeyEvent *event)
{
    if(event->key() == Qt::Key_F6)
    {
        QFile f("style.css");
        f.open(QIODevice::ReadOnly);
        QString str = f.readAll();
        this->setStyleSheet(str);
        f.close();
    }
}

void DataForm::clearPlc()
{

    {

        this->ui->lineEdit_R1dis->setText("");
        this->ui->lineEdit_R1status->setText("");
        this->ui->lineEdit_R1error->setText("");
    }

    {

        this->ui->lineEdit_S1dis->setText("");
        this->ui->lineEdit_S1status->setText("");
        this->ui->lineEdit_S1error->setText("");
    }

    {

        this->ui->lineEdit_L1dis->setText("");
        this->ui->lineEdit_L1status->setText("");
        this->ui->lineEdit_L1error->setText("");
    }

    {

        this->ui->lineEdit_R2dis->setText("");
        this->ui->lineEdit_R2status->setText("");
        this->ui->lineEdit_R2error->setText("");
    }

    {

        this->ui->lineEdit_S2dis->setText("");
        this->ui->lineEdit_S2status->setText("");
        this->ui->lineEdit_S2error->setText("");

    }


    {

        this->ui->lineEdit_L2dis->setText("");
        this->ui->lineEdit_L2status->setText("");
        this->ui->lineEdit_L2error->setText("");
    }


    {

        this->ui->lineEdit_R3dis->setText("");
        this->ui->lineEdit_R3status->setText("");
        this->ui->lineEdit_R3error->setText("");
    }


    {
        this->ui->lineEdit_S3dis->setText("");
        this->ui->lineEdit_S3status->setText("");
        this->ui->lineEdit_S3error->setText("");

    }


    {
        this->ui->lineEdit_L3dis->setText("");
        this->ui->lineEdit_L3status->setText("");
        this->ui->lineEdit_L3error->setText("");
    }


    {
        this->ui->lineEdit_R4dis->setText("");
        this->ui->lineEdit_R4status->setText("");
        this->ui->lineEdit_R4error->setText("");

    }


    {
        this->ui->lineEdit_S4dis->setText("");
        this->ui->lineEdit_S4status->setText("");
        this->ui->lineEdit_S4error->setText("");

    }


    {
        this->ui->lineEdit_L4dis->setText("");
        this->ui->lineEdit_L4status->setText("");
        this->ui->lineEdit_L4error->setText("");

    }

}

void DataForm::slotPlcSelected(int index)
{
    QString trackName = ui->comboBox->currentText();
    this->ui->GuiNum_lab->setText(trackName);
    this->clearPlc ();

    emit signPlcSelected(index);
}

void DataForm::slotSectionSelected(int index)
{
    switch (index)
    {
    case 0:
    {
        ui->tvL1->setStyleSheet("background-color: green;");
        ui->tvS1->setStyleSheet("background-color: green;");
        ui->tvR1->setStyleSheet("background-color: green;");


        ui->tvL2->setStyleSheet("background-color: yellow;");
        ui->tvS2->setStyleSheet("background-color: yellow;");
        ui->tvR2->setStyleSheet("background-color: yellow;");

        ui->tvL3->setStyleSheet("background-color: yellow;");
        ui->tvS3->setStyleSheet("background-color: yellow;");
        ui->tvR3->setStyleSheet("background-color: yellow;");

        ui->tvL4->setStyleSheet("background-color: yellow;");
        ui->tvS4->setStyleSheet("background-color: yellow;");
        ui->tvR4->setStyleSheet("background-color: yellow;");

        break;
    }

    case 1:
    {
        ui->tvL2->setStyleSheet("background-color: green;");
        ui->tvS2->setStyleSheet("background-color: green;");
        ui->tvR2->setStyleSheet("background-color: green;");

        ui->tvL1->setStyleSheet("background-color: yellow;");
        ui->tvS1->setStyleSheet("background-color: yellow;");
        ui->tvR1->setStyleSheet("background-color: yellow;");

        ui->tvL3->setStyleSheet("background-color: yellow;");
        ui->tvS3->setStyleSheet("background-color: yellow;");
        ui->tvR3->setStyleSheet("background-color: yellow;");

        ui->tvL4->setStyleSheet("background-color: yellow;");
        ui->tvS4->setStyleSheet("background-color: yellow;");
        ui->tvR4->setStyleSheet("background-color: yellow;");

        break;
    }

    case 2:
    {
        ui->tvL3->setStyleSheet("background-color: green;");
        ui->tvS3->setStyleSheet("background-color: green;");
        ui->tvR3->setStyleSheet("background-color: green;");


        ui->tvL1->setStyleSheet("background-color: yellow;");
        ui->tvS1->setStyleSheet("background-color: yellow;");
        ui->tvR1->setStyleSheet("background-color: yellow;");

        ui->tvL2->setStyleSheet("background-color: yellow;");
        ui->tvS2->setStyleSheet("background-color: yellow;");
        ui->tvR2->setStyleSheet("background-color: yellow;");

        ui->tvL4->setStyleSheet("background-color: yellow;");
        ui->tvS4->setStyleSheet("background-color: yellow;");
        ui->tvR4->setStyleSheet("background-color: yellow;");

        break;
    }

    case 3:
    {
        ui->tvL4->setStyleSheet("background-color: green;");
        ui->tvS4->setStyleSheet("background-color: green;");
        ui->tvR4->setStyleSheet("background-color: green;");

        ui->tvL1->setStyleSheet("background-color: yellow;");
        ui->tvS1->setStyleSheet("background-color: yellow;");
        ui->tvR1->setStyleSheet("background-color: yellow;");

        ui->tvL2->setStyleSheet("background-color: yellow;");
        ui->tvS2->setStyleSheet("background-color: yellow;");
        ui->tvR2->setStyleSheet("background-color: yellow;");

        ui->tvL3->setStyleSheet("background-color: yellow;");
        ui->tvS3->setStyleSheet("background-color: yellow;");
        ui->tvR3->setStyleSheet("background-color: yellow;");


        break;
    }


    case 4:
    {
        ui->tvL4->setStyleSheet("background-color: green;");
        ui->tvS4->setStyleSheet("background-color: green;");
        ui->tvR4->setStyleSheet("background-color: green;");

        ui->tvL1->setStyleSheet("background-color: green;");
        ui->tvS1->setStyleSheet("background-color: green;");
        ui->tvR1->setStyleSheet("background-color: green;");

        ui->tvL2->setStyleSheet("background-color: green;");
        ui->tvS2->setStyleSheet("background-color: green;");
        ui->tvR2->setStyleSheet("background-color: green;");

        ui->tvL3->setStyleSheet("background-color: green;");
        ui->tvS3->setStyleSheet("background-color: green;");
        ui->tvR3->setStyleSheet("background-color: green;");
        break;
    }

    default:
        break;
    }

    emit signSectionSelected(index);
}

void DataForm::slotUpdatePlc(PlcResponse &pr)
{
    QLabel* plcStatusLab = this->findChild<QLabel*>("StatusLed_"+QString::number(pr._index + 1));
    if(pr._status == PLC_STATUS_ERROR)
    {
        plcStatusLab->setPixmap(QPixmap(":/res/img/LED3.png"));
    }
    else if(pr._status == PLC_STATUS_NORMAL)
    {
        plcStatusLab->setPixmap(QPixmap(":/res/img/LED4.png"));
    }

    this->update();
}

void DataForm::slotUpdateMotor(MotorResponse &mr)
{
    switch (mr._index)
    {
    case 0:
    {
        double dis = (mr._positionH <<16 |(unsigned short)mr._positionL)/100.0;
        this->ui->lineEdit_L1dis->setText(QString::number(dis));
        this->ui->lineEdit_L1status->setText(getMotorsStatus(mr._status));
        this->ui->lineEdit_L1error->setText(getMotorsErrNo(mr._status));

        break;
    }

    case 1:
    {
        double dis = (mr._positionH <<16 |(unsigned short)mr._positionL)/100.0;
        this->ui->lineEdit_S1dis->setText(QString::number(dis));
        this->ui->lineEdit_S1status->setText(getMotorsStatus(mr._status));
        this->ui->lineEdit_S1error->setText(getMotorsErrNo(mr._status));
        break;
    }

    case 11:
    {
        double dis = (mr._positionH <<16 |(unsigned short)mr._positionL)/100.0;
        this->ui->lineEdit_R1dis->setText(QString::number(dis));
        this->ui->lineEdit_R1status->setText(getMotorsStatus(mr._status));
        this->ui->lineEdit_R1error->setText(getMotorsErrNo(mr._status));
        break;
    }

    case 2:
    {
        double dis = (mr._positionH <<16 |(unsigned short)mr._positionL)/100.0;
        this->ui->lineEdit_L2dis->setText(QString::number(dis));
        this->ui->lineEdit_L2status->setText(getMotorsStatus(mr._status));
        this->ui->lineEdit_L2error->setText(getMotorsErrNo(mr._status));
        break;
    }

    case 3:
    {
        double dis = (mr._positionH <<16 |(unsigned short)mr._positionL)/100.0;
        this->ui->lineEdit_S2dis->setText(QString::number(dis));
        this->ui->lineEdit_S2status->setText(getMotorsStatus(mr._status));
        this->ui->lineEdit_S2error->setText(getMotorsErrNo(mr._status));
        break;
    }

    case 10:
    {
        double dis = (mr._positionH <<16 |(unsigned short)mr._positionL)/100.0;
        this->ui->lineEdit_R2dis->setText(QString::number(dis));
        this->ui->lineEdit_R2status->setText(getMotorsStatus(mr._status));
        this->ui->lineEdit_R2error->setText(getMotorsErrNo(mr._status));
        break;
    }

    case 4:
    {
        double dis = (mr._positionH <<16 |(unsigned short)mr._positionL)/100.0;
        this->ui->lineEdit_L3dis->setText(QString::number(dis));
        this->ui->lineEdit_L3status->setText(getMotorsStatus(mr._status));
        this->ui->lineEdit_L3error->setText(getMotorsErrNo(mr._status));
        break;
    }

    case 5:
    {
        double dis = (mr._positionH <<16 |(unsigned short)mr._positionL)/100.0;
        this->ui->lineEdit_S3dis->setText(QString::number(dis));
        this->ui->lineEdit_S3status->setText(getMotorsStatus(mr._status));
        this->ui->lineEdit_S3error->setText(getMotorsErrNo(mr._status));
        break;
    }

    case 9:
    {
        double dis = (mr._positionH <<16 |(unsigned short)mr._positionL)/100.0;
        this->ui->lineEdit_R3dis->setText(QString::number(dis));
        this->ui->lineEdit_R3status->setText(getMotorsStatus(mr._status));
        this->ui->lineEdit_R3error->setText(getMotorsErrNo(mr._status));
        break;
    }

    case 6:
    {
        double dis = (mr._positionH <<16 |(unsigned short)mr._positionL)/100.0;
        this->ui->lineEdit_L4dis->setText(QString::number(dis));
        this->ui->lineEdit_L4status->setText(getMotorsStatus(mr._status));
        this->ui->lineEdit_L4error->setText(getMotorsErrNo(mr._status));
        break;
    }

    case 7:
    {
        double dis = (mr._positionH <<16 |(unsigned short)mr._positionL)/100.0;
        this->ui->lineEdit_S4dis->setText(QString::number(dis));
        this->ui->lineEdit_S4status->setText(getMotorsStatus(mr._status));
        this->ui->lineEdit_S4error->setText(getMotorsErrNo(mr._status));
        break;
    }

    case 8:
    {
        double dis = (mr._positionH <<16 |(unsigned short)mr._positionL)/100.0;
        this->ui->lineEdit_R4dis->setText(QString::number(dis));
        this->ui->lineEdit_R4status->setText(getMotorsStatus(mr._status));
        this->ui->lineEdit_R4error->setText(getMotorsErrNo(mr._status));
        break;
    }

    default:
        break;
    }

    this->update();
}

void DataForm::slotCtrlClicked()
{
    QObject* src = sender();
    QString name = src->objectName();
    double value = ui->doubleSpinBox->value()*100;


    if (name == "clear")
    {
        QMessageBox msgBox;
        msgBox.setWindowTitle(QString::fromLocal8Bit("轨排收腿"));
        msgBox.setText(QString::fromLocal8Bit("操作不可逆，确认收腿吗？"));

        // 自定义两个按钮
        QPushButton *saveBtn = msgBox.addButton(QString::fromLocal8Bit("确认"), QMessageBox::AcceptRole);
        QPushButton *cancelBtn = msgBox.addButton(QString::fromLocal8Bit("取消"), QMessageBox::RejectRole);

        // 显示弹窗并获取结果
        msgBox.exec();

        // 判断用户点击的按钮
        if (msgBox.clickedButton() == saveBtn)
        {
            emit signCtrlClicked(name,value);
        }

        else if (msgBox.clickedButton() == cancelBtn)
        {
            return;
        }
    }

    else if (name == "toZero")
    {
        QMessageBox msgBox;
        msgBox.setWindowTitle(QString::fromLocal8Bit("回到参考点"));
        msgBox.setText(QString::fromLocal8Bit("操作不可逆，确认回到参考点吗？"));

        // 自定义两个按钮
        QPushButton *saveBtn = msgBox.addButton(QString::fromLocal8Bit("确认"), QMessageBox::AcceptRole);
        QPushButton *cancelBtn = msgBox.addButton(QString::fromLocal8Bit("取消"), QMessageBox::RejectRole);

        // 显示弹窗并获取结果
        msgBox.exec();

        // 判断用户点击的按钮
        if (msgBox.clickedButton() == saveBtn)
        {
            emit signCtrlClicked(name,value);
        }

        else if (msgBox.clickedButton() == cancelBtn)
        {
            return;
        }
    }

    else if (name == "zeroSet")
    {
        QMessageBox msgBox;
        msgBox.setWindowTitle(QString::fromLocal8Bit("设置参考点"));
        msgBox.setText(QString::fromLocal8Bit("操作不可逆，确认设置当前位置为参考点吗？"));

        // 自定义两个按钮
        QPushButton *saveBtn = msgBox.addButton(QString::fromLocal8Bit("确认"), QMessageBox::AcceptRole);
        QPushButton *cancelBtn = msgBox.addButton(QString::fromLocal8Bit("取消"), QMessageBox::RejectRole);

        // 显示弹窗并获取结果
        msgBox.exec();

        // 判断用户点击的按钮
        if (msgBox.clickedButton() == saveBtn)
        {
            emit signCtrlClicked(name,value);
        }

        else if (msgBox.clickedButton() == cancelBtn)
        {
            return;
        }
    }


    else if (name == "foldDo")
    {
        QMessageBox msgBox;
        msgBox.setWindowTitle(QString::fromLocal8Bit("支腿压实"));
        msgBox.setText(QString::fromLocal8Bit("操作不可逆，确认执行支腿压实吗？"));

        // 自定义两个按钮
        QPushButton *saveBtn = msgBox.addButton(QString::fromLocal8Bit("确认"), QMessageBox::AcceptRole);
        QPushButton *cancelBtn = msgBox.addButton(QString::fromLocal8Bit("取消"), QMessageBox::RejectRole);

        // 显示弹窗并获取结果
        msgBox.exec();

        // 判断用户点击的按钮
        if (msgBox.clickedButton() == saveBtn)
        {
            emit signFoldDoClicked(name,value);
        }

        else if (msgBox.clickedButton() == cancelBtn)
        {
            return;
        }
    }

    else
    {
        emit signCtrlClicked(name,value);
    }
}

void DataForm::slotCannotWorkMessage (MotorStatus status)
{
    if (status >= MOTOR_STATUS_ERROR)
    {
        QMessageBox msgBox;
        msgBox.setWindowTitle(QString::fromLocal8Bit("提示"));
        msgBox.setText(QString::fromLocal8Bit("当前操作的电机组存在故障，请先检查状态"));

        // 自定义两个按钮
        QPushButton *saveBtn = msgBox.addButton(QString::fromLocal8Bit("确认"), QMessageBox::AcceptRole);
        QPushButton *cancelBtn = msgBox.addButton(QString::fromLocal8Bit("取消"), QMessageBox::RejectRole);

        // 显示弹窗并获取结果
        msgBox.exec();
    }

    else if (status != MOTOR_STATUS_IDEL)
    {
        QMessageBox msgBox;
        msgBox.setWindowTitle(QString::fromLocal8Bit("提示"));
        msgBox.setText(QString::fromLocal8Bit("当前操作的电机组正在工作，请等待完成后再执行"));

        // 自定义两个按钮
        QPushButton *saveBtn = msgBox.addButton(QString::fromLocal8Bit("确认"), QMessageBox::AcceptRole);
        QPushButton *cancelBtn = msgBox.addButton(QString::fromLocal8Bit("取消"), QMessageBox::RejectRole);

        // 显示弹窗并获取结果
        msgBox.exec();
    }
}

void DataForm::slotMotorFoldDoState(int &state)
{
    static QMessageBox msgBox;
    msgBox.setWindowTitle(QString::fromLocal8Bit("提示"));
    msgBox.setText(QString::fromLocal8Bit("当前操作的电机组正在工作，请等待完成后再执行"));

    // 自定义两个按钮
    QPushButton *saveBtn = msgBox.addButton(QString::fromLocal8Bit("确认"), QMessageBox::AcceptRole);
    QPushButton *cancelBtn = msgBox.addButton(QString::fromLocal8Bit("取消"), QMessageBox::RejectRole);

    if (state == 0)
    {
        saveBtn->setEnabled(false);
        cancelBtn->setEnabled(false);
    }

    else if (state == 1)
    {
        saveBtn->setEnabled(true);
        cancelBtn->setEnabled(true);
    }
    // 显示弹窗并获取结果
    msgBox.exec();

    if (msgBox.clickedButton() == saveBtn)
    {
        return;
    }

    else if (msgBox.clickedButton() == cancelBtn)
    {
        return;
    }
}

void DataForm::slotResetMotor()
{
    //    if (m_MultiPlcClient)
    //    {
    //        int plcindex = ui->comboBox->currentIndex();
    //    }
}

QString DataForm::getMotorsStatus(short value)
{
    QString str = "";
    if(value == 0)
        str = QString::fromLocal8Bit("空闲");
    else if(value == 1)
        str = QString::fromLocal8Bit("正转");
    else if(value == 2)
        str = QString::fromLocal8Bit("反转");
    else if(value == 4)
        str = QString::fromLocal8Bit("归0");
    else if (value > 100)
        str = QString::fromLocal8Bit("故障");
    return str;
}

QString DataForm::getMotorsErrNo(short value)
{
    QString str = "";
    if(value > 100)
        str = QString::number(value);
    return str;
}

void DataForm::slotMotorSettingStatus(MotorSettingStatus settingStatus)
{
    switch (settingStatus)
    {

    case Motor_Setting_Zero_Fail:
        {
            QMessageBox msgBox;
            msgBox.setWindowTitle(QString::fromLocal8Bit("参考点状态"));
            msgBox.setText(QString::fromLocal8Bit("设置参考点失败，请检查设备状态"));

            // 自定义两个按钮
            QPushButton *saveBtn = msgBox.addButton(QString::fromLocal8Bit("确认"), QMessageBox::AcceptRole);
//            QPushButton *cancelBtn = msgBox.addButton(QString::fromLocal8Bit("取消"), QMessageBox::RejectRole);

            // 显示弹窗并获取结果
            msgBox.exec();

            // 判断用户点击的按钮
            if (msgBox.clickedButton() == saveBtn)
            {
            }

            break;
        }

    case Motor_Setting_Zero_Success:
        {
            QMessageBox msgBox;
            msgBox.setWindowTitle(QString::fromLocal8Bit("参考点状态"));
            msgBox.setText(QString::fromLocal8Bit("设置参考点位置成功，请重启轨排设备和程序"));

            // 自定义两个按钮
            QPushButton *saveBtn = msgBox.addButton(QString::fromLocal8Bit("确认"), QMessageBox::AcceptRole);
//            QPushButton *cancelBtn = msgBox.addButton(QString::fromLocal8Bit("取消"), QMessageBox::RejectRole);

            // 显示弹窗并获取结果
            msgBox.exec();

            break;
        }

    default:
        break;
    }
}

void DataForm::slotShowInfo(int state,QString info)
{
    if (state == 0)
        this->ui->textBrowser->append(QString::fromLocal8Bit("<font color = 'red'> %1</font>").arg(info));
    else
        this->ui->textBrowser->append(QString::fromLocal8Bit("<font color = 'green'> %1</font>").arg(info));

    if (ui->textBrowser->document()->lineCount() > 500)
    {
        ui->textBrowser->clear();
    }
}

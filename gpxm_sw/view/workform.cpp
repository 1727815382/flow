#include "WorkForm.h"
#include "ui_WorkForm.h"
#include <QKeyEvent>
#include <QDebug>
#include <QTime>
#include "../aixsim.h"


#pragma execution_character_set("utf-8")

extern ConfigPara m_configPara;

enum ENUM_GP_STATUS
{
    ENUM_OFF, //�ر�
    ENUM_OPEN,//����״̬
    ENUM_WARING,//����״̬
    ENUM_ERROR // ����״̬
};

WorkForm::WorkForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::WorkForm)
{
    ui->setupUi(this);
    QStringList headers;
    //headers << "���" << "ID" << "��̣�m��" << "���߳�\n��mm��" << "����ƽ��\n��mm��" << "�ҹ�߳�\n��mm��" << "����ƫ��\n��mm��" << "���ƫ��\n��mm��" << "������ֵ\n��mm��" << "���ߵ���ֵ\n��mm��" << "�ҹ����ֵ\n��mm��";
    headers << "ID" << "��̣�m��" << "���߳�\n��mm��" << "����ƽ��\n��mm��" << "�ҹ�߳�\n��mm��" << "����ƫ��\n��mm��" << "���ƫ��\n��mm��" << "������ֵ\n��mm��" << "���ߵ���ֵ\n��mm��" << "�ҹ����ֵ\n��mm��";
    //    ui->CurStatusWgt->setHlabel(headers, ui->CurStatusWgt->width());
    //    ui->LastStatusWgt->setHlabel(headers, ui->LastStatusWgt->width());
    ui->CurStatusWgt->setHlabel(headers, 1250);
    ui->LastStatusWgt->setHlabel(headers, 1250);
    ui->widget->setObjectName("subWidget");
    //    ui->widget_2->setObjectName("subWidget");
    //    ui->widget_3->setObjectName("subWidget");
    ui->widget_4->setObjectName("subWidget");


    //    ui->label->setObjectName("subTitleLabel");
    //    ui->label_2->setObjectName("subTitleLabel");
    //    ui->label_13->setObjectName("subTitleLabel");
    //    ui->label_14->setObjectName("subTitleLabel");
    //    ui->label_15->setObjectName("subTitleLabel");
    ui->textEdit_info->setText("");



    {
        int col = ui->comboBox->model()->rowCount();
        for(int i =0; i <col; i++)
        {
            QModelIndex idx = ui->comboBox->model()->index(i,0, ui->comboBox->rootModelIndex());
            ui->comboBox->model()->setData(idx,QSize(0,40),Qt::SizeHintRole);
        }
    }


    {
        int col = ui->comboBox_measureDirection->model()->rowCount();
        for(int i =0; i <col; i++)
        {
            QModelIndex idx = ui->comboBox_measureDirection->model()->index(i,0, ui->comboBox_measureDirection->rootModelIndex());
            ui->comboBox_measureDirection->model()->setData(idx,QSize(0,40),Qt::SizeHintRole);
        }
    }



    _pTimer = new QTimer(this);
    _pTimer->start(1000);

    connect(_pTimer, &QTimer::timeout, this, [=](){
        QTime time;
        QDateTime datetime;
        QString timStr = datetime.currentDateTime().toString("yyyy-MM-dd ");
        timStr.append(time.currentTime().toString("hh:mm:ss"));
        ui->timeLab->setText(timStr);
    });

    //ѡ��ĳһ���б�
    connect(ui->CurStatusWgt, SIGNAL(itemRowClicked(int)), this, SIGNAL(signRowSelected(int)));
    //    modifyRow = -1;

    //    connect (ui->btnSingle, SIGNAL(clicked()), this, SIGNAL(signSingleMesure()));
    //    connect (ui->btnAuto,   SIGNAL(clicked()), this, SIGNAL(signAutoMesure()));
    m_TractorDeviceThread = new QThread();
    m_TractorDevice = new TractorDevice("192.168.1.200");
    m_TractorDevice->SetDevIp("192.168.1.200","192.168.1.202");

    QObject::connect(m_TractorDevice, SIGNAL(SignDevOnLineStatus(bool)), this, SLOT(slotUpdateTractorDevStatus(bool)));
    QObject::connect(m_TractorDevice, SIGNAL(SignDevOnLineStatus2(bool)), this, SLOT(slotUpdateTrackPanelDevStatus(bool)));

    //    m_TractorDevice->moveToThread(m_TractorDeviceThread);
    //    connect(m_TractorDeviceThread,&QThread::started,m_TractorDevice,&TractorDevice::doWork);
    //    m_TractorDeviceThread->start();
    ui->LastStatusWgt->hide();
}

WorkForm::~WorkForm()
{
    delete ui;
}

void WorkForm::slotMesureUpdate(TrackMesure data)
{
    CalAdjustValue(data);
    this->ui->CurStatusWgt->addData(data);
    emit signMenuAddMesure (data);
}

void WorkForm::updataStatus(int index,short status)
{
    QString lableName = QString("StatusLed_%1").arg(index);
    QWidget *childWidget = ui->widget_4->findChild<QWidget*>(lableName);

    switch (status)
    {
    case 0:
        // ��������Ϊ "childWidgetName" ���ӿؼ�
        //        QWidget *childWidget = ui->groupBox->findChild<QWidget*>(lableName);

        if(childWidget)
        {
            // �ҵ����ӿؼ�
            qDebug() << "�ҵ��ӿؼ�:" << childWidget->objectName();
        } else {
            qDebug() << "δ�ҵ�ָ�����Ƶ��ӿؼ�";
        }
        break;
    case 1:

        break;
    case 2:

        break;
    default:
        break;
    }
}

void WorkForm::slotMesureUpdate(int row, TrackMesure &data)
{    
    ui->CurStatusWgt->modifyData (row, data);
}

void WorkForm::slotSelectedMesure(TrackMesure &me)
{
    this->ui->lineEdit_ID->setText(QString::number(me._id));
    this->ui->lineEdit_LC->setText(QString::number(me._mileage));
    this->ui->lineEdit_ZG->setText(QString::number(me._lRailElevationn));
    this->ui->lineEdit_ZX->setText(QString::number(me._cLinePlann));
    this->ui->lineEdit_YG->setText(QString::number(me._rRailElevationn));
    this->ui->lineEdit_CG->setText(QString::number(me._supDeviationn));
    this->ui->lineEdit_GJ->setText(QString::number(me._guiDeviationn));
}

void WorkForm::slotMesureRemoved(int row)
{
    ui->CurStatusWgt->deleteData(row);
}

void WorkForm::slotOldMesure(TrackMesure &me)
{
    ui->LastStatusWgt->addData(me);
}

void WorkForm::slotUpdateTractorDevStatus(bool status)
{
    if(status)
    {
        ui->label_QYC->setText("ǣ����ͨѶ�ɹ�");
        ui->label_QYC->setStyleSheet("color: #00ff00");
    }
    else
    {
        ui->label_QYC->setText("ǣ����ͨѶʧ��");
        ui->label_QYC->setStyleSheet("color: #ff6600");
    }
}

void WorkForm::slotUpdateTrackPanelDevStatus(bool status)
{
    if(status)
    {
        ui->label_GP->setText("����ͨѶ�ɹ�");
        ui->label_GP->setStyleSheet("color: #00ff00");
    }
    else
    {
        ui->label_GP->setText("����ͨѶʧ��");
        ui->label_GP->setStyleSheet("color: #ff6600");
    }
}

void WorkForm::on_pushButton_CJ_add_clicked()
{
    TrackMesure data;
    //    data._serialNumber = this->_wdata.count()+1;                // ���
    data._id = this->ui->lineEdit_ID->text().toDouble();                //id
    data._mileage = this->ui->lineEdit_LC->text().toDouble();           //���
    data._lRailElevationn = this->ui->lineEdit_ZG->text().toDouble();   //���߳�
    data._cLinePlann = this->ui->lineEdit_ZX->text().toDouble();        //����ƽ��
    data._rRailElevationn = this->ui->lineEdit_YG->text().toDouble();   //�ҹ�߳�
    data._supDeviationn = this->ui->lineEdit_CG->text().toDouble();     //����ƫ��
    data._guiDeviationn = this->ui->lineEdit_GJ->text().toDouble();     //���ƫ��
    CalAdjustValue(data);

    this->ui->CurStatusWgt->addData(data);

    emit signMenuAddMesure (data);
}

void WorkForm::on_pushButton_CJ_del_clicked()
{
    emit signMenuDelMesure();
}

void WorkForm::on_pushButton_CJ_modify_clicked()
{
    TrackMesure data;
    data._id = this->ui->lineEdit_ID->text().toDouble();                //id
    data._mileage = this->ui->lineEdit_LC->text().toDouble();           //���
    data._lRailElevationn = this->ui->lineEdit_ZG->text().toDouble();   //���߳�
    data._cLinePlann = this->ui->lineEdit_ZX->text().toDouble();        //����ƽ��
    data._rRailElevationn = this->ui->lineEdit_YG->text().toDouble();   //�ҹ�߳�
    data._supDeviationn = this->ui->lineEdit_CG->text().toDouble();     //����ƫ��
    data._guiDeviationn = this->ui->lineEdit_GJ->text().toDouble();     //���ƫ��

    CalAdjustValue(data);

    emit signMenuMdyMesure(data);
}

void WorkForm::on_pushButton_CJ_clean_clicked()
{
    this->ui->CurStatusWgt->cleanData();
    emit signMenuClrMesure();
}


void WorkForm::on_pushButton_check_clicked()
{
    emit signDataCheck ();

    this->ui->CurStatusWgt->datachack();

}

void WorkForm::on_pushButton_ctrl_deal_clicked(bool enable)
{
    if(enable)
        emit signDealSection (this->ui->comboBox->currentIndex());
    else
        emit signStopDealSection();
}

void WorkForm::on_pushButton_LastData_clicked(bool enable)
{
    if(enable)
    {
        ui->LastStatusWgt->show();
        ui->pushButton_LastData->setText("������һ������>>>");
    }

    else
    {
        ui->LastStatusWgt->hide();
        ui->pushButton_LastData->setText("չʾ��һ������>>>");
    }
}

void WorkForm::on_btnSingle_clicked(bool enable)
{

    if(enable)
        emit signSingleMesure (this->ui->comboBox_measureDirection->currentIndex());
    else
        emit signStopMesure();

}

void WorkForm::on_btnAuto_clicked(bool enable)
{

    if(enable)
        emit signAutoMesure (this->ui->comboBox_measureDirection->currentIndex());
    else
        emit signStopMesure();

}
//�����ļ�
void WorkForm::on_pushButton_saveData_clicked()
{
    this->ui->CurStatusWgt->saveTableToCSV();
}

//��ȡ�ļ�
void WorkForm::on_pushButton_readData_clicked()
{
    emit signMenuClrMesure();
    QList<TrackMesure> mesures = this->ui->CurStatusWgt->loadCSVToTable();

    foreach (TrackMesure mesure, mesures)
    {
        emit signMenuAddMesure(mesure);
    }
}

void WorkForm::CalAdjustValue(TrackMesure& data)
{
    data._lGuiAdjustmentn = data._lRailElevationn - (data._rRailElevationn - data._lRailElevationn)/m_configPara.dd * m_configPara.d1;
    data._rGuiAdjustmentn = data._rRailElevationn - (data._lRailElevationn - data._rRailElevationn)/m_configPara.dd * m_configPara.d2;
    data._cLinedjustmentn = data._cLinePlann;
//    if(data._lGuiAdjustmentn>0.1)
//    {
//        data._lGuiAdjustmentn += 0.4;
//    }
//    else if(data._lGuiAdjustmentn<0.1)
//    {
//         data._lGuiAdjustmentn -= 0.4;
//    }

//    if(data._rGuiAdjustmentn>0.1)
//    {
//        data._rGuiAdjustmentn += 0.4;
//    }
//    else if(data._rGuiAdjustmentn<0.1)
//    {
//         data._rGuiAdjustmentn -= 0.4;
//    }

//    if(data._cLinedjustmentn>0.1)
//    {
//        data._cLinedjustmentn += 0.4;
//    }
//    else if(data._cLinedjustmentn<0.1)
//    {
//         data._cLinedjustmentn -= 0.4;
//    }
}

void WorkForm::slotShowErrInfo(QString info)
{
    this->ui->textEdit_info->append(info);
}

void WorkForm::slotMotorRunOver()
{
    //on_pushButton_ctrl_deal_clicked(false);
    this->ui->pushButton_ctrl_deal->setChecked(false);
}

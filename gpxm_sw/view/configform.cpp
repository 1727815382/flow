#include "ConfigForm.h"
#include "ui_ConfigForm.h"
#include <QDebug>
#include <QKeyEvent>
#include <QMessageBox>
#include "../aixsim.h"

extern ConfigPara m_configPara;

ConfigForm::ConfigForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ConfigForm)
{
    ui->setupUi(this);
    QPixmap pixmap(":/res/img/pic.png");
    if(pixmap.isNull())
    {
        qDebug() << "null";
    }

    ui->picLabel->setPixmap(pixmap);
    ui->picLabel->setScaledContents(true);
    ui->picLabel->setObjectName("picLabel");

    ui->widget->setObjectName("subWidget");
    ui->widget_2->setObjectName("subWidget");

#ifdef ON_PAD
    m_IniFunc = new IniFunc(QString::fromLocal8Bit("参数设置.ini"));
#else
    m_IniFunc = new IniFunc(QString::fromLocal8Bit("E:\\WGZ\\Pad\\gpxm_sw\\参数设置.ini"));
#endif

    connect(ui->btnSave, SIGNAL(clicked()), this, SLOT(slotSave()));


    configSet();
}

ConfigForm::~ConfigForm()
{
    delete ui;
}

void ConfigForm::keyPressEvent(QKeyEvent *event)
{
    if(event->key() == Qt::Key_F6)
    {
        QFile f("E:\\interfacedesign\\demo2\\style.css");
        f.open(QIODevice::ReadOnly);
        QString str = f.readAll();
        this->setStyleSheet(str);
        f.close();
    }
}

void ConfigForm::configSet()
{
    this->ui->D1_Edit->setText(m_IniFunc->getConfigValue("CONFIG PARAMETER","d1"));
    this->ui->D2_Edit->setText(m_IniFunc->getConfigValue("CONFIG PARAMETER","d2"));
    this->ui->D_Edit->setText(m_IniFunc->getConfigValue("CONFIG PARAMETER","dd"));
    this->ui->RailNum_Edit->setText(m_IniFunc->getConfigValue("CONFIG PARAMETER","sll"));
    this->ui->ElevitionL_Edit->setText(m_IniFunc->getConfigValue("CONFIG PARAMETER","maxh1"));
    this->ui->CLineS_Edit->setText(m_IniFunc->getConfigValue("CONFIG PARAMETER","maxs"));
    this->ui->ElevitionR_Edit->setText(m_IniFunc->getConfigValue("CONFIG PARAMETER","maxh2"));
    this->ui->ElevationTravel_Edit->setText(m_IniFunc->getConfigValue("CONFIG PARAMETER","maxhet"));
    this->ui->CLine_Edit->setText(m_IniFunc->getConfigValue("CONFIG PARAMETER","maxset"));

//    this->ui->TuingTime_Edit->setText("50");
//    this->ui->D1_Edit->setText("943.3");
//    this->ui->D2_Edit->setText("943.3");
//    this->ui->D_Edit->setText("1433.4");

    m_configPara.d1 = this->ui->D1_Edit->text().toDouble();
    m_configPara.d2 = this->ui->D2_Edit->text().toDouble();
    m_configPara.dd = this->ui->D_Edit->text().toDouble();
    m_configPara.sll = this->ui->RailNum_Edit->text().toDouble();
    m_configPara.maxh1 = this->ui->ElevitionL_Edit->text().toDouble();
    m_configPara.maxs = this->ui->CLineS_Edit->text().toDouble();
    m_configPara.maxh2 = this->ui->ElevitionR_Edit->text().toDouble();
    m_configPara.maxhet = this->ui->ElevationTravel_Edit->text().toDouble();
    m_configPara.maxset = this->ui->CLine_Edit->text().toDouble();

}

void ConfigForm::slotSave()
{
    m_IniFunc->setConfigValue("CONFIG PARAMETER","d1", this->ui->D1_Edit->text());
    m_IniFunc->setConfigValue("CONFIG PARAMETER","d2", this->ui->D2_Edit->text());
    m_IniFunc->setConfigValue("CONFIG PARAMETER","dd", this->ui->D_Edit->text());
    m_IniFunc->setConfigValue("CONFIG PARAMETER","sll", this->ui->RailNum_Edit->text());
    m_IniFunc->setConfigValue("CONFIG PARAMETER","maxh1", this->ui->ElevitionL_Edit->text());
    m_IniFunc->setConfigValue("CONFIG PARAMETER","maxs", this->ui->CLineS_Edit->text());
    m_IniFunc->setConfigValue("CONFIG PARAMETER","maxh2", this->ui->ElevitionR_Edit->text());
    m_IniFunc->setConfigValue("CONFIG PARAMETER","maxhet", this->ui->ElevationTravel_Edit->text());
    m_IniFunc->setConfigValue("CONFIG PARAMETER","maxset", this->ui->CLine_Edit->text());
    QMessageBox msgBox;
    msgBox.setWindowTitle(QString::fromLocal8Bit("参数保存"));
    msgBox.setText(QString::fromLocal8Bit("    参数配置保存完成！   "));

    // 自定义两个按钮
    QPushButton *saveBtn = msgBox.addButton(QString::fromLocal8Bit("确认"), QMessageBox::AcceptRole);

    // 显示弹窗并获取结果
    msgBox.exec();
}


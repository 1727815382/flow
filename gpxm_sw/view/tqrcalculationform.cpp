#include "TQRCalculationForm.h"
#include "ui_TQRCalculationForm.h"

TQRCalculationForm::TQRCalculationForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TQRCalculationForm)
{
    ui->setupUi(this);
    this->setObjectName("subWidget");
    ui->widget->setObjectName("subWidget");
}

TQRCalculationForm::~TQRCalculationForm()
{
    delete ui;
}

void TQRCalculationForm::CalculationShow(QVector<QVector<double>> &m_td)
{
    double TQI_GJPC = 0,TQI_NQ = 0,TQI_CGPC = 0,LTQI_GXPC = 0,RTQI_GXPC = 0,LTQI_GDPC = 0,RTQI_GDPC = 0;
    for(int i = 0;i<m_td.count();i++)
    {
        TQI_GJPC += m_td.at(i).at(1);//¹ì¾à
        TQI_NQ += m_td.at(i).at(2);//Å¤¾Ø2.5
        TQI_CGPC += m_td.at(i).at(3);//³¬¸ßÆ«²î
        LTQI_GXPC += m_td.at(i).at(4);//¹ìÏò1Æ«²îL
        LTQI_GDPC += m_td.at(i).at(5);//¸ßµÍ1Æ«²îL
        RTQI_GXPC += m_td.at(i).at(6);//¹ìÏò1Æ«²îR
        RTQI_GDPC += m_td.at(i).at(7);//¸ßµÍ1Æ«²îR
    }
    TQI_GJPC  /= m_td.count();
    TQI_NQ    /= m_td.count();
    TQI_CGPC  /= m_td.count();
    LTQI_GXPC /= m_td.count();
    LTQI_GDPC /= m_td.count();
    RTQI_GXPC /= m_td.count();
    RTQI_GDPC /= m_td.count();

    this->ui->lineEdit_TQI_GJPC   ->setText(QString::number(TQI_GJPC ));
    this->ui->lineEdit_TQI_NQ     ->setText(QString::number(TQI_NQ   ));
    this->ui->lineEdit_TQI_CGPC   ->setText(QString::number(TQI_CGPC ));
    this->ui->lineEdit_LTQI_GXPC  ->setText(QString::number(LTQI_GXPC));
    this->ui->lineEdit_LTQI_GDPC  ->setText(QString::number(LTQI_GDPC));
    this->ui->lineEdit_RTQI_GXPC  ->setText(QString::number(RTQI_GXPC));
    this->ui->lineEdit_RTQI_GDPC  ->setText(QString::number(RTQI_GDPC));
}

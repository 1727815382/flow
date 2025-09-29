#include "flow/precisiondataprocessing.h"
#include "TuingForm.h"
#include "ui_TuingForm.h"
#include <QFileDialog>
#include <QUrl>
#include <QAuthenticator>
#include <QFile>
#include <QDir>
#include <QDebug>
#include <QStandardItemModel>
#include <QStandardItem>

QString TuingForm::m_ftpFileName;

TuingForm::TuingForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TuingForm)
{
    ui->setupUi(this);

    //ui->verticalLayout_18->addStretch(10);
    QStringList headers;
    headers << "��̼�" << "���" << "Ť��2.5" << "����ƫ��" << "����1\nƫ��L" << "�ߵ�1\nƫ��L" << "����1\nƫ��R" << "�ߵ�1\nƫ��R" << "����2\nƫ��L" << "�ߵ�2\nƫ��L" << "����2\nƫ��R" << "�ߵ�2\nƫ��R";

    ui->tuing->setHlabel(headers,1850);

#ifdef ON_PAD
    m_localDir = "dataDealSoft\\";
#else
    m_localDir = "E:\\WGZ\\dataDealSoft\\";
#endif
    m_FtpClient = new FtpClient(this);
    m_FtpClient->setWindowFlags(m_FtpClient->windowFlags()|Qt::WindowStaysOnTopHint);
#ifdef ON_PAD
    m_FtpClient->SetDownloadDir("download\\");
#else
    m_FtpClient->SetDownloadDir("E:\\WGZ\\dataDealSoft\\����\\");
#endif
    m_ftpFileName = "111.asc";

//    this->ui->pushButton_downLoad->setEnabled(true);
//    this->ui->pushButton_downLoad->setStyleSheet("background-color: rgb(200, 200, 200);");

    ui->widget->setObjectName("subWidget");
}

TuingForm::~TuingForm()
{
    delete ui;
}

void TuingForm::on_pushButton_29_clicked()
{
    this->ui->pushButton_downLoad->setStyleSheet("background-color: rgb(200, 200, 200);");//��ɫ
    QString fileName = QFileDialog::getOpenFileName(
        nullptr,                // ������
        "ѡ���ļ�",             // �Ի������
        m_localDir,//QDir::homePath(),       // ��ʼĿ¼
        "�ı��ļ� (*.asc);;�����ļ� (*)"  // �ļ�������
    );

    if (!fileName.isEmpty())
    {
        qDebug() << "ѡ����ļ�:" << fileName;
        // �����ﴦ��ѡ�е��ļ�
    }
    else
    {
        qDebug() << "�û�ȡ����ѡ��";
    }

    PrecisionDataProcessing  Processing;
    QVector<QVector<double> >  arrayData = Processing.parseDataFile(fileName);

    //ȥ����һ�б���
    if(!arrayData.isEmpty())
        arrayData.removeFirst();

    ui->tuing->showData(arrayData);    

    ui->widget->CalculationShow(arrayData);
}

void TuingForm::on_pushButton_downLoad_clicked()
{
    this->ui->pushButton_downLoad->setStyleSheet("background-color: rgb(200, 200, 200);");//��ɫ
//    this->ui->pushButton_downLoad->setEnabled(false);
    m_FtpClient->open();
    m_FtpClient->onConnectServer();
    m_FtpClient->exec();
    this->ui->pushButton_downLoad->setStyleSheet("background-color: #5b7cff;");//
//    this->ui->pushButton_downLoad->setEnabled(false);

}



void TuingForm::connectToFtp()
{
    //���ĵ�ftp

}

void TuingForm::ftpCommandFinished(int, bool error)
{

//    if (ftp->currentCommand() == QFtp::ConnectToHost)
//    {
//        if (error)
//        {
//            this->ui->pushButton_downLoad->setEnabled(true);
//            this->ui->pushButton_downLoad->setStyleSheet("background-color: rgb(255, 0, 0);");
//            ftp->close();
//            return;
//        }
//        ftp->login(QUrl::fromPercentEncoding(url.userName().toLatin1()), url.password());
//    }

//    if (ftp->currentCommand() == QFtp::Login)
//    {
//        if (error)
//        {
//            this->ui->pushButton_downLoad->setEnabled(true);
//            this->ui->pushButton_downLoad->setStyleSheet("background-color: rgb(255, 0, 0);");
//            ftp->close();
//            return;
//        }
//        ftp->list();
//    }

//    if (ftp->currentCommand() == QFtp::List)
//    {
//        if (error)
//        {
//            this->ui->pushButton_downLoad->setEnabled(true);
//            this->ui->pushButton_downLoad->setStyleSheet("background-color: rgb(255, 0, 0);");
//            ftp->close();
//            return;
//        }
//        showFileList();

//    }

//    if (ftp->currentCommand() == QFtp::Get)
//    {
//        if (error)
//        {
//            file->close();
//            file->remove();
//            this->ui->pushButton_downLoad->setStyleSheet("background-color: rgb(255, 0, 0);");
//        }
//        else
//        {
//            file->close();
//            this->ui->pushButton_downLoad->setStyleSheet("background-color: rgb(0, 255, 0);");//��ɫ
//        }
//        delete file;
//        ftp->close();
//        this->ui->pushButton_downLoad->setEnabled(true);
//    }
}

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
    headers << "里程计" << "轨距" << "扭矩2.5" << "超高偏差" << "轨向1\n偏差L" << "高低1\n偏差L" << "轨向1\n偏差R" << "高低1\n偏差R" << "轨向2\n偏差L" << "高低2\n偏差L" << "轨向2\n偏差R" << "高低2\n偏差R";

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
    m_FtpClient->SetDownloadDir("E:\\WGZ\\dataDealSoft\\下载\\");
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
    this->ui->pushButton_downLoad->setStyleSheet("background-color: rgb(200, 200, 200);");//灰色
    QString fileName = QFileDialog::getOpenFileName(
        nullptr,                // 父窗口
        "选择文件",             // 对话框标题
        m_localDir,//QDir::homePath(),       // 初始目录
        "文本文件 (*.asc);;所有文件 (*)"  // 文件过滤器
    );

    if (!fileName.isEmpty())
    {
        qDebug() << "选择的文件:" << fileName;
        // 在这里处理选中的文件
    }
    else
    {
        qDebug() << "用户取消了选择";
    }

    PrecisionDataProcessing  Processing;
    QVector<QVector<double> >  arrayData = Processing.parseDataFile(fileName);

    //去除第一行标题
    if(!arrayData.isEmpty())
        arrayData.removeFirst();

    ui->tuing->showData(arrayData);    

    ui->widget->CalculationShow(arrayData);
}

void TuingForm::on_pushButton_downLoad_clicked()
{
    this->ui->pushButton_downLoad->setStyleSheet("background-color: rgb(200, 200, 200);");//灰色
//    this->ui->pushButton_downLoad->setEnabled(false);
    m_FtpClient->open();
    m_FtpClient->onConnectServer();
    m_FtpClient->exec();
    this->ui->pushButton_downLoad->setStyleSheet("background-color: #5b7cff;");//
//    this->ui->pushButton_downLoad->setEnabled(false);

}



void TuingForm::connectToFtp()
{
    //用心的ftp

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
//            this->ui->pushButton_downLoad->setStyleSheet("background-color: rgb(0, 255, 0);");//绿色
//        }
//        delete file;
//        ftp->close();
//        this->ui->pushButton_downLoad->setEnabled(true);
//    }
}

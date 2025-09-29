//#include "ftpclient.h"
//#include "ui_ftpclient.h"

//FtpClient::FtpClient(QDialog *parent)
//    : QDialog(parent)
//    , ui(new Ui::FtpClient)
//{
//    ui->setupUi(this);
//    setWindowTitle(QString::fromLocal8Bit("�����ļ�"));

//    // ��ʼ��FTP����
//    ftp = new QFtp(this);

//    // ����Ĭ������Ŀ¼Ϊ�û���Ŀ¼
//    currentDownloadDir = QDir::homePath();
//    ui->downloadDirEdit->setText(currentDownloadDir);
//    ui->groupBox->hide();

////    url.setScheme("ftp");//���øüƻ�������URL�����ͣ���Э�飩
////    url.setHost("127.0.0.1");//����������ַ

////    url.setPort(21);//����URL�Ķ˿ڡ��ö˿���URL��Ȩ�޵�һ���֣���setAuthority�����������˿ڱ����ǽ���0��65535���������˿�����Ϊ-1��ʾ�ö˿��ǲ�ȷ���ġ�
////    url.setPassword("123"); //����ftp�û�����
////    url.setUserName("123");//����tfp�û���



//    // ��ʼ���ļ����͹�����
//    ui->fileTypeCombo->addItem(QString::fromLocal8Bit("�����ļ�"), "*");
//    ui->fileTypeCombo->addItem(QString::fromLocal8Bit("�ı��ļ�"), "*.txt;*.asc;*.log");
//    ui->fileTypeCombo->addItem(QString::fromLocal8Bit("ͼƬ�ļ�"), "*.jpg;*.jpeg;*.png;*.bmp;*.gif");
//    ui->fileTypeCombo->addItem(QString::fromLocal8Bit("ѹ���ļ�"), "*.zip;*.rar;*.7z;*.tar;*.gz");
//    ui->fileTypeCombo->addItem(QString::fromLocal8Bit("*.asc"), "*.asc;");
//    currentFilter = "*";

//    // ����FTP�ź����
//    connect(ftp, SIGNAL(commandFinished(int, bool)), this, SLOT(on_ftpCommandFinished(int, bool)));
//    connect(ftp, SIGNAL(listInfo(const QUrlInfo &)), this, SLOT(on_ftpListInfo(const QUrlInfo &)));
//    connect(ftp, SIGNAL(dataTransferProgress(qint64, qint64)), this, SLOT(on_ftpDataTransferProgress(qint64, qint64)));
//    connect(ftp, SIGNAL(stateChanged(int)), this, SLOT(on_ftpStateChanged(int)));

//    // ���öϿ����Ӻ����ذ�ť
//    ui->disconnectButton->setEnabled(false);
//    ui->downloadButton->setEnabled(false);
//}

//FtpClient::~FtpClient()
//{
//    on_ExitButton_clicked();
//    delete ui;
//    if (ftp->state() == QFtp::Connected) {
//        ftp->close();
//    }
//    delete ftp;
//}

//void FtpClient::onConnectServer()
//{
//    ui->fileListWidget->clear();
//    on_connectButton_clicked();

//}

//void FtpClient::SetDownloadDir(QString path)
//{
//    currentDownloadDir = path;
//    ui->downloadDirEdit->setText(currentDownloadDir);
//}

//void FtpClient::closeEvent(QCloseEvent *)
//{
//    on_ExitButton_clicked();
//}

//// ���ӵ�FTP������
//void FtpClient::on_connectButton_clicked()
//{
//    QString host = ui->hostEdit->text();
//    int port = ui->portSpin->value();
//    QString user = ui->userEdit->text();
//    QString pass = ui->passEdit->text();
//    if (host.isEmpty()) {
//        QMessageBox::warning(this, QString::fromLocal8Bit("����"), QString::fromLocal8Bit("������FTP��������ַ"));
//        return;
//    }

//    ui->statusLabel->setText(QString::fromLocal8Bit("�������ӵ� %1...").arg(host));
//    ftp->connectToHost(host, port);
//    ftp->login(user, pass);
//}

//// �Ͽ�FTP����
//void FtpClient::on_disconnectButton_clicked()
//{
//    int aa = ftp->state();
//    if (ftp->state() == QFtp::LoggedIn) {
//        ftp->close();
//        ui->statusLabel->setText(QString::fromLocal8Bit("�ѶϿ�����"));
//        ui->fileListWidget->clear();
//        fileList.clear();
//    }
//}

//// ����ѡ�е��ļ�
//void FtpClient::on_downloadButton_clicked()
//{
//    QListWidgetItem *item = ui->fileListWidget->currentItem();
//    if (!item) {
//        QMessageBox::information(this, QString::fromLocal8Bit("��ʾ"), QString::fromLocal8Bit("����ѡ��Ҫ���ص��ļ�"));
//        return;
//    }

//    if (currentDownloadDir.isEmpty()) {
//        QMessageBox::warning(this, QString::fromLocal8Bit("����"), QString::fromLocal8Bit("��ѡ������Ŀ¼"));
//        return;
//    }

//    // ������ʾ�����ҵ���Ӧ��ԭʼ�ļ���
//    QString displayName = item->text();
//    QString originalFileName;
//    foreach (const FileInfo &info, fileList) {
//        if (info.displayName == displayName) {
//            originalFileName = info.originalName;
//            break;
//        }
//    }
//    if (originalFileName.isEmpty()) {
//        QMessageBox::critical(this, QString::fromLocal8Bit("����"), QString::fromLocal8Bit("δ�ҵ���Ӧ�������ļ������޷�����"));
//        return;
//    }

//    QString localFilePath = currentDownloadDir + "/" + displayName;

//    // ����ļ��Ƿ��Ѵ���
//    if (QFile::exists(localFilePath)) {
//        int ret = QMessageBox::question(this, QString::fromLocal8Bit("�ļ��Ѵ���"),
//                                       QString::fromLocal8Bit("�ļ� %1 �Ѵ��ڣ��Ƿ񸲸ǣ�").arg(displayName),
//                                       QMessageBox::Yes | QMessageBox::No);
//        if (ret == QMessageBox::No) {
//            return;
//        }
//    }

//    // ׼������
//    downloadFile = new QFile(localFilePath);
//    if (!downloadFile->open(QIODevice::WriteOnly)) {
//        QMessageBox::critical(this, QString::fromLocal8Bit("����"),
//                            QString::fromLocal8Bit("�޷����ļ� %1 ����д��").arg(localFilePath));
//        delete downloadFile;
//        downloadFile = nullptr;
//        return;
//    }

//    // ��ʾ���ȶԻ���
//    progressDialog = new QProgressDialog(this);
//    progressDialog->setWindowTitle(QString::fromLocal8Bit("�����ļ�"));
//    progressDialog->setLabelText(QString::fromLocal8Bit("�������� %1...").arg(displayName));
//    progressDialog->setCancelButtonText(QString::fromLocal8Bit("ȡ��"));
//    progressDialog->setRange(0, 0);
//    progressDialog->setValue(0);
//    progressDialog->show();

//    // ��ʼ���� - ʹ��ԭʼ�ļ���
//    ftp->get(originalFileName, downloadFile);
//}

//// �������Ŀ¼
//void FtpClient::on_browseButton_clicked()
//{
//    QString dir = QFileDialog::getExistingDirectory(this, QString::fromLocal8Bit("ѡ������Ŀ¼"),
//                                                   currentDownloadDir);
//    if (!dir.isEmpty()) {
//        currentDownloadDir = dir;
//        ui->downloadDirEdit->setText(currentDownloadDir);
//    }
//}

//// �ļ����͹��˸ı�
//void FtpClient::on_fileTypeCombo_currentIndexChanged(int index)
//{
//    currentFilter = ui->fileTypeCombo->itemData(index).toString();
//    updateFileList();
//}

//// FTP����ִ�����
//void FtpClient::on_ftpCommandFinished(int id, bool error)
//{
//    // ��������������
//    if (ftp->currentCommand() == QFtp::ConnectToHost) {
//        if (error) {
//            QMessageBox::critical(this, QString::fromLocal8Bit("����ʧ��"),
//                                QString::fromLocal8Bit("�޷����ӵ�FTP��������%1").arg(ftp->errorString()));
//            ui->statusLabel->setText(QString::fromLocal8Bit("����ʧ��: %1").arg(ftp->errorString()));
//        } else {
//            ui->statusLabel->setText(QString::fromLocal8Bit("�����ӵ�������"));
//        }
//        return;
//    }

//    // �����¼������
//    if (ftp->currentCommand() == QFtp::Login) {
//        if (error) {
//            QMessageBox::critical(this, QString::fromLocal8Bit("��¼ʧ��"),
//                                QString::fromLocal8Bit("��¼FTP������ʧ�ܣ�%1").arg(ftp->errorString()));
//            ui->statusLabel->setText(QString::fromLocal8Bit("��¼ʧ��: %1").arg(ftp->errorString()));
//            ftp->close();
//        } else {
//            ui->statusLabel->setText(QString::fromLocal8Bit("��¼�ɹ������ڻ�ȡ�ļ��б�..."));
//            // ��¼�ɹ����ȡ�ļ��б�
//            ftp->list();
//        }
//        return;
//    }

//    // �����ȡ�ļ��б�������
//    if (ftp->currentCommand() == QFtp::List) {
//        if (error) {
//            QMessageBox::warning(this, QString::fromLocal8Bit("��ȡ�ļ��б�ʧ��"),
//                               QString::fromLocal8Bit("�޷���ȡ�ļ��б�%1").arg(ftp->errorString()));
//        } else {
//            ui->statusLabel->setText(QString::fromLocal8Bit("�ļ��б��ȡ�ɹ����� %1 ���ļ�").arg(fileList.size()));
//            updateFileList();
//        }
//        return;
//    }

//    // ��������������
//    if (ftp->currentCommand() == QFtp::Get) {
//        progressDialog->close();

//        if (error) {
//            QMessageBox::critical(this, QString::fromLocal8Bit("����ʧ��"),
//                                QString::fromLocal8Bit("�ļ�����ʧ�ܣ�%1").arg(ftp->errorString()));
//            if (downloadFile) {
//                downloadFile->remove(); // ɾ�����������ļ�
//                downloadFile->close();
//                delete downloadFile;
//                downloadFile = nullptr;
//            }
//        } else {
//            QMessageBox::information(this, QString::fromLocal8Bit("���سɹ�"),
//                                   QString::fromLocal8Bit("�ļ��ѳɹ����ص���%1").arg(currentDownloadDir));
//            if (downloadFile) {
//                downloadFile->close();
//                delete downloadFile;
//                downloadFile = nullptr;
//            }
//        }

//        delete progressDialog;
//        progressDialog = nullptr;
//        return;
//    }
//}

//// ����FTP���������ص��ļ���Ϣ
//void FtpClient::on_ftpListInfo(const QUrlInfo &urlInfo)
//{
//    // ֻ�����ļ���������Ŀ¼
//    if (!urlInfo.isDir() && !urlInfo.isSymLink()) {
//        FileInfo info;
//        info.originalName = urlInfo.name(); // ����ԭʼ�ļ�������FTP����

//        // ת����ʾ���ƽ����������
//        QByteArray rawName = info.originalName.toLatin1();
//        info.displayName = convertFileName(rawName);

//        fileList.append(info);
//    }
//}

//// �����ļ��������
//void FtpClient::on_ftpDataTransferProgress(qint64 readBytes, qint64 totalBytes)
//{
//    if (progressDialog) {
//        progressDialog->setMaximum(totalBytes);
//        progressDialog->setValue(readBytes);
//        progressDialog->setLabelText(QString::fromLocal8Bit("�������� %1/%2 �ֽ�")
//                                    .arg(readBytes).arg(totalBytes));
//    }
//}

//// FTP״̬�ı�
//void FtpClient::on_ftpStateChanged(int state)
//{
//    switch (state) {
//    case QFtp::Unconnected:
//        ui->connectButton->setEnabled(true);
//        ui->disconnectButton->setEnabled(false);
//        ui->downloadButton->setEnabled(false);
//        break;
//    case QFtp::Connecting:
//        ui->connectButton->setEnabled(false);
//        ui->disconnectButton->setEnabled(false);
//        ui->downloadButton->setEnabled(false);
//        break;
//    case QFtp::Connected:
//        ui->connectButton->setEnabled(false);
//        ui->disconnectButton->setEnabled(true);
//        ui->downloadButton->setEnabled(true);
//        break;
//    case QFtp::Closing:
//        ui->connectButton->setEnabled(false);
//        ui->disconnectButton->setEnabled(false);
//        ui->downloadButton->setEnabled(false);
//        break;
//    }
//}

//void FtpClient::on_ExitButton_clicked()
//{
//    on_disconnectButton_clicked();
//    this->close();
//}

//// �����ļ��б�Ӧ�ù��ˣ�
//void FtpClient::updateFileList()
//{
//    ui->fileListWidget->clear();

//    foreach (const FileInfo &info, fileList) {
//        // Ӧ���ļ�����
//        if (currentFilter == "*" || info.displayName.contains(QRegExp(currentFilter.replace("*.", "\\.").replace(";", "|")))) {
//            ui->fileListWidget->addItem(info.displayName);
//        }
//    }
//}

//// ת���ļ������룬�����������
//QString FtpClient::convertFileName(const QByteArray &rawData)
//{
//    // ����GBK���루Windows���������ã�
//    QTextCodec *gbkCodec = QTextCodec::codecForName("GBK");
//    QString gbkStr = gbkCodec ? gbkCodec->toUnicode(rawData) : "";

//    // ����UTF-8���루Linux���������ã�
//    QString utf8Str = QString::fromUtf8(rawData);

//    // ���ж����ֱ����������ȷ�����������ַ���
//    if (gbkStr.contains(QRegExp("[\\u4e00-\\u9fa5]"))) {
//        return gbkStr;
//    } else if (utf8Str.contains(QRegExp("[\\u4e00-\\u9fa5]"))) {
//        return utf8Str;
//    }

//    // ������������ģ�ֱ�ӷ���ԭʼ�ַ���
//    return QString::fromLatin1(rawData);
//}
#include "ftpclient.h"
#include "ui_ftpclient.h"
#include <QMessageBox>
#include <QDir>
#include <QFileDialog>
#include <QTextCodec>
#include <QRegExp>
#include <QCloseEvent>
#include <QtDebug>

FtpClient::FtpClient(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::FtpClient)
{
    ui->setupUi(this);
    setWindowTitle(QString::fromLocal8Bit("FTP�ļ�������"));

    // ��ʼ��FTP����
    ftp = new QFtp(this);

    // ��ʼ��ͼ�꣨���滻Ϊ�Զ���ͼ�꣩
    fileIcon = QIcon::fromTheme("text-x-generic", QIcon(":/icons/file.png"));
    dirIcon = QIcon::fromTheme("folder", QIcon(":/icons/folder.png"));

    // ��ʼ��Ŀ¼������·��
    currentRemoteDir = "/";  // Զ�̸�Ŀ¼
    currentDownloadDir = QDir::homePath();
    ui->downloadDirEdit->setText(currentDownloadDir);
    ui->groupBox->hide();

    // ��ʼ���ļ����͹�����
    ui->fileTypeCombo->addItem(QString::fromLocal8Bit("�����ļ�"), "*");
    ui->fileTypeCombo->addItem(QString::fromLocal8Bit("�ı��ļ�"), "*.txt;*.asc;*.log");
    ui->fileTypeCombo->addItem(QString::fromLocal8Bit("ͼƬ�ļ�"), "*.jpg;*.jpeg;*.png;*.bmp;*.gif");
    ui->fileTypeCombo->addItem(QString::fromLocal8Bit("ѹ���ļ�"), "*.zip;*.rar;*.7z;*.tar;*.gz");
    ui->fileTypeCombo->addItem(QString::fromLocal8Bit("ASC�ļ�"), "*.asc");
    currentFilter = "*";

    // �����ź����
    connect(ftp, SIGNAL(commandFinished(int, bool)), this, SLOT(on_ftpCommandFinished(int, bool)));
    connect(ftp, SIGNAL(listInfo(const QUrlInfo &)), this, SLOT(on_ftpListInfo(const QUrlInfo &)));
    connect(ftp, SIGNAL(dataTransferProgress(qint64, qint64)), this, SLOT(on_ftpDataTransferProgress(qint64, qint64)));
    connect(ftp, SIGNAL(stateChanged(int)), this, SLOT(on_ftpStateChanged(int)));
    connect(ui->fileListWidget, &QListWidget::itemDoubleClicked, this, &FtpClient::on_itemDoubleClicked);

    // ���ð�ť��ʼ��
    ui->disconnectButton->setEnabled(false);
    ui->downloadButton->setEnabled(false);
    ui->goUpButton->setEnabled(false);  // ����UI��"�����ϼ�"��ť

}

FtpClient::~FtpClient()
{
    on_ExitButton_clicked();
    delete ui;
    if (ftp->state() == QFtp::Connected || ftp->state() == QFtp::LoggedIn) {
        ftp->close();
    }
    delete ftp;
}

void FtpClient::onConnectServer()
{
    ui->fileListWidget->clear();
    on_connectButton_clicked();
}

void FtpClient::SetDownloadDir(QString path)
{
    currentDownloadDir = path;
    ui->downloadDirEdit->setText(currentDownloadDir);
}

void FtpClient::closeEvent(QCloseEvent *event)
{
    on_ExitButton_clicked();
    event->accept();
}

// ���ӵ�FTP������
void FtpClient::on_connectButton_clicked()
{
    QString host = ui->hostEdit->text();
    int port = ui->portSpin->value();
    QString user = ui->userEdit->text();
    QString pass = ui->passEdit->text();

    if (host.isEmpty()) {
        QMessageBox::warning(this, QString::fromLocal8Bit("����"), QString::fromLocal8Bit("������FTP��������ַ"));
        return;
    }

    ui->statusLabel->setText(QString::fromLocal8Bit("�������ӵ� %1...").arg(host));
    ftp->connectToHost(host, port);
    ftp->login(user, pass);
}

// �Ͽ�FTP����
void FtpClient::on_disconnectButton_clicked()
{
    if (ftp->state() == QFtp::LoggedIn || ftp->state() == QFtp::Connected) {
        ftp->close();
        ui->statusLabel->setText(QString::fromLocal8Bit("�ѶϿ�����"));
        ui->fileListWidget->clear();
        fileList.clear();
        currentRemoteDir = "/";  // ����Զ��Ŀ¼
        ui->goUpButton->setEnabled(false);
    }
}

// ����ѡ�е��ļ�
void FtpClient::on_downloadButton_clicked()
{
    QListWidgetItem *item = ui->fileListWidget->currentItem();
    if (!item) {
        QMessageBox::information(this, QString::fromLocal8Bit("��ʾ"), QString::fromLocal8Bit("����ѡ��Ҫ���ص��ļ�"));
        return;
    }

    // ��ѡ��Ŀ¼������
    bool isDir = item->data(Qt::UserRole).toBool();
    if (isDir) {
        QMessageBox::information(this, QString::fromLocal8Bit("��ʾ"), QString::fromLocal8Bit("��ѡ���ļ�����Ŀ¼"));
        return;
    }

    if (currentDownloadDir.isEmpty()) {
        QMessageBox::warning(this, QString::fromLocal8Bit("����"), QString::fromLocal8Bit("��ѡ������Ŀ¼"));
        return;
    }

    // ��ȡԭʼ�ļ���
    QString displayName = item->text();
    QString originalFileName;
    foreach (const FileInfo &info, fileList) {
        if (info.displayName == displayName && !info.isDir) {
            originalFileName = info.originalName;
            break;
        }
    }
    if (originalFileName.isEmpty()) {
        QMessageBox::critical(this, QString::fromLocal8Bit("����"), QString::fromLocal8Bit("δ�ҵ���Ӧ�������ļ���"));
        return;
    }

    // ���������ļ�·��
    QString localFilePath = currentDownloadDir + "/" + displayName;
    if (QFile::exists(localFilePath)) {
        int ret = QMessageBox::question(this, QString::fromLocal8Bit("�ļ��Ѵ���"),
                                       QString::fromLocal8Bit("�ļ� %1 �Ѵ��ڣ��Ƿ񸲸ǣ�").arg(displayName),
                                       QMessageBox::Yes | QMessageBox::No);
        if (ret == QMessageBox::No) return;
    }

    // ׼�������ļ�
    downloadFile = new QFile(localFilePath);
    if (!downloadFile->open(QIODevice::WriteOnly)) {
        QMessageBox::critical(this, QString::fromLocal8Bit("����"),
                            QString::fromLocal8Bit("�޷����ļ� %1 ����д��").arg(localFilePath));
        delete downloadFile;
        downloadFile = nullptr;
        return;
    }

    // ��ʾ���ȶԻ���
    progressDialog = new QProgressDialog(this);
    progressDialog->setWindowTitle(QString::fromLocal8Bit("�����ļ�"));
    progressDialog->setLabelText(QString::fromLocal8Bit("�������� %1...").arg(displayName));
    progressDialog->setCancelButtonText(QString::fromLocal8Bit("ȡ��"));
    progressDialog->setRange(0, 0);
    progressDialog->show();

    // ��ʼ���أ�ʹ��Զ��Ŀ¼+ԭʼ�ļ�����
    QString remoteFilePath = currentRemoteDir;
    if (!remoteFilePath.endsWith("/")) remoteFilePath += "/";
    remoteFilePath += originalFileName;
    ftp->get(remoteFilePath, downloadFile);
}

// �������Ŀ¼
void FtpClient::on_browseButton_clicked()
{
    QString dir = QFileDialog::getExistingDirectory(this, QString::fromLocal8Bit("ѡ������Ŀ¼"),
                                                   currentDownloadDir);
    if (!dir.isEmpty()) {
        currentDownloadDir = dir;
        ui->downloadDirEdit->setText(currentDownloadDir);
    }
}

// �ļ����͹��˸ı�
void FtpClient::on_fileTypeCombo_currentIndexChanged(int index)
{
    currentFilter = ui->fileTypeCombo->itemData(index).toString();
    updateFileList();
}

// FTP����ִ�����
void FtpClient::on_ftpCommandFinished(int id, bool error)
{
    // ������������
    if (ftp->currentCommand() == QFtp::ConnectToHost) {
        if (error) {
            QMessageBox::critical(this, QString::fromLocal8Bit("����ʧ��"),
                                QString::fromLocal8Bit("�޷����ӵ���������%1").arg(ftp->errorString()));
            ui->statusLabel->setText(QString::fromLocal8Bit("����ʧ��: %1").arg(ftp->errorString()));
        } else {
            ui->statusLabel->setText(QString::fromLocal8Bit("�����ӵ�������"));
        }
        return;
    }

    // �����¼����
    if (ftp->currentCommand() == QFtp::Login) {
        if (error) {
            QMessageBox::critical(this, QString::fromLocal8Bit("��¼ʧ��"),
                                QString::fromLocal8Bit("��¼ʧ�ܣ�%1").arg(ftp->errorString()));
            ui->statusLabel->setText(QString::fromLocal8Bit("��¼ʧ��: %1").arg(ftp->errorString()));
            ftp->close();
        } else {
            ui->statusLabel->setText(QString::fromLocal8Bit("��¼�ɹ������ڻ�ȡ�ļ��б�..."));
            ftp->list();  // ��ȡ��Ŀ¼�б�
            ui->goUpButton->setEnabled(false);  // ��Ŀ¼�޷������ϼ�
        }
        return;
    }

    // ����Ŀ¼�л�����
    if (ftp->currentCommand() == QFtp::Cd) {
        if (error) {
            QMessageBox::warning(this, QString::fromLocal8Bit("�л�Ŀ¼ʧ��"),
                               QString::fromLocal8Bit("�޷�����Ŀ¼��%1").arg(ftp->errorString()));
            // �ع�Ŀ¼
            currentRemoteDir = currentRemoteDir.left(currentRemoteDir.lastIndexOf("/"));
            if (currentRemoteDir.isEmpty()) currentRemoteDir = "/";
        } else {
            ui->statusLabel->setText(QString::fromLocal8Bit("�ѽ���Ŀ¼: %1").arg(currentRemoteDir));
            ftp->list();  // ��ȡ��Ŀ¼�б�
            ui->goUpButton->setEnabled(currentRemoteDir != "/");  // ��Ŀ¼���÷���
        }
        return;
    }

    // �����ļ��б�����
    if (ftp->currentCommand() == QFtp::List) {
        if (error) {
            QMessageBox::warning(this, QString::fromLocal8Bit("��ȡ�б�ʧ��"),
                               QString::fromLocal8Bit("�޷���ȡ�ļ��б�%1").arg(ftp->errorString()));
        } else {
            ui->statusLabel->setText(QString::fromLocal8Bit("��ǰĿ¼: %1���� %2 ����Ŀ").arg(currentRemoteDir).arg(fileList.size()));
            updateFileList();
        }
        return;
    }

    // ������������
    if (ftp->currentCommand() == QFtp::Get) {
        progressDialog->close();

        if (error) {
            QMessageBox::critical(this, QString::fromLocal8Bit("����ʧ��"),
                                QString::fromLocal8Bit("����ʧ�ܣ�%1").arg(ftp->errorString()));
            if (downloadFile) {
                downloadFile->remove();
                downloadFile->close();
                delete downloadFile;
                downloadFile = nullptr;
            }
        } else {
            QMessageBox::information(this, QString::fromLocal8Bit("���سɹ�"),
                                   QString::fromLocal8Bit("�ļ��ѱ��浽��%1").arg(currentDownloadDir));
            if (downloadFile) {
                downloadFile->close();
                delete downloadFile;
                downloadFile = nullptr;
            }
        }

        delete progressDialog;
        progressDialog = nullptr;
        return;
    }
}

// ����FTP���������ص��ļ�/Ŀ¼��Ϣ
void FtpClient::on_ftpListInfo(const QUrlInfo &urlInfo)
{
    if (urlInfo.isSymLink()) return;  // ���Է�������

    FileInfo info;
    info.originalName = urlInfo.name();
    info.isDir = urlInfo.isDir();
    info.size = urlInfo.size();

    if(info.isDir)
    {
        // ת����ʾ���ƣ�����������룩
        QByteArray rawName = info.originalName.toLatin1();
        info.displayName = convertFileName2(rawName);
    }
    else
    {
        // ת����ʾ���ƣ�����������룩
        QByteArray rawName = info.originalName.toLatin1();
        info.displayName = convertFileName(rawName);
    }

    fileList.append(info);
}

// �����ļ��������
void FtpClient::on_ftpDataTransferProgress(qint64 readBytes, qint64 totalBytes)
{
    if (progressDialog) {
        progressDialog->setMaximum(totalBytes);
        progressDialog->setValue(readBytes);
        progressDialog->setLabelText(QString::fromLocal8Bit("�������� %1/%2 �ֽ�")
                                    .arg(readBytes).arg(totalBytes));
    }
}

// FTP״̬�ı�
void FtpClient::on_ftpStateChanged(int state)
{
    switch (state) {
    case QFtp::Unconnected:
        ui->connectButton->setEnabled(true);
        ui->disconnectButton->setEnabled(false);
        ui->downloadButton->setEnabled(false);
        ui->goUpButton->setEnabled(false);
        break;
    case QFtp::Connecting:
        ui->connectButton->setEnabled(false);
        ui->disconnectButton->setEnabled(false);
        ui->downloadButton->setEnabled(false);
        ui->goUpButton->setEnabled(false);
        break;
    case QFtp::Connected:
    case QFtp::LoggedIn:
        ui->connectButton->setEnabled(false);
        ui->disconnectButton->setEnabled(true);
        ui->downloadButton->setEnabled(true);
        ui->goUpButton->setEnabled(currentRemoteDir != "/");
        break;
    case QFtp::Closing:
        ui->connectButton->setEnabled(false);
        ui->disconnectButton->setEnabled(false);
        ui->downloadButton->setEnabled(false);
        ui->goUpButton->setEnabled(false);
        break;
    }
}

// �˳���ť
void FtpClient::on_ExitButton_clicked()
{
    on_disconnectButton_clicked();
    this->close();
}

// ˫��Ŀ¼�л�
void FtpClient::on_itemDoubleClicked(QListWidgetItem *item)
{
    bool isDir = item->data(Qt::UserRole).toBool();
    if (!isDir) return;  // ��Ŀ¼������

    // ��ȡĿ¼ԭʼ����
    QString displayName = item->text();
    QString targetDirOriginalName;
    foreach (const FileInfo &info, fileList) {
        if (info.displayName == displayName && info.isDir) {
            targetDirOriginalName = info.originalName;
            break;
        }
    }

    if (targetDirOriginalName.isEmpty()) {
        QMessageBox::warning(this, QString::fromLocal8Bit("����"), QString::fromLocal8Bit("Ŀ¼��Ϣ��ȡʧ��"));
        return;
    }

    // ������Ŀ¼·��
    QString newRemoteDir = currentRemoteDir;
    if (!newRemoteDir.endsWith("/")) newRemoteDir += "/";
    newRemoteDir += targetDirOriginalName;

    // �л�Ŀ¼
    ui->statusLabel->setText(QString::fromLocal8Bit("���ڽ���Ŀ¼: %1...").arg(displayName));
    ftp->cd(newRemoteDir);
    currentRemoteDir = newRemoteDir;

    // ��վ��б�
    fileList.clear();
    ui->fileListWidget->clear();
}

// �����ϼ�Ŀ¼
void FtpClient::on_goUpButton_clicked()
{
    if (currentRemoteDir == "/") {
        QMessageBox::information(this, QString::fromLocal8Bit("��ʾ"), QString::fromLocal8Bit("���ڸ�Ŀ¼"));
        return;
    }

    // �����ϼ�Ŀ¼·��
    QString parentDir = currentRemoteDir.left(currentRemoteDir.lastIndexOf("/"));
    if (parentDir.isEmpty()) parentDir = "/";

    // �л����ϼ�Ŀ¼
    ui->statusLabel->setText(QString::fromLocal8Bit("���ڷ����ϼ�Ŀ¼..."));
    ftp->cd(parentDir);
    currentRemoteDir = parentDir;

    // ��վ��б�
    fileList.clear();
    ui->fileListWidget->clear();
}

// �����ļ��б�Ӧ�ù��ˣ�
void FtpClient::updateFileList()
{
    ui->fileListWidget->clear();

    foreach (const FileInfo &info, fileList) {
        // Ŀ¼ʼ����ʾ���ļ�Ӧ�ù���
        if (info.isDir || currentFilter == "*" ||
            info.displayName.contains(QRegExp(currentFilter.replace("*.", "\\.").replace(";", "|")))) {

            QListWidgetItem *item = new QListWidgetItem(info.displayName);
            item->setIcon(info.isDir ? dirIcon : fileIcon);  // ����ͼ��
            item->setData(Qt::UserRole, info.isDir);  // �洢�Ƿ�ΪĿ¼
            ui->fileListWidget->addItem(item);
        }
    }
}
QString FtpClient::convertFileName2(const QByteArray &rawData)
{
    // ����FTP�������������ȼ���GBK(Windows)��UTF-8(Linux)��GB2312��Big5
    QList<QPair<QString, bool>> codecs = {
        {"GBK", true},    // Windows����������
        {"UTF-8", false}, // Linux/Unix����������
        {"GB2312", true},
        {"Big5", true}
    };

    for (auto &codecPair : codecs) {
        QTextCodec *codec = QTextCodec::codecForName(codecPair.first.toUtf8());
        if (!codec) continue;

        QString decoded = codec->toUnicode(rawData);
        // У�飺ͨ���ַ���Χ�ж��Ƿ�Ϊ�������ģ��ų����룩
        bool hasValidChinese = false;
        for (QChar c : decoded) {
            if (c.unicode() >= 0x4E00 && c.unicode() <= 0x9FFF) {
                hasValidChinese = true;
                break;
            }
        }
        if (hasValidChinese) {
            return decoded;
        }
    }

    // ���ף�����ϵͳ���ر���
    return QString::fromLocal8Bit(rawData);
}
// ת���ļ������루����������룩
QString FtpClient::convertFileName(const QByteArray &rawData)
{
    // ����GBK���루Windows���������ã�
    QTextCodec *gbkCodec = QTextCodec::codecForName("GBK");
    QString gbkStr = gbkCodec ? gbkCodec->toUnicode(rawData) : "";

    // ����UTF-8���루Linux���������ã�
    QString utf8Str = QString::fromUtf8(rawData);

    // �ж����ֱ����������ȷ���������ַ���
    if (gbkStr.contains(QRegExp("[\\u4e00-\\u9fa5]"))) {
        return gbkStr;
    } else if (utf8Str.contains(QRegExp("[\\u4e00-\\u9fa5]"))) {
        return utf8Str;
    }

    // �������򷵻�ԭʼ�ַ���
    return QString::fromLatin1(rawData);
}

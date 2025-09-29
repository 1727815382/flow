//#include "ftpclient.h"
//#include "ui_ftpclient.h"

//FtpClient::FtpClient(QDialog *parent)
//    : QDialog(parent)
//    , ui(new Ui::FtpClient)
//{
//    ui->setupUi(this);
//    setWindowTitle(QString::fromLocal8Bit("下载文件"));

//    // 初始化FTP对象
//    ftp = new QFtp(this);

//    // 设置默认下载目录为用户主目录
//    currentDownloadDir = QDir::homePath();
//    ui->downloadDirEdit->setText(currentDownloadDir);
//    ui->groupBox->hide();

////    url.setScheme("ftp");//设置该计划描述了URL的类型（或协议）
////    url.setHost("127.0.0.1");//设置主机地址

////    url.setPort(21);//设置URL的端口。该端口是URL的权限的一部分，如setAuthority（描述）。端口必须是介于0和65535（含）。端口设置为-1表示该端口是不确定的。
////    url.setPassword("123"); //设置ftp用户密码
////    url.setUserName("123");//设置tfp用户名



//    // 初始化文件类型过滤器
//    ui->fileTypeCombo->addItem(QString::fromLocal8Bit("所有文件"), "*");
//    ui->fileTypeCombo->addItem(QString::fromLocal8Bit("文本文件"), "*.txt;*.asc;*.log");
//    ui->fileTypeCombo->addItem(QString::fromLocal8Bit("图片文件"), "*.jpg;*.jpeg;*.png;*.bmp;*.gif");
//    ui->fileTypeCombo->addItem(QString::fromLocal8Bit("压缩文件"), "*.zip;*.rar;*.7z;*.tar;*.gz");
//    ui->fileTypeCombo->addItem(QString::fromLocal8Bit("*.asc"), "*.asc;");
//    currentFilter = "*";

//    // 连接FTP信号与槽
//    connect(ftp, SIGNAL(commandFinished(int, bool)), this, SLOT(on_ftpCommandFinished(int, bool)));
//    connect(ftp, SIGNAL(listInfo(const QUrlInfo &)), this, SLOT(on_ftpListInfo(const QUrlInfo &)));
//    connect(ftp, SIGNAL(dataTransferProgress(qint64, qint64)), this, SLOT(on_ftpDataTransferProgress(qint64, qint64)));
//    connect(ftp, SIGNAL(stateChanged(int)), this, SLOT(on_ftpStateChanged(int)));

//    // 禁用断开连接和下载按钮
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

//// 连接到FTP服务器
//void FtpClient::on_connectButton_clicked()
//{
//    QString host = ui->hostEdit->text();
//    int port = ui->portSpin->value();
//    QString user = ui->userEdit->text();
//    QString pass = ui->passEdit->text();
//    if (host.isEmpty()) {
//        QMessageBox::warning(this, QString::fromLocal8Bit("警告"), QString::fromLocal8Bit("请输入FTP服务器地址"));
//        return;
//    }

//    ui->statusLabel->setText(QString::fromLocal8Bit("正在连接到 %1...").arg(host));
//    ftp->connectToHost(host, port);
//    ftp->login(user, pass);
//}

//// 断开FTP连接
//void FtpClient::on_disconnectButton_clicked()
//{
//    int aa = ftp->state();
//    if (ftp->state() == QFtp::LoggedIn) {
//        ftp->close();
//        ui->statusLabel->setText(QString::fromLocal8Bit("已断开连接"));
//        ui->fileListWidget->clear();
//        fileList.clear();
//    }
//}

//// 下载选中的文件
//void FtpClient::on_downloadButton_clicked()
//{
//    QListWidgetItem *item = ui->fileListWidget->currentItem();
//    if (!item) {
//        QMessageBox::information(this, QString::fromLocal8Bit("提示"), QString::fromLocal8Bit("请先选择要下载的文件"));
//        return;
//    }

//    if (currentDownloadDir.isEmpty()) {
//        QMessageBox::warning(this, QString::fromLocal8Bit("警告"), QString::fromLocal8Bit("请选择下载目录"));
//        return;
//    }

//    // 根据显示名称找到对应的原始文件名
//    QString displayName = item->text();
//    QString originalFileName;
//    foreach (const FileInfo &info, fileList) {
//        if (info.displayName == displayName) {
//            originalFileName = info.originalName;
//            break;
//        }
//    }
//    if (originalFileName.isEmpty()) {
//        QMessageBox::critical(this, QString::fromLocal8Bit("错误"), QString::fromLocal8Bit("未找到对应服务器文件名，无法下载"));
//        return;
//    }

//    QString localFilePath = currentDownloadDir + "/" + displayName;

//    // 检查文件是否已存在
//    if (QFile::exists(localFilePath)) {
//        int ret = QMessageBox::question(this, QString::fromLocal8Bit("文件已存在"),
//                                       QString::fromLocal8Bit("文件 %1 已存在，是否覆盖？").arg(displayName),
//                                       QMessageBox::Yes | QMessageBox::No);
//        if (ret == QMessageBox::No) {
//            return;
//        }
//    }

//    // 准备下载
//    downloadFile = new QFile(localFilePath);
//    if (!downloadFile->open(QIODevice::WriteOnly)) {
//        QMessageBox::critical(this, QString::fromLocal8Bit("错误"),
//                            QString::fromLocal8Bit("无法打开文件 %1 进行写入").arg(localFilePath));
//        delete downloadFile;
//        downloadFile = nullptr;
//        return;
//    }

//    // 显示进度对话框
//    progressDialog = new QProgressDialog(this);
//    progressDialog->setWindowTitle(QString::fromLocal8Bit("下载文件"));
//    progressDialog->setLabelText(QString::fromLocal8Bit("正在下载 %1...").arg(displayName));
//    progressDialog->setCancelButtonText(QString::fromLocal8Bit("取消"));
//    progressDialog->setRange(0, 0);
//    progressDialog->setValue(0);
//    progressDialog->show();

//    // 开始下载 - 使用原始文件名
//    ftp->get(originalFileName, downloadFile);
//}

//// 浏览下载目录
//void FtpClient::on_browseButton_clicked()
//{
//    QString dir = QFileDialog::getExistingDirectory(this, QString::fromLocal8Bit("选择下载目录"),
//                                                   currentDownloadDir);
//    if (!dir.isEmpty()) {
//        currentDownloadDir = dir;
//        ui->downloadDirEdit->setText(currentDownloadDir);
//    }
//}

//// 文件类型过滤改变
//void FtpClient::on_fileTypeCombo_currentIndexChanged(int index)
//{
//    currentFilter = ui->fileTypeCombo->itemData(index).toString();
//    updateFileList();
//}

//// FTP命令执行完成
//void FtpClient::on_ftpCommandFinished(int id, bool error)
//{
//    // 处理连接命令结果
//    if (ftp->currentCommand() == QFtp::ConnectToHost) {
//        if (error) {
//            QMessageBox::critical(this, QString::fromLocal8Bit("连接失败"),
//                                QString::fromLocal8Bit("无法连接到FTP服务器：%1").arg(ftp->errorString()));
//            ui->statusLabel->setText(QString::fromLocal8Bit("连接失败: %1").arg(ftp->errorString()));
//        } else {
//            ui->statusLabel->setText(QString::fromLocal8Bit("已连接到服务器"));
//        }
//        return;
//    }

//    // 处理登录命令结果
//    if (ftp->currentCommand() == QFtp::Login) {
//        if (error) {
//            QMessageBox::critical(this, QString::fromLocal8Bit("登录失败"),
//                                QString::fromLocal8Bit("登录FTP服务器失败：%1").arg(ftp->errorString()));
//            ui->statusLabel->setText(QString::fromLocal8Bit("登录失败: %1").arg(ftp->errorString()));
//            ftp->close();
//        } else {
//            ui->statusLabel->setText(QString::fromLocal8Bit("登录成功，正在获取文件列表..."));
//            // 登录成功后获取文件列表
//            ftp->list();
//        }
//        return;
//    }

//    // 处理获取文件列表命令结果
//    if (ftp->currentCommand() == QFtp::List) {
//        if (error) {
//            QMessageBox::warning(this, QString::fromLocal8Bit("获取文件列表失败"),
//                               QString::fromLocal8Bit("无法获取文件列表：%1").arg(ftp->errorString()));
//        } else {
//            ui->statusLabel->setText(QString::fromLocal8Bit("文件列表获取成功，共 %1 个文件").arg(fileList.size()));
//            updateFileList();
//        }
//        return;
//    }

//    // 处理下载命令结果
//    if (ftp->currentCommand() == QFtp::Get) {
//        progressDialog->close();

//        if (error) {
//            QMessageBox::critical(this, QString::fromLocal8Bit("下载失败"),
//                                QString::fromLocal8Bit("文件下载失败：%1").arg(ftp->errorString()));
//            if (downloadFile) {
//                downloadFile->remove(); // 删除不完整的文件
//                downloadFile->close();
//                delete downloadFile;
//                downloadFile = nullptr;
//            }
//        } else {
//            QMessageBox::information(this, QString::fromLocal8Bit("下载成功"),
//                                   QString::fromLocal8Bit("文件已成功下载到：%1").arg(currentDownloadDir));
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

//// 处理FTP服务器返回的文件信息
//void FtpClient::on_ftpListInfo(const QUrlInfo &urlInfo)
//{
//    // 只处理文件，不处理目录
//    if (!urlInfo.isDir() && !urlInfo.isSymLink()) {
//        FileInfo info;
//        info.originalName = urlInfo.name(); // 保存原始文件名用于FTP操作

//        // 转换显示名称解决中文乱码
//        QByteArray rawName = info.originalName.toLatin1();
//        info.displayName = convertFileName(rawName);

//        fileList.append(info);
//    }
//}

//// 更新文件传输进度
//void FtpClient::on_ftpDataTransferProgress(qint64 readBytes, qint64 totalBytes)
//{
//    if (progressDialog) {
//        progressDialog->setMaximum(totalBytes);
//        progressDialog->setValue(readBytes);
//        progressDialog->setLabelText(QString::fromLocal8Bit("正在下载 %1/%2 字节")
//                                    .arg(readBytes).arg(totalBytes));
//    }
//}

//// FTP状态改变
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

//// 更新文件列表（应用过滤）
//void FtpClient::updateFileList()
//{
//    ui->fileListWidget->clear();

//    foreach (const FileInfo &info, fileList) {
//        // 应用文件过滤
//        if (currentFilter == "*" || info.displayName.contains(QRegExp(currentFilter.replace("*.", "\\.").replace(";", "|")))) {
//            ui->fileListWidget->addItem(info.displayName);
//        }
//    }
//}

//// 转换文件名编码，解决中文乱码
//QString FtpClient::convertFileName(const QByteArray &rawData)
//{
//    // 尝试GBK编码（Windows服务器常用）
//    QTextCodec *gbkCodec = QTextCodec::codecForName("GBK");
//    QString gbkStr = gbkCodec ? gbkCodec->toUnicode(rawData) : "";

//    // 尝试UTF-8编码（Linux服务器常用）
//    QString utf8Str = QString::fromUtf8(rawData);

//    // 简单判断哪种编码更可能正确（包含中文字符）
//    if (gbkStr.contains(QRegExp("[\\u4e00-\\u9fa5]"))) {
//        return gbkStr;
//    } else if (utf8Str.contains(QRegExp("[\\u4e00-\\u9fa5]"))) {
//        return utf8Str;
//    }

//    // 如果都不是中文，直接返回原始字符串
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
    setWindowTitle(QString::fromLocal8Bit("FTP文件下载器"));

    // 初始化FTP对象
    ftp = new QFtp(this);

    // 初始化图标（可替换为自定义图标）
    fileIcon = QIcon::fromTheme("text-x-generic", QIcon(":/icons/file.png"));
    dirIcon = QIcon::fromTheme("folder", QIcon(":/icons/folder.png"));

    // 初始化目录和下载路径
    currentRemoteDir = "/";  // 远程根目录
    currentDownloadDir = QDir::homePath();
    ui->downloadDirEdit->setText(currentDownloadDir);
    ui->groupBox->hide();

    // 初始化文件类型过滤器
    ui->fileTypeCombo->addItem(QString::fromLocal8Bit("所有文件"), "*");
    ui->fileTypeCombo->addItem(QString::fromLocal8Bit("文本文件"), "*.txt;*.asc;*.log");
    ui->fileTypeCombo->addItem(QString::fromLocal8Bit("图片文件"), "*.jpg;*.jpeg;*.png;*.bmp;*.gif");
    ui->fileTypeCombo->addItem(QString::fromLocal8Bit("压缩文件"), "*.zip;*.rar;*.7z;*.tar;*.gz");
    ui->fileTypeCombo->addItem(QString::fromLocal8Bit("ASC文件"), "*.asc");
    currentFilter = "*";

    // 连接信号与槽
    connect(ftp, SIGNAL(commandFinished(int, bool)), this, SLOT(on_ftpCommandFinished(int, bool)));
    connect(ftp, SIGNAL(listInfo(const QUrlInfo &)), this, SLOT(on_ftpListInfo(const QUrlInfo &)));
    connect(ftp, SIGNAL(dataTransferProgress(qint64, qint64)), this, SLOT(on_ftpDataTransferProgress(qint64, qint64)));
    connect(ftp, SIGNAL(stateChanged(int)), this, SLOT(on_ftpStateChanged(int)));
    connect(ui->fileListWidget, &QListWidget::itemDoubleClicked, this, &FtpClient::on_itemDoubleClicked);

    // 禁用按钮初始化
    ui->disconnectButton->setEnabled(false);
    ui->downloadButton->setEnabled(false);
    ui->goUpButton->setEnabled(false);  // 假设UI有"返回上级"按钮

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

// 连接到FTP服务器
void FtpClient::on_connectButton_clicked()
{
    QString host = ui->hostEdit->text();
    int port = ui->portSpin->value();
    QString user = ui->userEdit->text();
    QString pass = ui->passEdit->text();

    if (host.isEmpty()) {
        QMessageBox::warning(this, QString::fromLocal8Bit("警告"), QString::fromLocal8Bit("请输入FTP服务器地址"));
        return;
    }

    ui->statusLabel->setText(QString::fromLocal8Bit("正在连接到 %1...").arg(host));
    ftp->connectToHost(host, port);
    ftp->login(user, pass);
}

// 断开FTP连接
void FtpClient::on_disconnectButton_clicked()
{
    if (ftp->state() == QFtp::LoggedIn || ftp->state() == QFtp::Connected) {
        ftp->close();
        ui->statusLabel->setText(QString::fromLocal8Bit("已断开连接"));
        ui->fileListWidget->clear();
        fileList.clear();
        currentRemoteDir = "/";  // 重置远程目录
        ui->goUpButton->setEnabled(false);
    }
}

// 下载选中的文件
void FtpClient::on_downloadButton_clicked()
{
    QListWidgetItem *item = ui->fileListWidget->currentItem();
    if (!item) {
        QMessageBox::information(this, QString::fromLocal8Bit("提示"), QString::fromLocal8Bit("请先选择要下载的文件"));
        return;
    }

    // 若选中目录则不下载
    bool isDir = item->data(Qt::UserRole).toBool();
    if (isDir) {
        QMessageBox::information(this, QString::fromLocal8Bit("提示"), QString::fromLocal8Bit("请选择文件而非目录"));
        return;
    }

    if (currentDownloadDir.isEmpty()) {
        QMessageBox::warning(this, QString::fromLocal8Bit("警告"), QString::fromLocal8Bit("请选择下载目录"));
        return;
    }

    // 获取原始文件名
    QString displayName = item->text();
    QString originalFileName;
    foreach (const FileInfo &info, fileList) {
        if (info.displayName == displayName && !info.isDir) {
            originalFileName = info.originalName;
            break;
        }
    }
    if (originalFileName.isEmpty()) {
        QMessageBox::critical(this, QString::fromLocal8Bit("错误"), QString::fromLocal8Bit("未找到对应服务器文件名"));
        return;
    }

    // 构建本地文件路径
    QString localFilePath = currentDownloadDir + "/" + displayName;
    if (QFile::exists(localFilePath)) {
        int ret = QMessageBox::question(this, QString::fromLocal8Bit("文件已存在"),
                                       QString::fromLocal8Bit("文件 %1 已存在，是否覆盖？").arg(displayName),
                                       QMessageBox::Yes | QMessageBox::No);
        if (ret == QMessageBox::No) return;
    }

    // 准备下载文件
    downloadFile = new QFile(localFilePath);
    if (!downloadFile->open(QIODevice::WriteOnly)) {
        QMessageBox::critical(this, QString::fromLocal8Bit("错误"),
                            QString::fromLocal8Bit("无法打开文件 %1 进行写入").arg(localFilePath));
        delete downloadFile;
        downloadFile = nullptr;
        return;
    }

    // 显示进度对话框
    progressDialog = new QProgressDialog(this);
    progressDialog->setWindowTitle(QString::fromLocal8Bit("下载文件"));
    progressDialog->setLabelText(QString::fromLocal8Bit("正在下载 %1...").arg(displayName));
    progressDialog->setCancelButtonText(QString::fromLocal8Bit("取消"));
    progressDialog->setRange(0, 0);
    progressDialog->show();

    // 开始下载（使用远程目录+原始文件名）
    QString remoteFilePath = currentRemoteDir;
    if (!remoteFilePath.endsWith("/")) remoteFilePath += "/";
    remoteFilePath += originalFileName;
    ftp->get(remoteFilePath, downloadFile);
}

// 浏览下载目录
void FtpClient::on_browseButton_clicked()
{
    QString dir = QFileDialog::getExistingDirectory(this, QString::fromLocal8Bit("选择下载目录"),
                                                   currentDownloadDir);
    if (!dir.isEmpty()) {
        currentDownloadDir = dir;
        ui->downloadDirEdit->setText(currentDownloadDir);
    }
}

// 文件类型过滤改变
void FtpClient::on_fileTypeCombo_currentIndexChanged(int index)
{
    currentFilter = ui->fileTypeCombo->itemData(index).toString();
    updateFileList();
}

// FTP命令执行完成
void FtpClient::on_ftpCommandFinished(int id, bool error)
{
    // 处理连接命令
    if (ftp->currentCommand() == QFtp::ConnectToHost) {
        if (error) {
            QMessageBox::critical(this, QString::fromLocal8Bit("连接失败"),
                                QString::fromLocal8Bit("无法连接到服务器：%1").arg(ftp->errorString()));
            ui->statusLabel->setText(QString::fromLocal8Bit("连接失败: %1").arg(ftp->errorString()));
        } else {
            ui->statusLabel->setText(QString::fromLocal8Bit("已连接到服务器"));
        }
        return;
    }

    // 处理登录命令
    if (ftp->currentCommand() == QFtp::Login) {
        if (error) {
            QMessageBox::critical(this, QString::fromLocal8Bit("登录失败"),
                                QString::fromLocal8Bit("登录失败：%1").arg(ftp->errorString()));
            ui->statusLabel->setText(QString::fromLocal8Bit("登录失败: %1").arg(ftp->errorString()));
            ftp->close();
        } else {
            ui->statusLabel->setText(QString::fromLocal8Bit("登录成功，正在获取文件列表..."));
            ftp->list();  // 获取根目录列表
            ui->goUpButton->setEnabled(false);  // 根目录无法返回上级
        }
        return;
    }

    // 处理目录切换命令
    if (ftp->currentCommand() == QFtp::Cd) {
        if (error) {
            QMessageBox::warning(this, QString::fromLocal8Bit("切换目录失败"),
                               QString::fromLocal8Bit("无法进入目录：%1").arg(ftp->errorString()));
            // 回滚目录
            currentRemoteDir = currentRemoteDir.left(currentRemoteDir.lastIndexOf("/"));
            if (currentRemoteDir.isEmpty()) currentRemoteDir = "/";
        } else {
            ui->statusLabel->setText(QString::fromLocal8Bit("已进入目录: %1").arg(currentRemoteDir));
            ftp->list();  // 获取新目录列表
            ui->goUpButton->setEnabled(currentRemoteDir != "/");  // 根目录禁用返回
        }
        return;
    }

    // 处理文件列表命令
    if (ftp->currentCommand() == QFtp::List) {
        if (error) {
            QMessageBox::warning(this, QString::fromLocal8Bit("获取列表失败"),
                               QString::fromLocal8Bit("无法获取文件列表：%1").arg(ftp->errorString()));
        } else {
            ui->statusLabel->setText(QString::fromLocal8Bit("当前目录: %1，共 %2 个项目").arg(currentRemoteDir).arg(fileList.size()));
            updateFileList();
        }
        return;
    }

    // 处理下载命令
    if (ftp->currentCommand() == QFtp::Get) {
        progressDialog->close();

        if (error) {
            QMessageBox::critical(this, QString::fromLocal8Bit("下载失败"),
                                QString::fromLocal8Bit("下载失败：%1").arg(ftp->errorString()));
            if (downloadFile) {
                downloadFile->remove();
                downloadFile->close();
                delete downloadFile;
                downloadFile = nullptr;
            }
        } else {
            QMessageBox::information(this, QString::fromLocal8Bit("下载成功"),
                                   QString::fromLocal8Bit("文件已保存到：%1").arg(currentDownloadDir));
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

// 处理FTP服务器返回的文件/目录信息
void FtpClient::on_ftpListInfo(const QUrlInfo &urlInfo)
{
    if (urlInfo.isSymLink()) return;  // 忽略符号链接

    FileInfo info;
    info.originalName = urlInfo.name();
    info.isDir = urlInfo.isDir();
    info.size = urlInfo.size();

    if(info.isDir)
    {
        // 转换显示名称（解决中文乱码）
        QByteArray rawName = info.originalName.toLatin1();
        info.displayName = convertFileName2(rawName);
    }
    else
    {
        // 转换显示名称（解决中文乱码）
        QByteArray rawName = info.originalName.toLatin1();
        info.displayName = convertFileName(rawName);
    }

    fileList.append(info);
}

// 更新文件传输进度
void FtpClient::on_ftpDataTransferProgress(qint64 readBytes, qint64 totalBytes)
{
    if (progressDialog) {
        progressDialog->setMaximum(totalBytes);
        progressDialog->setValue(readBytes);
        progressDialog->setLabelText(QString::fromLocal8Bit("正在下载 %1/%2 字节")
                                    .arg(readBytes).arg(totalBytes));
    }
}

// FTP状态改变
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

// 退出按钮
void FtpClient::on_ExitButton_clicked()
{
    on_disconnectButton_clicked();
    this->close();
}

// 双击目录切换
void FtpClient::on_itemDoubleClicked(QListWidgetItem *item)
{
    bool isDir = item->data(Qt::UserRole).toBool();
    if (!isDir) return;  // 非目录不处理

    // 获取目录原始名称
    QString displayName = item->text();
    QString targetDirOriginalName;
    foreach (const FileInfo &info, fileList) {
        if (info.displayName == displayName && info.isDir) {
            targetDirOriginalName = info.originalName;
            break;
        }
    }

    if (targetDirOriginalName.isEmpty()) {
        QMessageBox::warning(this, QString::fromLocal8Bit("警告"), QString::fromLocal8Bit("目录信息获取失败"));
        return;
    }

    // 构建新目录路径
    QString newRemoteDir = currentRemoteDir;
    if (!newRemoteDir.endsWith("/")) newRemoteDir += "/";
    newRemoteDir += targetDirOriginalName;

    // 切换目录
    ui->statusLabel->setText(QString::fromLocal8Bit("正在进入目录: %1...").arg(displayName));
    ftp->cd(newRemoteDir);
    currentRemoteDir = newRemoteDir;

    // 清空旧列表
    fileList.clear();
    ui->fileListWidget->clear();
}

// 返回上级目录
void FtpClient::on_goUpButton_clicked()
{
    if (currentRemoteDir == "/") {
        QMessageBox::information(this, QString::fromLocal8Bit("提示"), QString::fromLocal8Bit("已在根目录"));
        return;
    }

    // 计算上级目录路径
    QString parentDir = currentRemoteDir.left(currentRemoteDir.lastIndexOf("/"));
    if (parentDir.isEmpty()) parentDir = "/";

    // 切换到上级目录
    ui->statusLabel->setText(QString::fromLocal8Bit("正在返回上级目录..."));
    ftp->cd(parentDir);
    currentRemoteDir = parentDir;

    // 清空旧列表
    fileList.clear();
    ui->fileListWidget->clear();
}

// 更新文件列表（应用过滤）
void FtpClient::updateFileList()
{
    ui->fileListWidget->clear();

    foreach (const FileInfo &info, fileList) {
        // 目录始终显示，文件应用过滤
        if (info.isDir || currentFilter == "*" ||
            info.displayName.contains(QRegExp(currentFilter.replace("*.", "\\.").replace(";", "|")))) {

            QListWidgetItem *item = new QListWidgetItem(info.displayName);
            item->setIcon(info.isDir ? dirIcon : fileIcon);  // 设置图标
            item->setData(Qt::UserRole, info.isDir);  // 存储是否为目录
            ui->fileListWidget->addItem(item);
        }
    }
}
QString FtpClient::convertFileName2(const QByteArray &rawData)
{
    // 常见FTP服务器编码优先级：GBK(Windows)、UTF-8(Linux)、GB2312、Big5
    QList<QPair<QString, bool>> codecs = {
        {"GBK", true},    // Windows服务器常用
        {"UTF-8", false}, // Linux/Unix服务器常用
        {"GB2312", true},
        {"Big5", true}
    };

    for (auto &codecPair : codecs) {
        QTextCodec *codec = QTextCodec::codecForName(codecPair.first.toUtf8());
        if (!codec) continue;

        QString decoded = codec->toUnicode(rawData);
        // 校验：通过字符范围判断是否为合理中文（排除乱码）
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

    // 兜底：尝试系统本地编码
    return QString::fromLocal8Bit(rawData);
}
// 转换文件名编码（解决中文乱码）
QString FtpClient::convertFileName(const QByteArray &rawData)
{
    // 尝试GBK编码（Windows服务器常用）
    QTextCodec *gbkCodec = QTextCodec::codecForName("GBK");
    QString gbkStr = gbkCodec ? gbkCodec->toUnicode(rawData) : "";

    // 尝试UTF-8编码（Linux服务器常用）
    QString utf8Str = QString::fromUtf8(rawData);

    // 判断哪种编码更可能正确（含中文字符）
    if (gbkStr.contains(QRegExp("[\\u4e00-\\u9fa5]"))) {
        return gbkStr;
    } else if (utf8Str.contains(QRegExp("[\\u4e00-\\u9fa5]"))) {
        return utf8Str;
    }

    // 非中文则返回原始字符串
    return QString::fromLatin1(rawData);
}

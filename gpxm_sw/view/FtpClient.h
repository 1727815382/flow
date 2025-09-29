#ifndef FTPCLIENT_H
#define FTPCLIENT_H

#include <QDialog>
#include "qftp/qftp.h"
#include <QUrl>
#include <QProgressDialog>
#include <QFile>
#include <QListWidgetItem>
#include <QIcon>

QT_BEGIN_NAMESPACE
namespace Ui { class FtpClient; }
QT_END_NAMESPACE

class FtpClient : public QDialog
{
    Q_OBJECT

public:
    FtpClient(QWidget *parent = nullptr);
    ~FtpClient();

    void onConnectServer();
    void SetDownloadDir(QString path);

    QString convertFileName2(const QByteArray &rawData);
private slots:
    void on_connectButton_clicked();
    void on_disconnectButton_clicked();
    void on_downloadButton_clicked();
    void on_browseButton_clicked();
    void on_fileTypeCombo_currentIndexChanged(int index);
    void on_ftpCommandFinished(int id, bool error);
    void on_ftpListInfo(const QUrlInfo &urlInfo);
    void on_ftpDataTransferProgress(qint64 readBytes, qint64 totalBytes);
    void on_ftpStateChanged(int state);
    void on_ExitButton_clicked();
    void on_itemDoubleClicked(QListWidgetItem *item);  // 双击目录切换
    void on_goUpButton_clicked();  // 返回上级目录

private:
    Ui::FtpClient *ui;
    QFtp *ftp;
    QString currentDownloadDir;
    QString currentFilter;
    QFile *downloadFile;
    QProgressDialog *progressDialog;

    // 文件/目录信息结构体
    struct FileInfo {
        QString originalName;  // 服务器原始名称（用于FTP操作）
        QString displayName;   // 转换后显示名称
        bool isDir;            // 是否为目录
        qint64 size;           // 文件大小
    };
    QList<FileInfo> fileList;

    QString currentRemoteDir;  // 当前远程目录路径
    QIcon fileIcon;            // 文件图标
    QIcon dirIcon;             // 目录图标

    void updateFileList();
    QString convertFileName(const QByteArray &rawData);
    void closeEvent(QCloseEvent *event) override;
};
#endif // FTPCLIENT_H

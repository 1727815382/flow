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
    void on_itemDoubleClicked(QListWidgetItem *item);  // ˫��Ŀ¼�л�
    void on_goUpButton_clicked();  // �����ϼ�Ŀ¼

private:
    Ui::FtpClient *ui;
    QFtp *ftp;
    QString currentDownloadDir;
    QString currentFilter;
    QFile *downloadFile;
    QProgressDialog *progressDialog;

    // �ļ�/Ŀ¼��Ϣ�ṹ��
    struct FileInfo {
        QString originalName;  // ������ԭʼ���ƣ�����FTP������
        QString displayName;   // ת������ʾ����
        bool isDir;            // �Ƿ�ΪĿ¼
        qint64 size;           // �ļ���С
    };
    QList<FileInfo> fileList;

    QString currentRemoteDir;  // ��ǰԶ��Ŀ¼·��
    QIcon fileIcon;            // �ļ�ͼ��
    QIcon dirIcon;             // Ŀ¼ͼ��

    void updateFileList();
    QString convertFileName(const QByteArray &rawData);
    void closeEvent(QCloseEvent *event) override;
};
#endif // FTPCLIENT_H

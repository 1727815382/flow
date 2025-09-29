#ifndef TRACTORDEVICE_H
#define TRACTORDEVICE_H

#include <QObject>
#include <QProcess>
#include <QTimer>

class TractorDevice:public QObject
{
    Q_OBJECT
public:
    TractorDevice(QString devIp = "192.168.1.100",QObject *parent = 0);
    void SetDevIp(QString devIp,QString devIp2);
    void Reset();
    //    void doWork();
    void UpdateStatus2();
    void UpdateStatus1();
public slots:
    void doWork();
    void UpdateStatus(int devIndex);
signals:
    void SignDevOnLineStatus(bool flg);
    void SignDevOnLineStatus2(bool flg);
private:
    bool m_isExit;
    QString m_ip;
    bool m_inline;
    int m_count ;
    QTimer m_pTimer;

    QString m_ip2;
    bool m_inline2;
    int m_count2 ;
    QProcess *m_proc1;
    QProcess *m_proc2;
    int m_devIndex;

};

#endif // TRACTORDEVICE_H

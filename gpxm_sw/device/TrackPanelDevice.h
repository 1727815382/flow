#ifndef TRACKPANELDEVICE_H
#define TRACKPANELDEVICE_H

#include <QObject>
#include <QTimer>

class TrackPanelDevice:public QObject
{
    Q_OBJECT
public:
    TrackPanelDevice(QString devIp ,QObject *parent = 0 );
    void Reset();
    void doWork();
    void SetDevIp(QString devIp);
public slots:
    void UpdateStatus();
signals:
    void SignDevOnLineStatus(bool);
private:
    bool m_isExit;
    QString m_ip;
    bool m_inline;
    QTimer m_pTimer;
    int m_count ;
};

#endif // TRACKPANELDEVICE_H

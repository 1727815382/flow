#include "TrackPanelDevice.h"
#include "QProcess"
#include <QThread>
#include <QtDebug>


TrackPanelDevice::TrackPanelDevice(QString devIp, QObject *parent):QObject(parent)
{
    m_isExit = false;
    m_inline = false;
    m_ip = devIp;
    connect(&m_pTimer,SIGNAL(timeout()),this,SLOT(UpdateStatus()));
    m_count = 0;
//    m_pTimer.start(7000);
}

void TrackPanelDevice::Reset()
{
    m_isExit = true;
    m_inline = false;

}
void TrackPanelDevice::SetDevIp(QString devIp)
{
     m_ip = devIp;
}

void TrackPanelDevice::doWork()
{
    while(1)
    {
        UpdateStatus();
        QThread::sleep(2000);
    }
}
void TrackPanelDevice::UpdateStatus()
{
//    if(!m_isExit)
    {
        qDebug()<<"===TrackPanelDevice222222=======\n";
        QProcess proc;
        proc.start("ping "+m_ip);
        proc.waitForFinished(5000);
        QString response = proc.readAll();
        if(response.contains("TTL="))
        {
             qDebug()<<"====TrackPanelDevice444444444444=======\n";
            m_count = 0;
            if(!m_inline)
            {
                emit SignDevOnLineStatus(true);
                m_inline = true;
            }
        }
        else
        {
            qDebug()<<"====TrackPanelDevice33333333333=======\n"<<m_ip << m_count <<m_inline;
            m_count++;
            if(m_inline && m_count >= 3)
            {
                m_count = 3;
                emit SignDevOnLineStatus(false);
                m_inline = false;
            }
        }
    }
}


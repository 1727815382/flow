#include "TractorDevice.h"

#include <QProcess>
#include <QStringlist>
#include <QThread>
#include <QtDebug>
TractorDevice::TractorDevice(QString devIp, QObject *parent):QObject(parent)
{
    m_isExit = false;
    m_inline = false;
    m_ip = devIp;
    m_count = 0;

    m_inline2 = false;
    m_ip2 = devIp;
    m_count2 = 0;
    m_devIndex = 0;
    connect(&m_pTimer,SIGNAL(timeout()),this,SLOT(doWork()));
    m_pTimer.start(5000);
}

void TractorDevice::SetDevIp(QString devIp, QString devIp2)
{
    m_ip = devIp;
    m_ip2 = devIp2;
}

void TractorDevice::Reset()
{
    m_isExit = true;
    m_inline = false;
}

void TractorDevice::doWork()
{
    ////    while(1)
    //    {
    //        UpdateStatus(1);
    ////        QThread::sleep(1000);
    //        UpdateStatus(2);
    ////        QThread::sleep(1000);
    //    }

    if(m_devIndex == 0)
    {
        UpdateStatus1();
        m_devIndex = 1;
    }
    else
    {
        UpdateStatus2();
        m_devIndex = 0;
    }
}

//void TractorDevice::UpdateStatus(int devIndex)
//{
//    qDebug()<<"====222222=======\n";
//    QProcess *proc = new QProcess(this);
//    QProcess *proc2 = new QProcess(this);
//    if(devIndex == 1)
//    {
//        QString cmd = QString("-n 1 %1\n").arg(m_ip);
//        proc->start("ping",cmd);
//        qDebug()<<cmd;
//        proc.waitForFinished(500);
//        QString response = proc.readAll();
//        if(response.contains("TTL="))
//        {
//            qDebug()<<"====444444444444=======\n";
//            m_count = 0;
//            if(!m_inline)
//            {
//                emit SignDevOnLineStatus(true);
//                m_inline = true;
//            }
//        }
//        else
//        {
//            qDebug()<<"====33333333333=======\n"<<m_ip << m_count <<m_inline;
//            m_count++;
//            if(m_inline && m_count >= 3)
//            {
//                m_count = 3;
//                emit SignDevOnLineStatus(false);
//                m_inline = false;
//            }
//        }
//    }
//    else if(devIndex == 2)
//    {
//        QString cmd = QString("ping -n 1 %1\n").arg(m_ip2);
//        proc.start(cmd);
//        qDebug()<<cmd;
//        qDebug()<<"========\n"<<m_ip2 << m_count2 <<m_inline2;
//        proc.start(cmd);
////        proc.waitForFinished(500);
//        QString response = proc.readAll();
//        if(response.contains("TTL="))
//        {
//            m_count2 = 0;
//            if(!m_inline2)
//            {
//                emit SignDevOnLineStatus2(true);
//                m_inline2 = true;
//            }
//        }
//        else
//        {

//            m_count2++;
//            if(m_inline2 && m_count2 >= 3)
//            {
//                m_count2 = 3;
//                emit SignDevOnLineStatus2(false);
//                m_inline2 = false;
//            }
//        }
//    }
//    proc->deleteLater();
//}


void TractorDevice::UpdateStatus(int devIndex)
{
    //    qDebug()<<"====222222=======\n";
    //    QProcess *proc = new QProcess(this);
    //    QProcess *proc2 = new QProcess(this);
    //    if(devIndex == 1)
    //    {
    //        QString cmd = QString("-n 1 %1\n").arg(m_ip);
    //        proc->start("ping",cmd);
    //        qDebug()<<cmd;
    //        proc.waitForFinished(500);
    //        QString response = proc.readAll();
    //        if(response.contains("TTL="))
    //        {
    //            qDebug()<<"====444444444444=======\n";
    //            m_count = 0;
    //            if(!m_inline)
    //            {
    //                emit SignDevOnLineStatus(true);
    //                m_inline = true;
    //            }
    //        }
    //        else
    //        {
    //            qDebug()<<"====33333333333=======\n"<<m_ip << m_count <<m_inline;
    //            m_count++;
    //            if(m_inline && m_count >= 3)
    //            {
    //                m_count = 3;
    //                emit SignDevOnLineStatus(false);
    //                m_inline = false;
    //            }
    //        }
    //    }
    //    else if(devIndex == 2)
    //    {
    //        QString cmd = QString("ping -n 1 %1\n").arg(m_ip2);
    //        proc.start(cmd);
    //        qDebug()<<cmd;
    //        qDebug()<<"========\n"<<m_ip2 << m_count2 <<m_inline2;
    //        proc.start(cmd);
    ////        proc.waitForFinished(500);
    //        QString response = proc.readAll();
    //        if(response.contains("TTL="))
    //        {
    //            m_count2 = 0;
    //            if(!m_inline2)
    //            {
    //                emit SignDevOnLineStatus2(true);
    //                m_inline2 = true;
    //            }
    //        }
    //        else
    //        {

    //            m_count2++;
    //            if(m_inline2 && m_count2 >= 3)
    //            {
    //                m_count2 = 3;
    //                emit SignDevOnLineStatus2(false);
    //                m_inline2 = false;
    //            }
    //        }
    //    }
    //    proc->deleteLater();
}

void TractorDevice::UpdateStatus1()
{
    qDebug()<<"====222222=======\n";
    m_proc1 = new QProcess(this);

    QStringList cmd /*= QString("-n 1 %1\n").arg(m_ip)*/;
    cmd <<"-n"<<"1";
    cmd<<m_ip;
    m_proc1->start("ping",cmd);
    qDebug()<<cmd;
    m_proc1->waitForFinished(500);
    QString response = m_proc1->readAll();
    if(response.contains("TTL="))
    {
        qDebug()<<"====444444444444=======\n";
        m_count = 0;
        if(!m_inline)
        {
            emit SignDevOnLineStatus(true);
            m_inline = true;
        }
    }
    else
    {
        qDebug()<<"====33333333333=======\n"<<m_ip << m_count <<m_inline;
        m_count++;
        if(m_inline && m_count >= 3)
        {
            m_count = 3;
            emit SignDevOnLineStatus(false);
            m_inline = false;
        }
    }
    m_proc1->close();
    m_proc1->deleteLater();
    m_proc1->kill();
    delete m_proc1;
}

void TractorDevice::UpdateStatus2()
{
    qDebug()<<"====222222=======\n";
    m_proc2 = new QProcess(this);
    QStringList cmd /*= QString("-n 1 %1\n").arg(m_ip)*/;
    cmd <<"-n"<<"1";
    cmd<<m_ip2;
    m_proc2->start("ping",cmd);
    qDebug()<<cmd;
    m_proc2->waitForFinished(500);
    QString response = m_proc2->readAll();
    if(response.contains("TTL="))
    {
        m_count2 = 0;
        if(!m_inline2)
        {
            emit SignDevOnLineStatus2(true);
            m_inline2 = true;
        }
    }
    else
    {

        m_count2++;
        if(m_inline2 && m_count2 >= 3)
        {
            m_count2 = 3;
            emit SignDevOnLineStatus2(false);
            m_inline2 = false;
        }
    }
    m_proc2->close();
    m_proc2->deleteLater();
    m_proc2->kill();
    delete m_proc2;
}

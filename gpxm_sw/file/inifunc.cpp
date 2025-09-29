#include "inifunc.h"
#include <QDebug>

IniFunc::IniFunc()
{
    m_Settings = new QSettings(QString::fromLocal8Bit("²ÎÊýÉèÖÃ.ini"),QSettings::IniFormat);
}

IniFunc::IniFunc(QString filename)
{
    m_Settings = new QSettings(filename,QSettings::IniFormat);
    QStringList keys = m_Settings->allKeys();

}

QString IniFunc::getConfigValue(QString section,QString key)
{
    QString path = section+"/"+key;
    QString val = m_Settings->value(path, "").toString();
    return val;
}

void IniFunc::setConfigValue(QString section, QString key, QString val)
{
    QString path = section+"//"+key;
    m_Settings->setValue(path,val);
}

#ifndef INIFUNC_H
#define INIFUNC_H

#include <QObject>
#include <QString>
#include <QSettings>

class IniFunc
{
public:
    IniFunc();
    IniFunc(QString filename);
    QString getConfigValue(QString section, QString key);
    void setConfigValue(QString section,QString key,QString val);

private:
    QSettings* m_Settings;
};

#endif // INIFUNC_H

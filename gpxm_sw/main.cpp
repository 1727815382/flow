#include "view/mainwidget.h"
#include <QApplication>
#include <QFile>
#include <QTextStream>
#include <QDebug>
#include <QTextCodec>
#include "aixsim.h"


ConfigPara m_configPara;

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);


    QString filePath (":/res/img/light.css");

    QFile file(filePath);
    if (file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QTextStream stream(&file);
        QString styleSheet = stream.readAll();
        qApp->setStyleSheet(styleSheet);
        file.close();
    }


    else
    {
        qWarning() << "Failed to load stylesheet from:" << filePath;
    }

    MainWidget w;
    w.showMaximized();


    return a.exec();
}

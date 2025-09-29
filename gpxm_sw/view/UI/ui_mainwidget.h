/********************************************************************************
** Form generated from reading UI file 'mainwidget.ui'
**
** Created by: Qt User Interface Compiler version 5.7.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWIDGET_H
#define UI_MAINWIDGET_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>
#include <SystemConfigForm.h>
#include <Tuingform.h>
#include <WorkForm.h>
#include <configform.h>
#include <dataform.h>
#include "multiplcclientform.h"

QT_BEGIN_NAMESPACE

class Ui_MainWidget
{
public:
    QVBoxLayout *verticalLayout;
    QTabWidget *tabWidget;
    QWidget *tab;
    QGridLayout *gridLayout_2;
    WorkForm *workWgt;
    QWidget *tab_2;
    QGridLayout *gridLayout_3;
    ConfigForm *ConfigWgt;
    QWidget *tab_3;
    QGridLayout *gridLayout_5;
    DataForm *DataWgt;
    QWidget *tab_4;
    QGridLayout *gridLayout_4;
    TuingForm *TuingWgt;
    QWidget *tab_5;
    QHBoxLayout *horizontalLayout;
    MultiPlcClientForm *widget;
    QWidget *tab_6;
    QGridLayout *gridLayout;
    SystemConfigForm *SysCfgWgt;

    void setupUi(QWidget *MainWidget)
    {
        if (MainWidget->objectName().isEmpty())
            MainWidget->setObjectName(QStringLiteral("MainWidget"));
        MainWidget->resize(1081, 601);
        verticalLayout = new QVBoxLayout(MainWidget);
        verticalLayout->setSpacing(5);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        verticalLayout->setContentsMargins(0, 0, 0, 0);
        tabWidget = new QTabWidget(MainWidget);
        tabWidget->setObjectName(QStringLiteral("tabWidget"));
        QFont font;
        font.setPointSize(16);
        tabWidget->setFont(font);
        tabWidget->setStyleSheet(QStringLiteral(""));
        tab = new QWidget();
        tab->setObjectName(QStringLiteral("tab"));
        gridLayout_2 = new QGridLayout(tab);
        gridLayout_2->setObjectName(QStringLiteral("gridLayout_2"));
        gridLayout_2->setContentsMargins(-1, 3, -1, -1);
        workWgt = new WorkForm(tab);
        workWgt->setObjectName(QStringLiteral("workWgt"));

        gridLayout_2->addWidget(workWgt, 0, 0, 1, 1);

        tabWidget->addTab(tab, QString());
        tab_2 = new QWidget();
        tab_2->setObjectName(QStringLiteral("tab_2"));
        gridLayout_3 = new QGridLayout(tab_2);
        gridLayout_3->setObjectName(QStringLiteral("gridLayout_3"));
        gridLayout_3->setContentsMargins(-1, 3, -1, -1);
        ConfigWgt = new ConfigForm(tab_2);
        ConfigWgt->setObjectName(QStringLiteral("ConfigWgt"));

        gridLayout_3->addWidget(ConfigWgt, 0, 0, 1, 1);

        tabWidget->addTab(tab_2, QString());
        tab_3 = new QWidget();
        tab_3->setObjectName(QStringLiteral("tab_3"));
        gridLayout_5 = new QGridLayout(tab_3);
        gridLayout_5->setObjectName(QStringLiteral("gridLayout_5"));
        gridLayout_5->setContentsMargins(-1, 3, -1, -1);
        DataWgt = new DataForm(tab_3);
        DataWgt->setObjectName(QStringLiteral("DataWgt"));

        gridLayout_5->addWidget(DataWgt, 0, 0, 1, 1);

        tabWidget->addTab(tab_3, QString());
        tab_4 = new QWidget();
        tab_4->setObjectName(QStringLiteral("tab_4"));
        gridLayout_4 = new QGridLayout(tab_4);
        gridLayout_4->setObjectName(QStringLiteral("gridLayout_4"));
        gridLayout_4->setContentsMargins(-1, 3, -1, -1);
        TuingWgt = new TuingForm(tab_4);
        TuingWgt->setObjectName(QStringLiteral("TuingWgt"));

        gridLayout_4->addWidget(TuingWgt, 0, 0, 1, 1);

        tabWidget->addTab(tab_4, QString());
        tab_5 = new QWidget();
        tab_5->setObjectName(QStringLiteral("tab_5"));
        horizontalLayout = new QHBoxLayout(tab_5);
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        horizontalLayout->setContentsMargins(-1, 3, -1, -1);
        widget = new MultiPlcClientForm(tab_5);
        widget->setObjectName(QStringLiteral("widget"));

        horizontalLayout->addWidget(widget);

        tabWidget->addTab(tab_5, QString());
        tab_6 = new QWidget();
        tab_6->setObjectName(QStringLiteral("tab_6"));
        gridLayout = new QGridLayout(tab_6);
        gridLayout->setObjectName(QStringLiteral("gridLayout"));
        gridLayout->setContentsMargins(-1, 3, -1, -1);
        SysCfgWgt = new SystemConfigForm(tab_6);
        SysCfgWgt->setObjectName(QStringLiteral("SysCfgWgt"));

        gridLayout->addWidget(SysCfgWgt, 0, 0, 1, 1);

        tabWidget->addTab(tab_6, QString());

        verticalLayout->addWidget(tabWidget);


        retranslateUi(MainWidget);

        tabWidget->setCurrentIndex(0);


        QMetaObject::connectSlotsByName(MainWidget);
    } // setupUi

    void retranslateUi(QWidget *MainWidget)
    {
        MainWidget->setWindowTitle(QApplication::translate("MainWidget", "Form", Q_NULLPTR));
#ifndef QT_NO_WHATSTHIS
        tabWidget->setWhatsThis(QApplication::translate("MainWidget", "<html><head/><body><p>font: 75 10pt &quot;\345\276\256\350\275\257\351\233\205\351\273\221 Light&quot;;</p></body></html>", Q_NULLPTR));
#endif // QT_NO_WHATSTHIS
        tabWidget->setTabText(tabWidget->indexOf(tab), QApplication::translate("MainWidget", "\345\267\245\344\275\234\347\225\214\351\235\242", Q_NULLPTR));
        tabWidget->setTabText(tabWidget->indexOf(tab_2), QApplication::translate("MainWidget", "\345\217\202\346\225\260\351\205\215\347\275\256", Q_NULLPTR));
        tabWidget->setTabText(tabWidget->indexOf(tab_3), QApplication::translate("MainWidget", "\350\275\250\346\216\222\346\225\260\346\215\256", Q_NULLPTR));
        tabWidget->setTabText(tabWidget->indexOf(tab_4), QApplication::translate("MainWidget", "\347\262\276\350\260\203\346\225\260\346\215\256", Q_NULLPTR));
        tabWidget->setTabText(tabWidget->indexOf(tab_5), QApplication::translate("MainWidget", "\347\224\265\346\234\272\350\260\203\350\257\225", Q_NULLPTR));
        tabWidget->setTabText(tabWidget->indexOf(tab_6), QApplication::translate("MainWidget", " \347\263\273\347\273\237\351\205\215\347\275\256", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class MainWidget: public Ui_MainWidget {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWIDGET_H

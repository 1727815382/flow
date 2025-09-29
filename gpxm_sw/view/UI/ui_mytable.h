/********************************************************************************
** Form generated from reading UI file 'mytable.ui'
**
** Created by: Qt User Interface Compiler version 5.7.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MYTABLE_H
#define UI_MYTABLE_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QTableWidget>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_Mytable
{
public:
    QGridLayout *gridLayout;
    QTableWidget *tableWidget;

    void setupUi(QWidget *Mytable)
    {
        if (Mytable->objectName().isEmpty())
            Mytable->setObjectName(QStringLiteral("Mytable"));
        Mytable->resize(862, 508);
        gridLayout = new QGridLayout(Mytable);
        gridLayout->setSpacing(0);
        gridLayout->setObjectName(QStringLiteral("gridLayout"));
        gridLayout->setContentsMargins(0, 0, 0, 0);
        tableWidget = new QTableWidget(Mytable);
        tableWidget->setObjectName(QStringLiteral("tableWidget"));
        tableWidget->setStyleSheet(QStringLiteral(""));
        tableWidget->verticalHeader()->setDefaultSectionSize(50);

        gridLayout->addWidget(tableWidget, 0, 0, 1, 1);


        retranslateUi(Mytable);

        QMetaObject::connectSlotsByName(Mytable);
    } // setupUi

    void retranslateUi(QWidget *Mytable)
    {
        Mytable->setWindowTitle(QApplication::translate("Mytable", "Form", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class Mytable: public Ui_Mytable {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MYTABLE_H

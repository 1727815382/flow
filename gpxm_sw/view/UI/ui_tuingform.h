/********************************************************************************
** Form generated from reading UI file 'tuingform.ui'
**
** Created by: Qt User Interface Compiler version 5.7.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_TUINGFORM_H
#define UI_TUINGFORM_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>
#include <mytable.h>
#include "tqrcalculationform.h"

QT_BEGIN_NAMESPACE

class Ui_TuingForm
{
public:
    QVBoxLayout *verticalLayout;
    QHBoxLayout *horizontalLayout;
    TQRCalculationForm *widget;
    QVBoxLayout *verticalLayout_18;
    QSpacerItem *verticalSpacer;
    QPushButton *pushButton_29;
    QPushButton *pushButton_downLoad;
    QSpacerItem *verticalSpacer_2;
    Mytable *tuing;

    void setupUi(QWidget *TuingForm)
    {
        if (TuingForm->objectName().isEmpty())
            TuingForm->setObjectName(QStringLiteral("TuingForm"));
        TuingForm->resize(955, 487);
        verticalLayout = new QVBoxLayout(TuingForm);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        widget = new TQRCalculationForm(TuingForm);
        widget->setObjectName(QStringLiteral("widget"));
        QFont font;
        font.setPointSize(14);
        widget->setFont(font);

        horizontalLayout->addWidget(widget);

        verticalLayout_18 = new QVBoxLayout();
        verticalLayout_18->setSpacing(20);
        verticalLayout_18->setObjectName(QStringLiteral("verticalLayout_18"));
        verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout_18->addItem(verticalSpacer);

        pushButton_29 = new QPushButton(TuingForm);
        pushButton_29->setObjectName(QStringLiteral("pushButton_29"));
        pushButton_29->setMinimumSize(QSize(0, 70));
        pushButton_29->setMaximumSize(QSize(200, 16777215));

        verticalLayout_18->addWidget(pushButton_29);

        pushButton_downLoad = new QPushButton(TuingForm);
        pushButton_downLoad->setObjectName(QStringLiteral("pushButton_downLoad"));
        pushButton_downLoad->setMinimumSize(QSize(0, 70));
        pushButton_downLoad->setMaximumSize(QSize(200, 16777215));
        pushButton_downLoad->setStyleSheet(QStringLiteral(""));

        verticalLayout_18->addWidget(pushButton_downLoad);

        verticalSpacer_2 = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout_18->addItem(verticalSpacer_2);


        horizontalLayout->addLayout(verticalLayout_18);

        horizontalLayout->setStretch(0, 1);

        verticalLayout->addLayout(horizontalLayout);

        tuing = new Mytable(TuingForm);
        tuing->setObjectName(QStringLiteral("tuing"));

        verticalLayout->addWidget(tuing);

        verticalLayout->setStretch(0, 3);
        verticalLayout->setStretch(1, 7);
        tuing->raise();

        retranslateUi(TuingForm);

        QMetaObject::connectSlotsByName(TuingForm);
    } // setupUi

    void retranslateUi(QWidget *TuingForm)
    {
        TuingForm->setWindowTitle(QApplication::translate("TuingForm", "Form", Q_NULLPTR));
        pushButton_29->setText(QApplication::translate("TuingForm", "\346\211\223\345\274\200\346\225\260\346\215\256", Q_NULLPTR));
        pushButton_downLoad->setText(QApplication::translate("TuingForm", "\344\270\213\350\275\275\346\225\260\346\215\256", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class TuingForm: public Ui_TuingForm {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_TUINGFORM_H

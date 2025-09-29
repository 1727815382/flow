/********************************************************************************
** Form generated from reading UI file 'tqrcalculationform.ui'
**
** Created by: Qt User Interface Compiler version 5.7.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_TQRCALCULATIONFORM_H
#define UI_TQRCALCULATIONFORM_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_TQRCalculationForm
{
public:
    QGridLayout *gridLayout_2;
    QSpacerItem *verticalSpacer;
    QSpacerItem *verticalSpacer_2;
    QSpacerItem *horizontalSpacer;
    QSpacerItem *horizontalSpacer_2;
    QWidget *widget;
    QGridLayout *gridLayout;
    QLabel *label;
    QLineEdit *lineEdit_TQI_GJPC;
    QLabel *label_6;
    QLineEdit *lineEdit_LTQI_GDPC;
    QLabel *label_2;
    QLineEdit *lineEdit_TQI_NQ;
    QLabel *label_7;
    QLineEdit *lineEdit_LTQI_GXPC;
    QLabel *label_3;
    QLineEdit *lineEdit_TQI_CGPC;
    QLabel *label_8;
    QLineEdit *lineEdit_RTQI_GDPC;
    QLabel *label_9;
    QLineEdit *lineEdit_TQI;
    QLabel *label_4;
    QLineEdit *lineEdit_RTQI_GXPC;

    void setupUi(QWidget *TQRCalculationForm)
    {
        if (TQRCalculationForm->objectName().isEmpty())
            TQRCalculationForm->setObjectName(QStringLiteral("TQRCalculationForm"));
        TQRCalculationForm->resize(761, 306);
        gridLayout_2 = new QGridLayout(TQRCalculationForm);
        gridLayout_2->setObjectName(QStringLiteral("gridLayout_2"));
        verticalSpacer = new QSpacerItem(20, 50, QSizePolicy::Minimum, QSizePolicy::Expanding);

        gridLayout_2->addItem(verticalSpacer, 2, 1, 1, 1);

        verticalSpacer_2 = new QSpacerItem(20, 50, QSizePolicy::Minimum, QSizePolicy::Expanding);

        gridLayout_2->addItem(verticalSpacer_2, 0, 1, 1, 1);

        horizontalSpacer = new QSpacerItem(104, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        gridLayout_2->addItem(horizontalSpacer, 1, 0, 1, 1);

        horizontalSpacer_2 = new QSpacerItem(103, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        gridLayout_2->addItem(horizontalSpacer_2, 1, 2, 1, 1);

        widget = new QWidget(TQRCalculationForm);
        widget->setObjectName(QStringLiteral("widget"));
        widget->setMinimumSize(QSize(0, 200));
        QFont font;
        font.setPointSize(14);
        widget->setFont(font);
        gridLayout = new QGridLayout(widget);
        gridLayout->setObjectName(QStringLiteral("gridLayout"));
        label = new QLabel(widget);
        label->setObjectName(QStringLiteral("label"));

        gridLayout->addWidget(label, 0, 0, 1, 1);

        lineEdit_TQI_GJPC = new QLineEdit(widget);
        lineEdit_TQI_GJPC->setObjectName(QStringLiteral("lineEdit_TQI_GJPC"));

        gridLayout->addWidget(lineEdit_TQI_GJPC, 0, 1, 1, 1);

        label_6 = new QLabel(widget);
        label_6->setObjectName(QStringLiteral("label_6"));

        gridLayout->addWidget(label_6, 0, 2, 1, 1);

        lineEdit_LTQI_GDPC = new QLineEdit(widget);
        lineEdit_LTQI_GDPC->setObjectName(QStringLiteral("lineEdit_LTQI_GDPC"));

        gridLayout->addWidget(lineEdit_LTQI_GDPC, 0, 3, 1, 1);

        label_2 = new QLabel(widget);
        label_2->setObjectName(QStringLiteral("label_2"));

        gridLayout->addWidget(label_2, 1, 0, 1, 1);

        lineEdit_TQI_NQ = new QLineEdit(widget);
        lineEdit_TQI_NQ->setObjectName(QStringLiteral("lineEdit_TQI_NQ"));

        gridLayout->addWidget(lineEdit_TQI_NQ, 1, 1, 1, 1);

        label_7 = new QLabel(widget);
        label_7->setObjectName(QStringLiteral("label_7"));

        gridLayout->addWidget(label_7, 1, 2, 1, 1);

        lineEdit_LTQI_GXPC = new QLineEdit(widget);
        lineEdit_LTQI_GXPC->setObjectName(QStringLiteral("lineEdit_LTQI_GXPC"));

        gridLayout->addWidget(lineEdit_LTQI_GXPC, 1, 3, 1, 1);

        label_3 = new QLabel(widget);
        label_3->setObjectName(QStringLiteral("label_3"));

        gridLayout->addWidget(label_3, 2, 0, 1, 1);

        lineEdit_TQI_CGPC = new QLineEdit(widget);
        lineEdit_TQI_CGPC->setObjectName(QStringLiteral("lineEdit_TQI_CGPC"));

        gridLayout->addWidget(lineEdit_TQI_CGPC, 2, 1, 1, 1);

        label_8 = new QLabel(widget);
        label_8->setObjectName(QStringLiteral("label_8"));

        gridLayout->addWidget(label_8, 2, 2, 1, 1);

        lineEdit_RTQI_GDPC = new QLineEdit(widget);
        lineEdit_RTQI_GDPC->setObjectName(QStringLiteral("lineEdit_RTQI_GDPC"));

        gridLayout->addWidget(lineEdit_RTQI_GDPC, 2, 3, 1, 1);

        label_9 = new QLabel(widget);
        label_9->setObjectName(QStringLiteral("label_9"));

        gridLayout->addWidget(label_9, 3, 0, 1, 1);

        lineEdit_TQI = new QLineEdit(widget);
        lineEdit_TQI->setObjectName(QStringLiteral("lineEdit_TQI"));

        gridLayout->addWidget(lineEdit_TQI, 3, 1, 1, 1);

        label_4 = new QLabel(widget);
        label_4->setObjectName(QStringLiteral("label_4"));

        gridLayout->addWidget(label_4, 3, 2, 1, 1);

        lineEdit_RTQI_GXPC = new QLineEdit(widget);
        lineEdit_RTQI_GXPC->setObjectName(QStringLiteral("lineEdit_RTQI_GXPC"));
        QFont font1;
        font1.setPointSize(13);
        lineEdit_RTQI_GXPC->setFont(font1);

        gridLayout->addWidget(lineEdit_RTQI_GXPC, 3, 3, 1, 1);


        gridLayout_2->addWidget(widget, 1, 1, 1, 1);


        retranslateUi(TQRCalculationForm);

        QMetaObject::connectSlotsByName(TQRCalculationForm);
    } // setupUi

    void retranslateUi(QWidget *TQRCalculationForm)
    {
        TQRCalculationForm->setWindowTitle(QApplication::translate("TQRCalculationForm", "Form", Q_NULLPTR));
        label->setText(QApplication::translate("TQRCalculationForm", "\350\275\250\350\267\235\345\201\217\345\267\256TQI", Q_NULLPTR));
        label_6->setText(QApplication::translate("TQRCalculationForm", "\351\253\230\344\275\2161\345\201\217\345\267\256LTQ1", Q_NULLPTR));
        label_2->setText(QApplication::translate("TQRCalculationForm", "\346\211\255\346\233\2622.5TQI", Q_NULLPTR));
        label_7->setText(QApplication::translate("TQRCalculationForm", "\350\275\250\345\220\2211\345\201\217\345\267\256LTQ1", Q_NULLPTR));
        label_3->setText(QApplication::translate("TQRCalculationForm", "\350\266\205\351\253\230\345\201\217\345\267\256TQI", Q_NULLPTR));
        label_8->setText(QApplication::translate("TQRCalculationForm", "\351\253\230\344\275\2161\345\201\217\345\267\256RTQ1", Q_NULLPTR));
        label_9->setText(QApplication::translate("TQRCalculationForm", "TQI", Q_NULLPTR));
        label_4->setText(QApplication::translate("TQRCalculationForm", "\350\275\250\345\220\2211\345\201\217\345\267\256RTQI", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class TQRCalculationForm: public Ui_TQRCalculationForm {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_TQRCALCULATIONFORM_H

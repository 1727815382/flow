/********************************************************************************
** Form generated from reading UI file 'configform.ui'
**
** Created by: Qt User Interface Compiler version 5.7.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_CONFIGFORM_H
#define UI_CONFIGFORM_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>
#include <customlineedit.h>

QT_BEGIN_NAMESPACE

class Ui_ConfigForm
{
public:
    QHBoxLayout *horizontalLayout_19;
    QVBoxLayout *verticalLayout_2;
    QWidget *widget_2;
    QHBoxLayout *horizontalLayout_14;
    QVBoxLayout *verticalLayout_3;
    QLabel *picLabel;
    QWidget *widget;
    QVBoxLayout *verticalLayout;
    QGridLayout *gridLayout_6;
    QHBoxLayout *horizontalLayout_6;
    QLabel *label_13;
    CustomLineEdit *D1_Edit;
    QLabel *label_22;
    QHBoxLayout *horizontalLayout_15;
    QLabel *label_14;
    CustomLineEdit *D2_Edit;
    QLabel *label_23;
    QHBoxLayout *horizontalLayout_5;
    QLabel *label_35;
    CustomLineEdit *ElevitionR_Edit;
    QLabel *label_48;
    QHBoxLayout *horizontalLayout_22;
    QLabel *label_18;
    CustomLineEdit *CLine_Edit;
    QLabel *label_27;
    QHBoxLayout *horizontalLayout_4;
    QLabel *label_34;
    CustomLineEdit *CLineS_Edit;
    QLabel *label_43;
    QHBoxLayout *horizontalLayout_21;
    QLabel *label_17;
    CustomLineEdit *ElevationTravel_Edit;
    QLabel *label_25;
    QHBoxLayout *horizontalLayout_16;
    QLabel *label_15;
    CustomLineEdit *D_Edit;
    QLabel *label_24;
    QHBoxLayout *horizontalLayout_20;
    QLabel *label_16;
    CustomLineEdit *RailNum_Edit;
    QLabel *label_26;
    QHBoxLayout *horizontalLayout_3;
    QLabel *label_33;
    CustomLineEdit *ElevitionL_Edit;
    QLabel *label_47;
    QHBoxLayout *horizontalLayout_24;
    QHBoxLayout *horizontalLayout_25;
    QPushButton *btnSave;
    QVBoxLayout *verticalLayout_4;

    void setupUi(QWidget *ConfigForm)
    {
        if (ConfigForm->objectName().isEmpty())
            ConfigForm->setObjectName(QStringLiteral("ConfigForm"));
        ConfigForm->resize(1372, 799);
        horizontalLayout_19 = new QHBoxLayout(ConfigForm);
        horizontalLayout_19->setSpacing(20);
        horizontalLayout_19->setObjectName(QStringLiteral("horizontalLayout_19"));
        verticalLayout_2 = new QVBoxLayout();
        verticalLayout_2->setObjectName(QStringLiteral("verticalLayout_2"));
        widget_2 = new QWidget(ConfigForm);
        widget_2->setObjectName(QStringLiteral("widget_2"));
        widget_2->setMinimumSize(QSize(0, 0));
        widget_2->setStyleSheet(QStringLiteral(""));
        horizontalLayout_14 = new QHBoxLayout(widget_2);
        horizontalLayout_14->setObjectName(QStringLiteral("horizontalLayout_14"));
        verticalLayout_3 = new QVBoxLayout();
        verticalLayout_3->setObjectName(QStringLiteral("verticalLayout_3"));
        verticalLayout_3->setContentsMargins(400, -1, 400, -1);
        picLabel = new QLabel(widget_2);
        picLabel->setObjectName(QStringLiteral("picLabel"));
        picLabel->setMinimumSize(QSize(500, 300));
        picLabel->setStyleSheet(QStringLiteral(""));
        picLabel->setFrameShape(QFrame::StyledPanel);

        verticalLayout_3->addWidget(picLabel);


        horizontalLayout_14->addLayout(verticalLayout_3);


        verticalLayout_2->addWidget(widget_2);

        widget = new QWidget(ConfigForm);
        widget->setObjectName(QStringLiteral("widget"));
        verticalLayout = new QVBoxLayout(widget);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        verticalLayout->setContentsMargins(20, -1, 20, -1);
        gridLayout_6 = new QGridLayout();
        gridLayout_6->setObjectName(QStringLiteral("gridLayout_6"));
        gridLayout_6->setHorizontalSpacing(0);
        gridLayout_6->setVerticalSpacing(40);
        gridLayout_6->setContentsMargins(0, -1, 0, -1);
        horizontalLayout_6 = new QHBoxLayout();
        horizontalLayout_6->setSpacing(20);
        horizontalLayout_6->setObjectName(QStringLiteral("horizontalLayout_6"));
        horizontalLayout_6->setContentsMargins(250, -1, 50, -1);
        label_13 = new QLabel(widget);
        label_13->setObjectName(QStringLiteral("label_13"));
        label_13->setMinimumSize(QSize(150, 0));
        label_13->setMaximumSize(QSize(150, 16777215));
        label_13->setStyleSheet(QStringLiteral(""));
        label_13->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        horizontalLayout_6->addWidget(label_13);

        D1_Edit = new CustomLineEdit(widget);
        D1_Edit->setObjectName(QStringLiteral("D1_Edit"));
        D1_Edit->setMinimumSize(QSize(0, 30));
        D1_Edit->setMaximumSize(QSize(130, 16777215));

        horizontalLayout_6->addWidget(D1_Edit);

        label_22 = new QLabel(widget);
        label_22->setObjectName(QStringLiteral("label_22"));
        label_22->setMinimumSize(QSize(40, 0));
        label_22->setMaximumSize(QSize(40, 16777215));
        label_22->setFrameShape(QFrame::NoFrame);

        horizontalLayout_6->addWidget(label_22);


        gridLayout_6->addLayout(horizontalLayout_6, 0, 0, 1, 1);

        horizontalLayout_15 = new QHBoxLayout();
        horizontalLayout_15->setSpacing(20);
        horizontalLayout_15->setObjectName(QStringLiteral("horizontalLayout_15"));
        horizontalLayout_15->setContentsMargins(250, -1, 50, -1);
        label_14 = new QLabel(widget);
        label_14->setObjectName(QStringLiteral("label_14"));
        label_14->setMinimumSize(QSize(150, 0));
        label_14->setMaximumSize(QSize(150, 16777215));
        label_14->setStyleSheet(QStringLiteral(""));
        label_14->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        horizontalLayout_15->addWidget(label_14);

        D2_Edit = new CustomLineEdit(widget);
        D2_Edit->setObjectName(QStringLiteral("D2_Edit"));
        D2_Edit->setMinimumSize(QSize(0, 30));
        D2_Edit->setMaximumSize(QSize(130, 16777215));

        horizontalLayout_15->addWidget(D2_Edit);

        label_23 = new QLabel(widget);
        label_23->setObjectName(QStringLiteral("label_23"));
        label_23->setMinimumSize(QSize(40, 0));
        label_23->setMaximumSize(QSize(40, 16777215));

        horizontalLayout_15->addWidget(label_23);


        gridLayout_6->addLayout(horizontalLayout_15, 1, 0, 1, 1);

        horizontalLayout_5 = new QHBoxLayout();
        horizontalLayout_5->setSpacing(15);
        horizontalLayout_5->setObjectName(QStringLiteral("horizontalLayout_5"));
        horizontalLayout_5->setContentsMargins(100, -1, 300, -1);
        label_35 = new QLabel(widget);
        label_35->setObjectName(QStringLiteral("label_35"));
        label_35->setMinimumSize(QSize(170, 0));
        label_35->setMaximumSize(QSize(170, 16777215));
        label_35->setLayoutDirection(Qt::LeftToRight);
        label_35->setInputMethodHints(Qt::ImhLatinOnly);
        label_35->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        horizontalLayout_5->addWidget(label_35);

        ElevitionR_Edit = new CustomLineEdit(widget);
        ElevitionR_Edit->setObjectName(QStringLiteral("ElevitionR_Edit"));
        ElevitionR_Edit->setMinimumSize(QSize(0, 30));
        ElevitionR_Edit->setMaximumSize(QSize(130, 16777215));

        horizontalLayout_5->addWidget(ElevitionR_Edit);

        label_48 = new QLabel(widget);
        label_48->setObjectName(QStringLiteral("label_48"));
        label_48->setMinimumSize(QSize(80, 0));
        label_48->setMaximumSize(QSize(80, 16777215));

        horizontalLayout_5->addWidget(label_48);


        gridLayout_6->addLayout(horizontalLayout_5, 4, 1, 1, 1);

        horizontalLayout_22 = new QHBoxLayout();
        horizontalLayout_22->setSpacing(15);
        horizontalLayout_22->setObjectName(QStringLiteral("horizontalLayout_22"));
        horizontalLayout_22->setContentsMargins(100, -1, 300, -1);
        label_18 = new QLabel(widget);
        label_18->setObjectName(QStringLiteral("label_18"));
        label_18->setMinimumSize(QSize(170, 0));
        label_18->setMaximumSize(QSize(170, 16777215));
        label_18->setInputMethodHints(Qt::ImhLatinOnly);
        label_18->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        horizontalLayout_22->addWidget(label_18);

        CLine_Edit = new CustomLineEdit(widget);
        CLine_Edit->setObjectName(QStringLiteral("CLine_Edit"));
        CLine_Edit->setMinimumSize(QSize(0, 30));
        CLine_Edit->setMaximumSize(QSize(130, 16777215));

        horizontalLayout_22->addWidget(CLine_Edit);

        label_27 = new QLabel(widget);
        label_27->setObjectName(QStringLiteral("label_27"));
        label_27->setMinimumSize(QSize(80, 0));
        label_27->setMaximumSize(QSize(80, 16777215));

        horizontalLayout_22->addWidget(label_27);


        gridLayout_6->addLayout(horizontalLayout_22, 1, 1, 1, 1);

        horizontalLayout_4 = new QHBoxLayout();
        horizontalLayout_4->setSpacing(15);
        horizontalLayout_4->setObjectName(QStringLiteral("horizontalLayout_4"));
        horizontalLayout_4->setContentsMargins(100, -1, 300, -1);
        label_34 = new QLabel(widget);
        label_34->setObjectName(QStringLiteral("label_34"));
        label_34->setMinimumSize(QSize(170, 0));
        label_34->setMaximumSize(QSize(170, 16777215));
        label_34->setLayoutDirection(Qt::LeftToRight);
        label_34->setInputMethodHints(Qt::ImhLatinOnly);
        label_34->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        horizontalLayout_4->addWidget(label_34);

        CLineS_Edit = new CustomLineEdit(widget);
        CLineS_Edit->setObjectName(QStringLiteral("CLineS_Edit"));
        CLineS_Edit->setMinimumSize(QSize(0, 30));
        CLineS_Edit->setMaximumSize(QSize(130, 16777215));

        horizontalLayout_4->addWidget(CLineS_Edit);

        label_43 = new QLabel(widget);
        label_43->setObjectName(QStringLiteral("label_43"));
        label_43->setMinimumSize(QSize(80, 0));
        label_43->setMaximumSize(QSize(80, 16777215));

        horizontalLayout_4->addWidget(label_43);


        gridLayout_6->addLayout(horizontalLayout_4, 3, 1, 1, 1);

        horizontalLayout_21 = new QHBoxLayout();
        horizontalLayout_21->setSpacing(15);
        horizontalLayout_21->setObjectName(QStringLiteral("horizontalLayout_21"));
        horizontalLayout_21->setContentsMargins(100, -1, 300, -1);
        label_17 = new QLabel(widget);
        label_17->setObjectName(QStringLiteral("label_17"));
        label_17->setMinimumSize(QSize(170, 0));
        label_17->setMaximumSize(QSize(170, 16777215));
        label_17->setInputMethodHints(Qt::ImhLatinOnly);
        label_17->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        horizontalLayout_21->addWidget(label_17);

        ElevationTravel_Edit = new CustomLineEdit(widget);
        ElevationTravel_Edit->setObjectName(QStringLiteral("ElevationTravel_Edit"));
        ElevationTravel_Edit->setMinimumSize(QSize(0, 30));
        ElevationTravel_Edit->setMaximumSize(QSize(130, 16777215));

        horizontalLayout_21->addWidget(ElevationTravel_Edit);

        label_25 = new QLabel(widget);
        label_25->setObjectName(QStringLiteral("label_25"));
        label_25->setMinimumSize(QSize(80, 0));
        label_25->setMaximumSize(QSize(80, 16777215));

        horizontalLayout_21->addWidget(label_25);


        gridLayout_6->addLayout(horizontalLayout_21, 0, 1, 1, 1);

        horizontalLayout_16 = new QHBoxLayout();
        horizontalLayout_16->setSpacing(20);
        horizontalLayout_16->setObjectName(QStringLiteral("horizontalLayout_16"));
        horizontalLayout_16->setContentsMargins(250, -1, 50, -1);
        label_15 = new QLabel(widget);
        label_15->setObjectName(QStringLiteral("label_15"));
        label_15->setMinimumSize(QSize(150, 0));
        label_15->setMaximumSize(QSize(150, 16777215));
        label_15->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        horizontalLayout_16->addWidget(label_15);

        D_Edit = new CustomLineEdit(widget);
        D_Edit->setObjectName(QStringLiteral("D_Edit"));
        D_Edit->setMinimumSize(QSize(0, 30));
        D_Edit->setMaximumSize(QSize(130, 16777215));

        horizontalLayout_16->addWidget(D_Edit);

        label_24 = new QLabel(widget);
        label_24->setObjectName(QStringLiteral("label_24"));
        label_24->setMinimumSize(QSize(40, 0));
        label_24->setMaximumSize(QSize(40, 16777215));

        horizontalLayout_16->addWidget(label_24);


        gridLayout_6->addLayout(horizontalLayout_16, 2, 0, 1, 1);

        horizontalLayout_20 = new QHBoxLayout();
        horizontalLayout_20->setSpacing(20);
        horizontalLayout_20->setObjectName(QStringLiteral("horizontalLayout_20"));
        horizontalLayout_20->setContentsMargins(250, -1, 50, -1);
        label_16 = new QLabel(widget);
        label_16->setObjectName(QStringLiteral("label_16"));
        label_16->setMinimumSize(QSize(150, 0));
        label_16->setMaximumSize(QSize(150, 16777215));
        label_16->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        horizontalLayout_20->addWidget(label_16);

        RailNum_Edit = new CustomLineEdit(widget);
        RailNum_Edit->setObjectName(QStringLiteral("RailNum_Edit"));
        RailNum_Edit->setMinimumSize(QSize(0, 30));
        RailNum_Edit->setMaximumSize(QSize(130, 16777215));

        horizontalLayout_20->addWidget(RailNum_Edit);

        label_26 = new QLabel(widget);
        label_26->setObjectName(QStringLiteral("label_26"));
        label_26->setMinimumSize(QSize(40, 0));
        label_26->setMaximumSize(QSize(40, 16777215));

        horizontalLayout_20->addWidget(label_26);


        gridLayout_6->addLayout(horizontalLayout_20, 3, 0, 1, 1);

        horizontalLayout_3 = new QHBoxLayout();
        horizontalLayout_3->setSpacing(15);
        horizontalLayout_3->setObjectName(QStringLiteral("horizontalLayout_3"));
        horizontalLayout_3->setContentsMargins(100, -1, 300, -1);
        label_33 = new QLabel(widget);
        label_33->setObjectName(QStringLiteral("label_33"));
        label_33->setMinimumSize(QSize(170, 0));
        label_33->setMaximumSize(QSize(170, 16777215));
        label_33->setLayoutDirection(Qt::LeftToRight);
        label_33->setStyleSheet(QStringLiteral(""));
        label_33->setInputMethodHints(Qt::ImhLatinOnly);
        label_33->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        horizontalLayout_3->addWidget(label_33);

        ElevitionL_Edit = new CustomLineEdit(widget);
        ElevitionL_Edit->setObjectName(QStringLiteral("ElevitionL_Edit"));
        ElevitionL_Edit->setMinimumSize(QSize(0, 30));
        ElevitionL_Edit->setMaximumSize(QSize(130, 16777215));

        horizontalLayout_3->addWidget(ElevitionL_Edit);

        label_47 = new QLabel(widget);
        label_47->setObjectName(QStringLiteral("label_47"));
        label_47->setMinimumSize(QSize(80, 0));
        label_47->setMaximumSize(QSize(80, 16777215));

        horizontalLayout_3->addWidget(label_47);


        gridLayout_6->addLayout(horizontalLayout_3, 2, 1, 1, 1);

        horizontalLayout_24 = new QHBoxLayout();
        horizontalLayout_24->setSpacing(20);
        horizontalLayout_24->setObjectName(QStringLiteral("horizontalLayout_24"));
        horizontalLayout_24->setContentsMargins(250, -1, 0, -1);

        gridLayout_6->addLayout(horizontalLayout_24, 4, 0, 1, 1);


        verticalLayout->addLayout(gridLayout_6);

        horizontalLayout_25 = new QHBoxLayout();
        horizontalLayout_25->setSpacing(20);
        horizontalLayout_25->setObjectName(QStringLiteral("horizontalLayout_25"));
        horizontalLayout_25->setContentsMargins(-1, -1, 0, -1);
        btnSave = new QPushButton(widget);
        btnSave->setObjectName(QStringLiteral("btnSave"));
        btnSave->setMinimumSize(QSize(100, 50));
        btnSave->setMaximumSize(QSize(160, 16777215));

        horizontalLayout_25->addWidget(btnSave);


        verticalLayout->addLayout(horizontalLayout_25);


        verticalLayout_2->addWidget(widget);


        horizontalLayout_19->addLayout(verticalLayout_2);

        verticalLayout_4 = new QVBoxLayout();
        verticalLayout_4->setObjectName(QStringLiteral("verticalLayout_4"));

        horizontalLayout_19->addLayout(verticalLayout_4);


        retranslateUi(ConfigForm);

        QMetaObject::connectSlotsByName(ConfigForm);
    } // setupUi

    void retranslateUi(QWidget *ConfigForm)
    {
        ConfigForm->setWindowTitle(QApplication::translate("ConfigForm", "Form", Q_NULLPTR));
        picLabel->setText(QString());
        label_13->setText(QApplication::translate("ConfigForm", "D1", Q_NULLPTR));
        D1_Edit->setText(QApplication::translate("ConfigForm", "1", Q_NULLPTR));
        label_22->setText(QApplication::translate("ConfigForm", "mm", Q_NULLPTR));
        label_14->setText(QApplication::translate("ConfigForm", "D2", Q_NULLPTR));
        D2_Edit->setText(QApplication::translate("ConfigForm", "1", Q_NULLPTR));
        label_23->setText(QApplication::translate("ConfigForm", "mm", Q_NULLPTR));
        label_35->setText(QApplication::translate("ConfigForm", "\351\253\230\347\250\213R\346\234\200\345\244\247\345\201\217\345\267\256", Q_NULLPTR));
        ElevitionR_Edit->setText(QApplication::translate("ConfigForm", "1", Q_NULLPTR));
        label_48->setText(QApplication::translate("ConfigForm", "mm", Q_NULLPTR));
        label_18->setText(QApplication::translate("ConfigForm", "\344\270\255\347\272\277\350\241\214\347\250\213", Q_NULLPTR));
        CLine_Edit->setText(QApplication::translate("ConfigForm", "1", Q_NULLPTR));
        label_27->setText(QApplication::translate("ConfigForm", "mm", Q_NULLPTR));
        label_34->setText(QApplication::translate("ConfigForm", "\344\270\255\347\272\277S\346\234\200\345\244\247\345\201\217\345\267\256", Q_NULLPTR));
        CLineS_Edit->setText(QApplication::translate("ConfigForm", "1", Q_NULLPTR));
        label_43->setText(QApplication::translate("ConfigForm", "mm", Q_NULLPTR));
        label_17->setText(QApplication::translate("ConfigForm", "\351\253\230\347\250\213\350\241\214\347\250\213", Q_NULLPTR));
        ElevationTravel_Edit->setText(QApplication::translate("ConfigForm", "1", Q_NULLPTR));
        label_25->setText(QApplication::translate("ConfigForm", "mm", Q_NULLPTR));
        label_15->setText(QApplication::translate("ConfigForm", "D", Q_NULLPTR));
        D_Edit->setText(QApplication::translate("ConfigForm", "1", Q_NULLPTR));
        label_24->setText(QApplication::translate("ConfigForm", "mm", Q_NULLPTR));
        label_16->setText(QApplication::translate("ConfigForm", "\350\275\250\346\216\222\346\225\260\351\207\217", Q_NULLPTR));
        RailNum_Edit->setText(QApplication::translate("ConfigForm", "1", Q_NULLPTR));
        label_26->setText(QString());
        label_33->setText(QApplication::translate("ConfigForm", "\351\253\230\347\250\213L\346\234\200\345\244\247\345\201\217\345\267\256", Q_NULLPTR));
        ElevitionL_Edit->setText(QApplication::translate("ConfigForm", "1", Q_NULLPTR));
        label_47->setText(QApplication::translate("ConfigForm", "mm", Q_NULLPTR));
        btnSave->setText(QApplication::translate("ConfigForm", "\344\277\235\345\255\230\346\225\260\346\215\256", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class ConfigForm: public Ui_ConfigForm {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_CONFIGFORM_H

/********************************************************************************
** Form generated from reading UI file 'workform.ui'
**
** Created by: Qt User Interface Compiler version 5.7.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_WORKFORM_H
#define UI_WORKFORM_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>
#include <customlineedit.h>
#include <mytable.h>

QT_BEGIN_NAMESPACE

class Ui_WorkForm
{
public:
    QGridLayout *gridLayout_4;
    QHBoxLayout *horizontalLayout_6;
    QLabel *titleLabel;
    QLabel *timeLab;
    QHBoxLayout *horizontalLayout;
    QVBoxLayout *verticalLayout;
    Mytable *CurStatusWgt;
    QPushButton *pushButton_LastData;
    Mytable *LastStatusWgt;
    QVBoxLayout *verticalLayout_11;
    QWidget *widget;
    QVBoxLayout *verticalLayout_15;
    QGroupBox *groupBox;
    QVBoxLayout *verticalLayout_12;
    QHBoxLayout *horizontalLayout_5;
    QVBoxLayout *verticalLayout_13;
    QVBoxLayout *verticalLayout_3;
    QLabel *label_6;
    CustomLineEdit *lineEdit_LC;
    QVBoxLayout *verticalLayout_4;
    QLabel *label_8;
    CustomLineEdit *lineEdit_ZX;
    QVBoxLayout *verticalLayout_14;
    QVBoxLayout *verticalLayout_9;
    QLabel *label_7;
    CustomLineEdit *lineEdit_ID;
    QVBoxLayout *verticalLayout_8;
    QLabel *label_9;
    CustomLineEdit *lineEdit_GJ;
    QHBoxLayout *horizontalLayout_2;
    QVBoxLayout *verticalLayout_5;
    QLabel *label_10;
    CustomLineEdit *lineEdit_ZG;
    QVBoxLayout *verticalLayout_6;
    QLabel *label_11;
    CustomLineEdit *lineEdit_CG;
    QVBoxLayout *verticalLayout_7;
    QLabel *label_12;
    CustomLineEdit *lineEdit_YG;
    QGridLayout *gridLayout_2;
    QPushButton *pushButton_CJ_add;
    QPushButton *pushButton_CJ_del;
    QPushButton *pushButton_CJ_modify;
    QPushButton *pushButton_CJ_clean;
    QGroupBox *groupBox_2;
    QGridLayout *gridLayout;
    QPushButton *btnSingle;
    QPushButton *btnAuto;
    QComboBox *comboBox;
    QPushButton *pushButton_check;
    QPushButton *pushButton_ctrl_deal;
    QComboBox *comboBox_measureDirection;
    QGroupBox *groupBox_3;
    QHBoxLayout *horizontalLayout_4;
    QPushButton *pushButton_saveData;
    QPushButton *pushButton_readData;
    QWidget *widget_4;
    QVBoxLayout *verticalLayout_10;
    QGroupBox *groupBox_4;
    QGridLayout *gridLayout_3;
    QTextEdit *textEdit_info;
    QHBoxLayout *horizontalLayout_3;
    QLabel *label_QYC;
    QLabel *label_GP;
    QLabel *label_JT;

    void setupUi(QWidget *WorkForm)
    {
        if (WorkForm->objectName().isEmpty())
            WorkForm->setObjectName(QStringLiteral("WorkForm"));
        WorkForm->resize(1020, 824);
        gridLayout_4 = new QGridLayout(WorkForm);
        gridLayout_4->setObjectName(QStringLiteral("gridLayout_4"));
        gridLayout_4->setHorizontalSpacing(6);
        gridLayout_4->setVerticalSpacing(3);
        gridLayout_4->setContentsMargins(-1, 6, -1, -1);
        horizontalLayout_6 = new QHBoxLayout();
        horizontalLayout_6->setSpacing(0);
        horizontalLayout_6->setObjectName(QStringLiteral("horizontalLayout_6"));
        titleLabel = new QLabel(WorkForm);
        titleLabel->setObjectName(QStringLiteral("titleLabel"));
        titleLabel->setMinimumSize(QSize(0, 60));
        titleLabel->setMaximumSize(QSize(16777215, 60));
        titleLabel->setStyleSheet(QStringLiteral(""));
        titleLabel->setAlignment(Qt::AlignCenter);

        horizontalLayout_6->addWidget(titleLabel);

        timeLab = new QLabel(WorkForm);
        timeLab->setObjectName(QStringLiteral("timeLab"));
        timeLab->setMinimumSize(QSize(200, 0));
        timeLab->setStyleSheet(QStringLiteral(""));
        timeLab->setAlignment(Qt::AlignCenter);

        horizontalLayout_6->addWidget(timeLab);

        horizontalLayout_6->setStretch(0, 8);
        horizontalLayout_6->setStretch(1, 2);

        gridLayout_4->addLayout(horizontalLayout_6, 0, 0, 1, 1);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setSpacing(3);
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        verticalLayout = new QVBoxLayout();
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        CurStatusWgt = new Mytable(WorkForm);
        CurStatusWgt->setObjectName(QStringLiteral("CurStatusWgt"));
        CurStatusWgt->setMinimumSize(QSize(500, 0));

        verticalLayout->addWidget(CurStatusWgt);

        pushButton_LastData = new QPushButton(WorkForm);
        pushButton_LastData->setObjectName(QStringLiteral("pushButton_LastData"));
        pushButton_LastData->setCheckable(true);

        verticalLayout->addWidget(pushButton_LastData);

        LastStatusWgt = new Mytable(WorkForm);
        LastStatusWgt->setObjectName(QStringLiteral("LastStatusWgt"));

        verticalLayout->addWidget(LastStatusWgt);


        horizontalLayout->addLayout(verticalLayout);

        verticalLayout_11 = new QVBoxLayout();
        verticalLayout_11->setSpacing(10);
        verticalLayout_11->setObjectName(QStringLiteral("verticalLayout_11"));
        widget = new QWidget(WorkForm);
        widget->setObjectName(QStringLiteral("widget"));
        widget->setStyleSheet(QStringLiteral(""));
        verticalLayout_15 = new QVBoxLayout(widget);
        verticalLayout_15->setSpacing(5);
        verticalLayout_15->setObjectName(QStringLiteral("verticalLayout_15"));
        verticalLayout_15->setContentsMargins(0, 0, 0, 0);
        groupBox = new QGroupBox(widget);
        groupBox->setObjectName(QStringLiteral("groupBox"));
        groupBox->setMinimumSize(QSize(0, 50));
        verticalLayout_12 = new QVBoxLayout(groupBox);
        verticalLayout_12->setSpacing(10);
        verticalLayout_12->setObjectName(QStringLiteral("verticalLayout_12"));
        verticalLayout_12->setContentsMargins(6, 6, 6, 6);
        horizontalLayout_5 = new QHBoxLayout();
        horizontalLayout_5->setObjectName(QStringLiteral("horizontalLayout_5"));
        horizontalLayout_5->setSizeConstraint(QLayout::SetDefaultConstraint);
        verticalLayout_13 = new QVBoxLayout();
        verticalLayout_13->setObjectName(QStringLiteral("verticalLayout_13"));
        verticalLayout_3 = new QVBoxLayout();
        verticalLayout_3->setSpacing(0);
        verticalLayout_3->setObjectName(QStringLiteral("verticalLayout_3"));
        label_6 = new QLabel(groupBox);
        label_6->setObjectName(QStringLiteral("label_6"));
        label_6->setMinimumSize(QSize(0, 25));
        label_6->setFrameShape(QFrame::StyledPanel);
        label_6->setAlignment(Qt::AlignCenter);

        verticalLayout_3->addWidget(label_6);

        lineEdit_LC = new CustomLineEdit(groupBox);
        lineEdit_LC->setObjectName(QStringLiteral("lineEdit_LC"));
        lineEdit_LC->setMinimumSize(QSize(200, 35));
        lineEdit_LC->setAlignment(Qt::AlignCenter);

        verticalLayout_3->addWidget(lineEdit_LC);


        verticalLayout_13->addLayout(verticalLayout_3);

        verticalLayout_4 = new QVBoxLayout();
        verticalLayout_4->setSpacing(0);
        verticalLayout_4->setObjectName(QStringLiteral("verticalLayout_4"));
        label_8 = new QLabel(groupBox);
        label_8->setObjectName(QStringLiteral("label_8"));
        label_8->setMinimumSize(QSize(0, 25));
        label_8->setFrameShape(QFrame::StyledPanel);
        label_8->setAlignment(Qt::AlignCenter);

        verticalLayout_4->addWidget(label_8);

        lineEdit_ZX = new CustomLineEdit(groupBox);
        lineEdit_ZX->setObjectName(QStringLiteral("lineEdit_ZX"));
        lineEdit_ZX->setMinimumSize(QSize(0, 35));
        lineEdit_ZX->setAlignment(Qt::AlignCenter);

        verticalLayout_4->addWidget(lineEdit_ZX);


        verticalLayout_13->addLayout(verticalLayout_4);


        horizontalLayout_5->addLayout(verticalLayout_13);

        verticalLayout_14 = new QVBoxLayout();
        verticalLayout_14->setObjectName(QStringLiteral("verticalLayout_14"));
        verticalLayout_9 = new QVBoxLayout();
        verticalLayout_9->setSpacing(0);
        verticalLayout_9->setObjectName(QStringLiteral("verticalLayout_9"));
        label_7 = new QLabel(groupBox);
        label_7->setObjectName(QStringLiteral("label_7"));
        label_7->setFrameShape(QFrame::StyledPanel);
        label_7->setAlignment(Qt::AlignCenter);

        verticalLayout_9->addWidget(label_7);

        lineEdit_ID = new CustomLineEdit(groupBox);
        lineEdit_ID->setObjectName(QStringLiteral("lineEdit_ID"));
        lineEdit_ID->setMinimumSize(QSize(0, 35));
        lineEdit_ID->setAlignment(Qt::AlignCenter);

        verticalLayout_9->addWidget(lineEdit_ID);


        verticalLayout_14->addLayout(verticalLayout_9);

        verticalLayout_8 = new QVBoxLayout();
        verticalLayout_8->setSpacing(0);
        verticalLayout_8->setObjectName(QStringLiteral("verticalLayout_8"));
        label_9 = new QLabel(groupBox);
        label_9->setObjectName(QStringLiteral("label_9"));
        label_9->setFrameShape(QFrame::StyledPanel);
        label_9->setAlignment(Qt::AlignCenter);

        verticalLayout_8->addWidget(label_9);

        lineEdit_GJ = new CustomLineEdit(groupBox);
        lineEdit_GJ->setObjectName(QStringLiteral("lineEdit_GJ"));
        lineEdit_GJ->setMinimumSize(QSize(0, 35));
        lineEdit_GJ->setAlignment(Qt::AlignCenter);

        verticalLayout_8->addWidget(lineEdit_GJ);


        verticalLayout_14->addLayout(verticalLayout_8);


        horizontalLayout_5->addLayout(verticalLayout_14);

        horizontalLayout_5->setStretch(0, 5);
        horizontalLayout_5->setStretch(1, 5);

        verticalLayout_12->addLayout(horizontalLayout_5);

        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setObjectName(QStringLiteral("horizontalLayout_2"));
        horizontalLayout_2->setContentsMargins(-1, 10, -1, -1);
        verticalLayout_5 = new QVBoxLayout();
        verticalLayout_5->setSpacing(0);
        verticalLayout_5->setObjectName(QStringLiteral("verticalLayout_5"));
        label_10 = new QLabel(groupBox);
        label_10->setObjectName(QStringLiteral("label_10"));
        label_10->setMinimumSize(QSize(0, 25));
        label_10->setFrameShape(QFrame::StyledPanel);
        label_10->setAlignment(Qt::AlignCenter);

        verticalLayout_5->addWidget(label_10);

        lineEdit_ZG = new CustomLineEdit(groupBox);
        lineEdit_ZG->setObjectName(QStringLiteral("lineEdit_ZG"));
        lineEdit_ZG->setMinimumSize(QSize(0, 35));
        lineEdit_ZG->setAlignment(Qt::AlignCenter);

        verticalLayout_5->addWidget(lineEdit_ZG);


        horizontalLayout_2->addLayout(verticalLayout_5);

        verticalLayout_6 = new QVBoxLayout();
        verticalLayout_6->setSpacing(0);
        verticalLayout_6->setObjectName(QStringLiteral("verticalLayout_6"));
        label_11 = new QLabel(groupBox);
        label_11->setObjectName(QStringLiteral("label_11"));
        label_11->setFrameShape(QFrame::StyledPanel);
        label_11->setAlignment(Qt::AlignCenter);

        verticalLayout_6->addWidget(label_11);

        lineEdit_CG = new CustomLineEdit(groupBox);
        lineEdit_CG->setObjectName(QStringLiteral("lineEdit_CG"));
        lineEdit_CG->setMinimumSize(QSize(0, 35));
        lineEdit_CG->setAlignment(Qt::AlignCenter);

        verticalLayout_6->addWidget(lineEdit_CG);


        horizontalLayout_2->addLayout(verticalLayout_6);

        verticalLayout_7 = new QVBoxLayout();
        verticalLayout_7->setSpacing(0);
        verticalLayout_7->setObjectName(QStringLiteral("verticalLayout_7"));
        label_12 = new QLabel(groupBox);
        label_12->setObjectName(QStringLiteral("label_12"));
        label_12->setFrameShape(QFrame::StyledPanel);
        label_12->setAlignment(Qt::AlignCenter);

        verticalLayout_7->addWidget(label_12);

        lineEdit_YG = new CustomLineEdit(groupBox);
        lineEdit_YG->setObjectName(QStringLiteral("lineEdit_YG"));
        lineEdit_YG->setMinimumSize(QSize(0, 35));
        lineEdit_YG->setAlignment(Qt::AlignCenter);

        verticalLayout_7->addWidget(lineEdit_YG);


        horizontalLayout_2->addLayout(verticalLayout_7);


        verticalLayout_12->addLayout(horizontalLayout_2);

        gridLayout_2 = new QGridLayout();
        gridLayout_2->setSpacing(20);
        gridLayout_2->setObjectName(QStringLiteral("gridLayout_2"));
        gridLayout_2->setContentsMargins(0, 0, 0, 5);
        pushButton_CJ_add = new QPushButton(groupBox);
        pushButton_CJ_add->setObjectName(QStringLiteral("pushButton_CJ_add"));
        pushButton_CJ_add->setMinimumSize(QSize(0, 30));

        gridLayout_2->addWidget(pushButton_CJ_add, 0, 0, 1, 1);

        pushButton_CJ_del = new QPushButton(groupBox);
        pushButton_CJ_del->setObjectName(QStringLiteral("pushButton_CJ_del"));
        pushButton_CJ_del->setMinimumSize(QSize(0, 30));

        gridLayout_2->addWidget(pushButton_CJ_del, 0, 1, 1, 1);

        pushButton_CJ_modify = new QPushButton(groupBox);
        pushButton_CJ_modify->setObjectName(QStringLiteral("pushButton_CJ_modify"));
        pushButton_CJ_modify->setMinimumSize(QSize(0, 30));

        gridLayout_2->addWidget(pushButton_CJ_modify, 1, 0, 1, 1);

        pushButton_CJ_clean = new QPushButton(groupBox);
        pushButton_CJ_clean->setObjectName(QStringLiteral("pushButton_CJ_clean"));
        pushButton_CJ_clean->setMinimumSize(QSize(0, 30));

        gridLayout_2->addWidget(pushButton_CJ_clean, 1, 1, 1, 1);


        verticalLayout_12->addLayout(gridLayout_2);


        verticalLayout_15->addWidget(groupBox);


        verticalLayout_11->addWidget(widget);

        groupBox_2 = new QGroupBox(WorkForm);
        groupBox_2->setObjectName(QStringLiteral("groupBox_2"));
        gridLayout = new QGridLayout(groupBox_2);
        gridLayout->setObjectName(QStringLiteral("gridLayout"));
        gridLayout->setVerticalSpacing(5);
        gridLayout->setContentsMargins(6, 0, 6, 0);
        btnSingle = new QPushButton(groupBox_2);
        btnSingle->setObjectName(QStringLiteral("btnSingle"));
        btnSingle->setMinimumSize(QSize(0, 30));
        btnSingle->setCheckable(true);

        gridLayout->addWidget(btnSingle, 0, 0, 1, 1);

        btnAuto = new QPushButton(groupBox_2);
        btnAuto->setObjectName(QStringLiteral("btnAuto"));
        btnAuto->setMinimumSize(QSize(0, 30));
        btnAuto->setCheckable(true);

        gridLayout->addWidget(btnAuto, 0, 1, 1, 1);

        comboBox = new QComboBox(groupBox_2);
        comboBox->setObjectName(QStringLiteral("comboBox"));
        comboBox->setMinimumSize(QSize(0, 30));
        comboBox->setLayoutDirection(Qt::LeftToRight);
        comboBox->setStyleSheet(QStringLiteral(""));

        gridLayout->addWidget(comboBox, 1, 1, 1, 1);

        pushButton_check = new QPushButton(groupBox_2);
        pushButton_check->setObjectName(QStringLiteral("pushButton_check"));
        pushButton_check->setMinimumSize(QSize(0, 30));
        pushButton_check->setCheckable(false);

        gridLayout->addWidget(pushButton_check, 2, 0, 1, 1);

        pushButton_ctrl_deal = new QPushButton(groupBox_2);
        pushButton_ctrl_deal->setObjectName(QStringLiteral("pushButton_ctrl_deal"));
        pushButton_ctrl_deal->setMinimumSize(QSize(0, 30));
        pushButton_ctrl_deal->setCheckable(true);

        gridLayout->addWidget(pushButton_ctrl_deal, 2, 1, 1, 1);

        comboBox_measureDirection = new QComboBox(groupBox_2);
        comboBox_measureDirection->setObjectName(QStringLiteral("comboBox_measureDirection"));
        comboBox_measureDirection->setMinimumSize(QSize(0, 30));
        comboBox_measureDirection->setLayoutDirection(Qt::LeftToRight);
        comboBox_measureDirection->setStyleSheet(QStringLiteral(""));

        gridLayout->addWidget(comboBox_measureDirection, 1, 0, 1, 1);


        verticalLayout_11->addWidget(groupBox_2);

        groupBox_3 = new QGroupBox(WorkForm);
        groupBox_3->setObjectName(QStringLiteral("groupBox_3"));
        horizontalLayout_4 = new QHBoxLayout(groupBox_3);
        horizontalLayout_4->setObjectName(QStringLiteral("horizontalLayout_4"));
        horizontalLayout_4->setContentsMargins(6, 6, 6, 6);
        pushButton_saveData = new QPushButton(groupBox_3);
        pushButton_saveData->setObjectName(QStringLiteral("pushButton_saveData"));
        pushButton_saveData->setMinimumSize(QSize(0, 27));

        horizontalLayout_4->addWidget(pushButton_saveData);

        pushButton_readData = new QPushButton(groupBox_3);
        pushButton_readData->setObjectName(QStringLiteral("pushButton_readData"));
        pushButton_readData->setMinimumSize(QSize(0, 27));

        horizontalLayout_4->addWidget(pushButton_readData);


        verticalLayout_11->addWidget(groupBox_3);

        widget_4 = new QWidget(WorkForm);
        widget_4->setObjectName(QStringLiteral("widget_4"));
        verticalLayout_10 = new QVBoxLayout(widget_4);
        verticalLayout_10->setObjectName(QStringLiteral("verticalLayout_10"));
        verticalLayout_10->setContentsMargins(0, 0, 0, 0);
        groupBox_4 = new QGroupBox(widget_4);
        groupBox_4->setObjectName(QStringLiteral("groupBox_4"));
        gridLayout_3 = new QGridLayout(groupBox_4);
        gridLayout_3->setObjectName(QStringLiteral("gridLayout_3"));
        gridLayout_3->setContentsMargins(0, -1, 0, -1);
        textEdit_info = new QTextEdit(groupBox_4);
        textEdit_info->setObjectName(QStringLiteral("textEdit_info"));
        textEdit_info->setStyleSheet(QStringLiteral("color: rgb(255, 0, 0);"));

        gridLayout_3->addWidget(textEdit_info, 0, 0, 1, 1);


        verticalLayout_10->addWidget(groupBox_4);

        horizontalLayout_3 = new QHBoxLayout();
        horizontalLayout_3->setSpacing(0);
        horizontalLayout_3->setObjectName(QStringLiteral("horizontalLayout_3"));
        horizontalLayout_3->setContentsMargins(0, 0, -1, 5);
        label_QYC = new QLabel(widget_4);
        label_QYC->setObjectName(QStringLiteral("label_QYC"));
        label_QYC->setStyleSheet(QStringLiteral(""));

        horizontalLayout_3->addWidget(label_QYC);

        label_GP = new QLabel(widget_4);
        label_GP->setObjectName(QStringLiteral("label_GP"));

        horizontalLayout_3->addWidget(label_GP);

        label_JT = new QLabel(widget_4);
        label_JT->setObjectName(QStringLiteral("label_JT"));

        horizontalLayout_3->addWidget(label_JT);


        verticalLayout_10->addLayout(horizontalLayout_3);


        verticalLayout_11->addWidget(widget_4);

        verticalLayout_11->setStretch(0, 5);
        verticalLayout_11->setStretch(1, 3);
        verticalLayout_11->setStretch(2, 1);
        verticalLayout_11->setStretch(3, 1);

        horizontalLayout->addLayout(verticalLayout_11);

        horizontalLayout->setStretch(0, 7);
        horizontalLayout->setStretch(1, 3);

        gridLayout_4->addLayout(horizontalLayout, 1, 0, 1, 1);


        retranslateUi(WorkForm);

        comboBox->setCurrentIndex(1);


        QMetaObject::connectSlotsByName(WorkForm);
    } // setupUi

    void retranslateUi(QWidget *WorkForm)
    {
        WorkForm->setWindowTitle(QApplication::translate("WorkForm", "Form", Q_NULLPTR));
        titleLabel->setText(QApplication::translate("WorkForm", "\351\225\277\345\256\211\346\225\260\347\247\221\346\231\272\351\200\240\346\231\272\350\203\275\350\275\250\346\216\222\347\262\276\350\260\203\347\263\273\347\273\237", Q_NULLPTR));
        timeLab->setText(QString());
        pushButton_LastData->setText(QApplication::translate("WorkForm", "\345\261\225\347\244\272\344\270\212\344\270\200\346\254\241\346\225\260\346\215\256>>>", Q_NULLPTR));
        groupBox->setTitle(QApplication::translate("WorkForm", "\351\207\207\351\233\206", Q_NULLPTR));
        label_6->setText(QApplication::translate("WorkForm", "\351\207\214\347\250\213", Q_NULLPTR));
        lineEdit_LC->setText(QApplication::translate("WorkForm", "0", Q_NULLPTR));
        label_8->setText(QApplication::translate("WorkForm", "\344\270\255\347\272\277\345\271\263\351\235\242", Q_NULLPTR));
        lineEdit_ZX->setText(QApplication::translate("WorkForm", "0.0", Q_NULLPTR));
        label_7->setText(QApplication::translate("WorkForm", "ID", Q_NULLPTR));
        lineEdit_ID->setText(QApplication::translate("WorkForm", "0", Q_NULLPTR));
        label_9->setText(QApplication::translate("WorkForm", "\350\275\250\350\267\235\345\201\217\345\267\256", Q_NULLPTR));
        lineEdit_GJ->setText(QApplication::translate("WorkForm", "0.0", Q_NULLPTR));
        label_10->setText(QApplication::translate("WorkForm", "\345\267\246\350\275\250\351\253\230\347\250\213", Q_NULLPTR));
        lineEdit_ZG->setText(QApplication::translate("WorkForm", "0.0", Q_NULLPTR));
        label_11->setText(QApplication::translate("WorkForm", "\350\266\205\351\253\230\345\201\217\347\250\213", Q_NULLPTR));
        lineEdit_CG->setText(QApplication::translate("WorkForm", "0.0", Q_NULLPTR));
        label_12->setText(QApplication::translate("WorkForm", "\345\217\263\350\275\250\351\253\230\347\250\213", Q_NULLPTR));
        lineEdit_YG->setText(QApplication::translate("WorkForm", "0.0", Q_NULLPTR));
        pushButton_CJ_add->setText(QApplication::translate("WorkForm", "\346\267\273\345\212\240", Q_NULLPTR));
        pushButton_CJ_del->setText(QApplication::translate("WorkForm", "\345\210\240\351\231\244", Q_NULLPTR));
        pushButton_CJ_modify->setText(QApplication::translate("WorkForm", "\344\277\256\346\224\271", Q_NULLPTR));
        pushButton_CJ_clean->setText(QApplication::translate("WorkForm", "\346\270\205\347\251\272", Q_NULLPTR));
        groupBox_2->setTitle(QApplication::translate("WorkForm", "\346\216\247\345\210\266", Q_NULLPTR));
        btnSingle->setText(QApplication::translate("WorkForm", "\351\200\220\346\236\225\346\265\213\351\207\217", Q_NULLPTR));
        btnAuto->setText(QApplication::translate("WorkForm", "\350\207\252\345\212\250\346\265\213\351\207\217", Q_NULLPTR));
        comboBox->clear();
        comboBox->insertItems(0, QStringList()
         << QApplication::translate("WorkForm", "\345\205\250\351\203\250\350\260\203\346\225\264", Q_NULLPTR)
         << QApplication::translate("WorkForm", "\345\267\246\345\217\263\351\253\230\347\250\213", Q_NULLPTR)
         << QApplication::translate("WorkForm", "\344\270\255\347\272\277", Q_NULLPTR)
         << QApplication::translate("WorkForm", "\345\267\246\351\253\230\347\250\213", Q_NULLPTR)
         << QApplication::translate("WorkForm", "\345\217\263\351\253\230\347\250\213", Q_NULLPTR)
        );
        pushButton_check->setText(QApplication::translate("WorkForm", "\346\243\200\346\237\245", Q_NULLPTR));
        pushButton_ctrl_deal->setText(QApplication::translate("WorkForm", "\347\262\276\350\260\203", Q_NULLPTR));
        comboBox_measureDirection->clear();
        comboBox_measureDirection->insertItems(0, QStringList()
         << QApplication::translate("WorkForm", "\346\255\243\345\220\221\346\265\213\351\207\217", Q_NULLPTR)
         << QApplication::translate("WorkForm", "\345\217\215\345\220\221\346\265\213\351\207\217", Q_NULLPTR)
        );
        groupBox_3->setTitle(QApplication::translate("WorkForm", "\344\277\235\345\255\230", Q_NULLPTR));
        pushButton_saveData->setText(QApplication::translate("WorkForm", "\344\277\235\345\255\230\346\225\260\346\215\256", Q_NULLPTR));
        pushButton_readData->setText(QApplication::translate("WorkForm", "\346\211\223\345\274\200\346\225\260\346\215\256", Q_NULLPTR));
        groupBox_4->setTitle(QApplication::translate("WorkForm", "\346\217\220\347\244\272\344\277\241\346\201\257", Q_NULLPTR));
        textEdit_info->setHtml(QApplication::translate("WorkForm", "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n"
"<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\n"
"p, li { white-space: pre-wrap; }\n"
"</style></head><body style=\" font-family:'SimSun'; font-size:9pt; font-weight:400; font-style:normal;\">\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">qqq</p>\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">qq</p>\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">qq</p>\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">q</p>\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">q</p></body></html>", Q_NULLPTR));
        label_QYC->setText(QApplication::translate("WorkForm", "\347\211\265\345\274\225\350\275\246\351\200\232\350\256\257\345\244\261\350\264\245", Q_NULLPTR));
        label_GP->setText(QApplication::translate("WorkForm", "\350\275\250\346\216\222\351\200\232\350\256\257\345\244\261\350\264\245", Q_NULLPTR));
        label_JT->setText(QApplication::translate("WorkForm", "\347\262\276\350\260\203\347\224\265\346\234\272\346\255\243\345\270\270", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class WorkForm: public Ui_WorkForm {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_WORKFORM_H

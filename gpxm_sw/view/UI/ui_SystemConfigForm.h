/********************************************************************************
** Form generated from reading UI file 'SystemConfigForm.ui'
**
** Created by: Qt User Interface Compiler version 5.7.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_SYSTEMCONFIGFORM_H
#define UI_SYSTEMCONFIGFORM_H

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
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QTextBrowser>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_SystemConfigForm
{
public:
    QGridLayout *gridLayout;
    QTextBrowser *textBrowser;
    QGroupBox *groupBox;
    QGridLayout *gridLayout_2;
    QSpacerItem *verticalSpacer;
    QHBoxLayout *horizontalLayout;
    QSpacerItem *horizontalSpacer;
    QLabel *label;
    QComboBox *comboBox;
    QSpacerItem *horizontalSpacer_2;
    QHBoxLayout *horizontalLayout_2;
    QSpacerItem *horizontalSpacer_3;
    QPushButton *pBtnSetZero;
    QPushButton *pBtnToZero;
    QSpacerItem *horizontalSpacer_4;
    QGroupBox *groupBox_2;
    QHBoxLayout *horizontalLayout_5;
    QSpacerItem *horizontalSpacer_6;
    QVBoxLayout *verticalLayout;
    QHBoxLayout *horizontalLayout_4;
    QLabel *label_3;
    QSpinBox *spinBox;
    QPushButton *pushButton_2;
    QHBoxLayout *horizontalLayout_3;
    QLabel *label_2;
    QSpinBox *spinBox_2;
    QPushButton *pushButton;
    QSpacerItem *horizontalSpacer_5;

    void setupUi(QWidget *SystemConfigForm)
    {
        if (SystemConfigForm->objectName().isEmpty())
            SystemConfigForm->setObjectName(QStringLiteral("SystemConfigForm"));
        SystemConfigForm->resize(880, 511);
        gridLayout = new QGridLayout(SystemConfigForm);
        gridLayout->setObjectName(QStringLiteral("gridLayout"));
        textBrowser = new QTextBrowser(SystemConfigForm);
        textBrowser->setObjectName(QStringLiteral("textBrowser"));

        gridLayout->addWidget(textBrowser, 4, 0, 1, 1);

        groupBox = new QGroupBox(SystemConfigForm);
        groupBox->setObjectName(QStringLiteral("groupBox"));
        gridLayout_2 = new QGridLayout(groupBox);
        gridLayout_2->setObjectName(QStringLiteral("gridLayout_2"));
        verticalSpacer = new QSpacerItem(20, 47, QSizePolicy::Minimum, QSizePolicy::Expanding);

        gridLayout_2->addItem(verticalSpacer, 3, 0, 1, 1);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer);

        label = new QLabel(groupBox);
        label->setObjectName(QStringLiteral("label"));

        horizontalLayout->addWidget(label);

        comboBox = new QComboBox(groupBox);
        comboBox->setObjectName(QStringLiteral("comboBox"));
        comboBox->setMaximumSize(QSize(150, 16777215));
        QFont font;
        font.setFamily(QStringLiteral("AcadEref"));
        font.setPointSize(10);
        comboBox->setFont(font);

        horizontalLayout->addWidget(comboBox);

        horizontalSpacer_2 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer_2);


        gridLayout_2->addLayout(horizontalLayout, 0, 0, 1, 1);

        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setObjectName(QStringLiteral("horizontalLayout_2"));
        horizontalSpacer_3 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_2->addItem(horizontalSpacer_3);

        pBtnSetZero = new QPushButton(groupBox);
        pBtnSetZero->setObjectName(QStringLiteral("pBtnSetZero"));
        pBtnSetZero->setMaximumSize(QSize(230, 16777215));

        horizontalLayout_2->addWidget(pBtnSetZero);

        pBtnToZero = new QPushButton(groupBox);
        pBtnToZero->setObjectName(QStringLiteral("pBtnToZero"));
        pBtnToZero->setMinimumSize(QSize(115, 0));
        pBtnToZero->setMaximumSize(QSize(230, 16777215));

        horizontalLayout_2->addWidget(pBtnToZero);

        horizontalSpacer_4 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_2->addItem(horizontalSpacer_4);

        horizontalLayout_2->setStretch(1, 1);
        horizontalLayout_2->setStretch(2, 1);

        gridLayout_2->addLayout(horizontalLayout_2, 1, 0, 1, 1);


        gridLayout->addWidget(groupBox, 1, 0, 1, 1);

        groupBox_2 = new QGroupBox(SystemConfigForm);
        groupBox_2->setObjectName(QStringLiteral("groupBox_2"));
        horizontalLayout_5 = new QHBoxLayout(groupBox_2);
        horizontalLayout_5->setObjectName(QStringLiteral("horizontalLayout_5"));
        horizontalSpacer_6 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_5->addItem(horizontalSpacer_6);

        verticalLayout = new QVBoxLayout();
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        horizontalLayout_4 = new QHBoxLayout();
        horizontalLayout_4->setObjectName(QStringLiteral("horizontalLayout_4"));
        label_3 = new QLabel(groupBox_2);
        label_3->setObjectName(QStringLiteral("label_3"));
        label_3->setMinimumSize(QSize(100, 0));
        label_3->setMaximumSize(QSize(100, 16777215));
        label_3->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        horizontalLayout_4->addWidget(label_3);

        spinBox = new QSpinBox(groupBox_2);
        spinBox->setObjectName(QStringLiteral("spinBox"));
        spinBox->setMinimumSize(QSize(200, 50));
        spinBox->setMaximumSize(QSize(200, 100));

        horizontalLayout_4->addWidget(spinBox);

        pushButton_2 = new QPushButton(groupBox_2);
        pushButton_2->setObjectName(QStringLiteral("pushButton_2"));

        horizontalLayout_4->addWidget(pushButton_2);


        verticalLayout->addLayout(horizontalLayout_4);

        horizontalLayout_3 = new QHBoxLayout();
        horizontalLayout_3->setObjectName(QStringLiteral("horizontalLayout_3"));
        label_2 = new QLabel(groupBox_2);
        label_2->setObjectName(QStringLiteral("label_2"));
        label_2->setMinimumSize(QSize(100, 0));
        label_2->setMaximumSize(QSize(100, 16777215));
        label_2->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        horizontalLayout_3->addWidget(label_2);

        spinBox_2 = new QSpinBox(groupBox_2);
        spinBox_2->setObjectName(QStringLiteral("spinBox_2"));
        spinBox_2->setMinimumSize(QSize(200, 50));
        spinBox_2->setMaximumSize(QSize(200, 16777215));

        horizontalLayout_3->addWidget(spinBox_2);

        pushButton = new QPushButton(groupBox_2);
        pushButton->setObjectName(QStringLiteral("pushButton"));

        horizontalLayout_3->addWidget(pushButton);


        verticalLayout->addLayout(horizontalLayout_3);


        horizontalLayout_5->addLayout(verticalLayout);

        horizontalSpacer_5 = new QSpacerItem(216, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_5->addItem(horizontalSpacer_5);


        gridLayout->addWidget(groupBox_2, 2, 0, 1, 1);


        retranslateUi(SystemConfigForm);

        QMetaObject::connectSlotsByName(SystemConfigForm);
    } // setupUi

    void retranslateUi(QWidget *SystemConfigForm)
    {
        SystemConfigForm->setWindowTitle(QApplication::translate("SystemConfigForm", "Form", Q_NULLPTR));
        groupBox->setTitle(QApplication::translate("SystemConfigForm", "\346\224\266\350\205\277\350\256\276\347\275\256", Q_NULLPTR));
        label->setText(QApplication::translate("SystemConfigForm", "\350\275\250\346\216\222\351\200\211\346\213\251", Q_NULLPTR));
        comboBox->clear();
        comboBox->insertItems(0, QStringList()
         << QApplication::translate("SystemConfigForm", "1#\350\275\250\346\216\222", Q_NULLPTR)
         << QApplication::translate("SystemConfigForm", "2#\350\275\250\346\216\222", Q_NULLPTR)
         << QApplication::translate("SystemConfigForm", "3#\350\275\250\346\216\222", Q_NULLPTR)
         << QApplication::translate("SystemConfigForm", "4#\350\275\250\346\216\222", Q_NULLPTR)
         << QApplication::translate("SystemConfigForm", "5#\350\275\250\346\216\222", Q_NULLPTR)
         << QApplication::translate("SystemConfigForm", "6#\350\275\250\346\216\222", Q_NULLPTR)
         << QApplication::translate("SystemConfigForm", "7#\350\275\250\346\216\222", Q_NULLPTR)
         << QApplication::translate("SystemConfigForm", "8#\350\275\250\346\216\222", Q_NULLPTR)
         << QApplication::translate("SystemConfigForm", "9#\350\275\250\346\216\222", Q_NULLPTR)
         << QApplication::translate("SystemConfigForm", "10#\350\275\250\346\216\222", Q_NULLPTR)
         << QApplication::translate("SystemConfigForm", "11#\350\275\250\346\216\222", Q_NULLPTR)
         << QApplication::translate("SystemConfigForm", "12#\350\275\250\346\216\222", Q_NULLPTR)
         << QApplication::translate("SystemConfigForm", "13#\350\275\250\346\216\222", Q_NULLPTR)
         << QApplication::translate("SystemConfigForm", "14#\350\275\250\346\216\222", Q_NULLPTR)
         << QApplication::translate("SystemConfigForm", "15#\350\275\250\346\216\222", Q_NULLPTR)
         << QApplication::translate("SystemConfigForm", "16#\350\275\250\346\216\222", Q_NULLPTR)
         << QApplication::translate("SystemConfigForm", "17#\350\275\250\346\216\222", Q_NULLPTR)
         << QApplication::translate("SystemConfigForm", "18#\350\275\250\346\216\222", Q_NULLPTR)
         << QApplication::translate("SystemConfigForm", "19#\350\275\250\346\216\222", Q_NULLPTR)
         << QApplication::translate("SystemConfigForm", "20#\350\275\250\346\216\222", Q_NULLPTR)
         << QApplication::translate("SystemConfigForm", "\346\211\200\346\234\211\350\275\250\346\216\222", Q_NULLPTR)
        );
        pBtnSetZero->setText(QApplication::translate("SystemConfigForm", "\350\256\276\347\275\256\346\224\266\350\205\277\344\275\215\347\275\256", Q_NULLPTR));
        pBtnToZero->setText(QApplication::translate("SystemConfigForm", "\346\224\266\350\205\277", Q_NULLPTR));
        groupBox_2->setTitle(QApplication::translate("SystemConfigForm", "\351\200\237\345\272\246\350\256\276\347\275\256", Q_NULLPTR));
        label_3->setText(QApplication::translate("SystemConfigForm", "\344\275\216/\351\253\230\347\250\213\351\200\237\345\272\246\357\274\232", Q_NULLPTR));
        pushButton_2->setText(QApplication::translate("SystemConfigForm", "\350\256\276\347\275\256", Q_NULLPTR));
        label_2->setText(QApplication::translate("SystemConfigForm", "\344\270\255\347\250\213\351\200\237\345\272\246\357\274\232", Q_NULLPTR));
        pushButton->setText(QApplication::translate("SystemConfigForm", "\350\256\276\347\275\256", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class SystemConfigForm: public Ui_SystemConfigForm {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_SYSTEMCONFIGFORM_H

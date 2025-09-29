/********************************************************************************
** Form generated from reading UI file 'FtpClient.ui'
**
** Created by: Qt User Interface Compiler version 5.7.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_FTPCLIENT_H
#define UI_FTPCLIENT_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QListWidget>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_FtpClient
{
public:
    QVBoxLayout *verticalLayout;
    QGroupBox *groupBox;
    QFormLayout *formLayout;
    QLabel *label;
    QLineEdit *hostEdit;
    QLabel *label_2;
    QSpinBox *portSpin;
    QLabel *label_3;
    QLineEdit *userEdit;
    QLabel *label_4;
    QLineEdit *passEdit;
    QHBoxLayout *horizontalLayout;
    QPushButton *connectButton;
    QPushButton *disconnectButton;
    QSpacerItem *horizontalSpacer;
    QPushButton *ExitButton;
    QGroupBox *groupBox_2;
    QVBoxLayout *verticalLayout_2;
    QListWidget *fileListWidget;
    QHBoxLayout *horizontalLayout_2;
    QLabel *label_5;
    QComboBox *fileTypeCombo;
    QSpacerItem *horizontalSpacer_3;
    QPushButton *goUpButton;
    QSpacerItem *horizontalSpacer_2;
    QGroupBox *groupBox_3;
    QHBoxLayout *horizontalLayout_3;
    QLabel *label_6;
    QLineEdit *downloadDirEdit;
    QPushButton *browseButton;
    QPushButton *downloadButton;
    QLabel *statusLabel;

    void setupUi(QWidget *FtpClient)
    {
        if (FtpClient->objectName().isEmpty())
            FtpClient->setObjectName(QStringLiteral("FtpClient"));
        FtpClient->resize(600, 500);
        verticalLayout = new QVBoxLayout(FtpClient);
        verticalLayout->setSpacing(6);
        verticalLayout->setContentsMargins(11, 11, 11, 11);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        groupBox = new QGroupBox(FtpClient);
        groupBox->setObjectName(QStringLiteral("groupBox"));
        formLayout = new QFormLayout(groupBox);
        formLayout->setSpacing(6);
        formLayout->setContentsMargins(11, 11, 11, 11);
        formLayout->setObjectName(QStringLiteral("formLayout"));
        label = new QLabel(groupBox);
        label->setObjectName(QStringLiteral("label"));

        formLayout->setWidget(0, QFormLayout::LabelRole, label);

        hostEdit = new QLineEdit(groupBox);
        hostEdit->setObjectName(QStringLiteral("hostEdit"));

        formLayout->setWidget(0, QFormLayout::FieldRole, hostEdit);

        label_2 = new QLabel(groupBox);
        label_2->setObjectName(QStringLiteral("label_2"));

        formLayout->setWidget(1, QFormLayout::LabelRole, label_2);

        portSpin = new QSpinBox(groupBox);
        portSpin->setObjectName(QStringLiteral("portSpin"));
        portSpin->setMinimum(1);
        portSpin->setMaximum(65535);
        portSpin->setValue(21);

        formLayout->setWidget(1, QFormLayout::FieldRole, portSpin);

        label_3 = new QLabel(groupBox);
        label_3->setObjectName(QStringLiteral("label_3"));

        formLayout->setWidget(2, QFormLayout::LabelRole, label_3);

        userEdit = new QLineEdit(groupBox);
        userEdit->setObjectName(QStringLiteral("userEdit"));

        formLayout->setWidget(2, QFormLayout::FieldRole, userEdit);

        label_4 = new QLabel(groupBox);
        label_4->setObjectName(QStringLiteral("label_4"));

        formLayout->setWidget(3, QFormLayout::LabelRole, label_4);

        passEdit = new QLineEdit(groupBox);
        passEdit->setObjectName(QStringLiteral("passEdit"));
        passEdit->setEchoMode(QLineEdit::Password);

        formLayout->setWidget(3, QFormLayout::FieldRole, passEdit);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setSpacing(6);
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        connectButton = new QPushButton(groupBox);
        connectButton->setObjectName(QStringLiteral("connectButton"));

        horizontalLayout->addWidget(connectButton);

        disconnectButton = new QPushButton(groupBox);
        disconnectButton->setObjectName(QStringLiteral("disconnectButton"));

        horizontalLayout->addWidget(disconnectButton);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer);

        ExitButton = new QPushButton(groupBox);
        ExitButton->setObjectName(QStringLiteral("ExitButton"));

        horizontalLayout->addWidget(ExitButton);


        formLayout->setLayout(4, QFormLayout::SpanningRole, horizontalLayout);


        verticalLayout->addWidget(groupBox);

        groupBox_2 = new QGroupBox(FtpClient);
        groupBox_2->setObjectName(QStringLiteral("groupBox_2"));
        verticalLayout_2 = new QVBoxLayout(groupBox_2);
        verticalLayout_2->setSpacing(6);
        verticalLayout_2->setContentsMargins(11, 11, 11, 11);
        verticalLayout_2->setObjectName(QStringLiteral("verticalLayout_2"));
        fileListWidget = new QListWidget(groupBox_2);
        fileListWidget->setObjectName(QStringLiteral("fileListWidget"));

        verticalLayout_2->addWidget(fileListWidget);

        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setSpacing(6);
        horizontalLayout_2->setObjectName(QStringLiteral("horizontalLayout_2"));
        label_5 = new QLabel(groupBox_2);
        label_5->setObjectName(QStringLiteral("label_5"));

        horizontalLayout_2->addWidget(label_5);

        fileTypeCombo = new QComboBox(groupBox_2);
        fileTypeCombo->setObjectName(QStringLiteral("fileTypeCombo"));

        horizontalLayout_2->addWidget(fileTypeCombo);

        horizontalSpacer_3 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_2->addItem(horizontalSpacer_3);

        goUpButton = new QPushButton(groupBox_2);
        goUpButton->setObjectName(QStringLiteral("goUpButton"));

        horizontalLayout_2->addWidget(goUpButton);

        horizontalSpacer_2 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_2->addItem(horizontalSpacer_2);


        verticalLayout_2->addLayout(horizontalLayout_2);


        verticalLayout->addWidget(groupBox_2);

        groupBox_3 = new QGroupBox(FtpClient);
        groupBox_3->setObjectName(QStringLiteral("groupBox_3"));
        horizontalLayout_3 = new QHBoxLayout(groupBox_3);
        horizontalLayout_3->setSpacing(6);
        horizontalLayout_3->setContentsMargins(11, 11, 11, 11);
        horizontalLayout_3->setObjectName(QStringLiteral("horizontalLayout_3"));
        label_6 = new QLabel(groupBox_3);
        label_6->setObjectName(QStringLiteral("label_6"));

        horizontalLayout_3->addWidget(label_6);

        downloadDirEdit = new QLineEdit(groupBox_3);
        downloadDirEdit->setObjectName(QStringLiteral("downloadDirEdit"));

        horizontalLayout_3->addWidget(downloadDirEdit);

        browseButton = new QPushButton(groupBox_3);
        browseButton->setObjectName(QStringLiteral("browseButton"));

        horizontalLayout_3->addWidget(browseButton);

        downloadButton = new QPushButton(groupBox_3);
        downloadButton->setObjectName(QStringLiteral("downloadButton"));

        horizontalLayout_3->addWidget(downloadButton);


        verticalLayout->addWidget(groupBox_3);

        statusLabel = new QLabel(FtpClient);
        statusLabel->setObjectName(QStringLiteral("statusLabel"));

        verticalLayout->addWidget(statusLabel);


        retranslateUi(FtpClient);

        QMetaObject::connectSlotsByName(FtpClient);
    } // setupUi

    void retranslateUi(QWidget *FtpClient)
    {
        FtpClient->setWindowTitle(QApplication::translate("FtpClient", "FTP\345\256\242\346\210\267\347\253\257", Q_NULLPTR));
        groupBox->setTitle(QApplication::translate("FtpClient", "\346\234\215\345\212\241\345\231\250\344\277\241\346\201\257", Q_NULLPTR));
        label->setText(QApplication::translate("FtpClient", "\344\270\273\346\234\272\345\234\260\345\235\200\357\274\232", Q_NULLPTR));
        hostEdit->setText(QApplication::translate("FtpClient", "192.168.1.202", Q_NULLPTR));
        label_2->setText(QApplication::translate("FtpClient", "\347\253\257\345\217\243\357\274\232", Q_NULLPTR));
        label_3->setText(QApplication::translate("FtpClient", "\347\224\250\346\210\267\345\220\215\357\274\232", Q_NULLPTR));
        userEdit->setText(QApplication::translate("FtpClient", "123", Q_NULLPTR));
        label_4->setText(QApplication::translate("FtpClient", "\345\257\206\347\240\201\357\274\232", Q_NULLPTR));
        passEdit->setText(QApplication::translate("FtpClient", "123", Q_NULLPTR));
        connectButton->setText(QApplication::translate("FtpClient", "\350\277\236\346\216\245", Q_NULLPTR));
        disconnectButton->setText(QApplication::translate("FtpClient", "\346\226\255\345\274\200\350\277\236\346\216\245", Q_NULLPTR));
        ExitButton->setText(QApplication::translate("FtpClient", "\351\200\200\345\207\272", Q_NULLPTR));
        groupBox_2->setTitle(QApplication::translate("FtpClient", "\346\226\207\344\273\266\345\210\227\350\241\250", Q_NULLPTR));
        label_5->setText(QApplication::translate("FtpClient", "\346\226\207\344\273\266\347\261\273\345\236\213\357\274\232", Q_NULLPTR));
        goUpButton->setText(QApplication::translate("FtpClient", "\350\277\224\345\233\236", Q_NULLPTR));
        groupBox_3->setTitle(QApplication::translate("FtpClient", "\344\270\213\350\275\275\350\256\276\347\275\256", Q_NULLPTR));
        label_6->setText(QApplication::translate("FtpClient", "\344\270\213\350\275\275\347\233\256\345\275\225\357\274\232", Q_NULLPTR));
        browseButton->setText(QApplication::translate("FtpClient", "\346\265\217\350\247\210...", Q_NULLPTR));
        downloadButton->setText(QApplication::translate("FtpClient", "\344\270\213\350\275\275\351\200\211\344\270\255\346\226\207\344\273\266", Q_NULLPTR));
        statusLabel->setText(QApplication::translate("FtpClient", "\346\234\252\350\277\236\346\216\245", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class FtpClient: public Ui_FtpClient {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_FTPCLIENT_H

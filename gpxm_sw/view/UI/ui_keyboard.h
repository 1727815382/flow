/********************************************************************************
** Form generated from reading UI file 'keyboard.ui'
**
** Created by: Qt User Interface Compiler version 5.7.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_KEYBOARD_H
#define UI_KEYBOARD_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QDialog>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>

QT_BEGIN_NAMESPACE

class Ui_Keyboard
{
public:
    QGridLayout *gridLayout;
    QLineEdit *lineEdit;
    QPushButton *pushButton;
    QPushButton *pushButton_2;
    QPushButton *pushButton_3;
    QPushButton *pushButton_x;
    QPushButton *pushButton_4;
    QPushButton *pushButton_5;
    QPushButton *pushButton_6;
    QPushButton *pushButton_ce;
    QPushButton *pushButton_7;
    QPushButton *pushButton_8;
    QPushButton *pushButton_9;
    QPushButton *pushButton_esc;
    QPushButton *pushButton_0;
    QPushButton *pushButton_jian;
    QPushButton *pushButton_dian;
    QPushButton *enterButton;

    void setupUi(QDialog *Keyboard)
    {
        if (Keyboard->objectName().isEmpty())
            Keyboard->setObjectName(QStringLiteral("Keyboard"));
        Keyboard->setEnabled(true);
        Keyboard->resize(310, 306);
        QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(Keyboard->sizePolicy().hasHeightForWidth());
        Keyboard->setSizePolicy(sizePolicy);
        Keyboard->setStyleSheet(QStringLiteral("background-color: rgb(179, 179, 179);"));
        Keyboard->setSizeGripEnabled(false);
        gridLayout = new QGridLayout(Keyboard);
        gridLayout->setObjectName(QStringLiteral("gridLayout"));
        lineEdit = new QLineEdit(Keyboard);
        lineEdit->setObjectName(QStringLiteral("lineEdit"));
        lineEdit->setMaximumSize(QSize(16777215, 60));

        gridLayout->addWidget(lineEdit, 0, 0, 1, 4);

        pushButton = new QPushButton(Keyboard);
        pushButton->setObjectName(QStringLiteral("pushButton"));
        pushButton->setMaximumSize(QSize(65, 50));

        gridLayout->addWidget(pushButton, 1, 0, 1, 1);

        pushButton_2 = new QPushButton(Keyboard);
        pushButton_2->setObjectName(QStringLiteral("pushButton_2"));
        pushButton_2->setMaximumSize(QSize(65, 50));

        gridLayout->addWidget(pushButton_2, 1, 1, 1, 1);

        pushButton_3 = new QPushButton(Keyboard);
        pushButton_3->setObjectName(QStringLiteral("pushButton_3"));
        pushButton_3->setMaximumSize(QSize(65, 50));

        gridLayout->addWidget(pushButton_3, 1, 2, 1, 1);

        pushButton_x = new QPushButton(Keyboard);
        pushButton_x->setObjectName(QStringLiteral("pushButton_x"));
        pushButton_x->setMaximumSize(QSize(65, 50));
        pushButton_x->setStyleSheet(QString::fromUtf8("background-color: rgb(135, 135, 202);\n"
"font: 75 12pt \"\351\273\221\344\275\223\";"));

        gridLayout->addWidget(pushButton_x, 1, 3, 1, 1);

        pushButton_4 = new QPushButton(Keyboard);
        pushButton_4->setObjectName(QStringLiteral("pushButton_4"));
        pushButton_4->setMaximumSize(QSize(65, 50));

        gridLayout->addWidget(pushButton_4, 2, 0, 1, 1);

        pushButton_5 = new QPushButton(Keyboard);
        pushButton_5->setObjectName(QStringLiteral("pushButton_5"));
        pushButton_5->setMaximumSize(QSize(65, 50));

        gridLayout->addWidget(pushButton_5, 2, 1, 1, 1);

        pushButton_6 = new QPushButton(Keyboard);
        pushButton_6->setObjectName(QStringLiteral("pushButton_6"));
        pushButton_6->setMaximumSize(QSize(65, 50));

        gridLayout->addWidget(pushButton_6, 2, 2, 1, 1);

        pushButton_ce = new QPushButton(Keyboard);
        pushButton_ce->setObjectName(QStringLiteral("pushButton_ce"));
        pushButton_ce->setMaximumSize(QSize(65, 50));
        pushButton_ce->setStyleSheet(QString::fromUtf8("background-color: rgb(135, 135, 202);\n"
"font: 75 12pt \"\351\273\221\344\275\223\";"));

        gridLayout->addWidget(pushButton_ce, 2, 3, 1, 1);

        pushButton_7 = new QPushButton(Keyboard);
        pushButton_7->setObjectName(QStringLiteral("pushButton_7"));
        pushButton_7->setMaximumSize(QSize(65, 50));

        gridLayout->addWidget(pushButton_7, 3, 0, 1, 1);

        pushButton_8 = new QPushButton(Keyboard);
        pushButton_8->setObjectName(QStringLiteral("pushButton_8"));
        pushButton_8->setMaximumSize(QSize(65, 50));

        gridLayout->addWidget(pushButton_8, 3, 1, 1, 1);

        pushButton_9 = new QPushButton(Keyboard);
        pushButton_9->setObjectName(QStringLiteral("pushButton_9"));
        pushButton_9->setMaximumSize(QSize(65, 50));

        gridLayout->addWidget(pushButton_9, 3, 2, 1, 1);

        pushButton_esc = new QPushButton(Keyboard);
        pushButton_esc->setObjectName(QStringLiteral("pushButton_esc"));
        pushButton_esc->setMaximumSize(QSize(65, 50));

        gridLayout->addWidget(pushButton_esc, 3, 3, 1, 1);

        pushButton_0 = new QPushButton(Keyboard);
        pushButton_0->setObjectName(QStringLiteral("pushButton_0"));
        pushButton_0->setMaximumSize(QSize(65, 50));

        gridLayout->addWidget(pushButton_0, 4, 0, 1, 1);

        pushButton_jian = new QPushButton(Keyboard);
        pushButton_jian->setObjectName(QStringLiteral("pushButton_jian"));
        pushButton_jian->setMaximumSize(QSize(65, 50));

        gridLayout->addWidget(pushButton_jian, 4, 1, 1, 1);

        pushButton_dian = new QPushButton(Keyboard);
        pushButton_dian->setObjectName(QStringLiteral("pushButton_dian"));
        pushButton_dian->setMaximumSize(QSize(65, 50));

        gridLayout->addWidget(pushButton_dian, 4, 2, 1, 1);

        enterButton = new QPushButton(Keyboard);
        enterButton->setObjectName(QStringLiteral("enterButton"));
        enterButton->setMaximumSize(QSize(65, 50));

        gridLayout->addWidget(enterButton, 4, 3, 1, 1);


        retranslateUi(Keyboard);

        QMetaObject::connectSlotsByName(Keyboard);
    } // setupUi

    void retranslateUi(QDialog *Keyboard)
    {
        Keyboard->setWindowTitle(QApplication::translate("Keyboard", "Dialog", Q_NULLPTR));
        pushButton->setText(QApplication::translate("Keyboard", "1", Q_NULLPTR));
        pushButton_2->setText(QApplication::translate("Keyboard", "2", Q_NULLPTR));
        pushButton_3->setText(QApplication::translate("Keyboard", "3", Q_NULLPTR));
        pushButton_x->setText(QApplication::translate("Keyboard", "\345\210\240\351\231\244", Q_NULLPTR));
        pushButton_4->setText(QApplication::translate("Keyboard", "4", Q_NULLPTR));
        pushButton_5->setText(QApplication::translate("Keyboard", "5", Q_NULLPTR));
        pushButton_6->setText(QApplication::translate("Keyboard", "6", Q_NULLPTR));
        pushButton_ce->setText(QApplication::translate("Keyboard", "\346\270\205\347\251\272", Q_NULLPTR));
        pushButton_7->setText(QApplication::translate("Keyboard", "7", Q_NULLPTR));
        pushButton_8->setText(QApplication::translate("Keyboard", "8", Q_NULLPTR));
        pushButton_9->setText(QApplication::translate("Keyboard", "9", Q_NULLPTR));
        pushButton_esc->setText(QApplication::translate("Keyboard", "\351\200\200\345\207\272", Q_NULLPTR));
        pushButton_0->setText(QApplication::translate("Keyboard", "0", Q_NULLPTR));
        pushButton_jian->setText(QApplication::translate("Keyboard", "-", Q_NULLPTR));
        pushButton_dian->setText(QApplication::translate("Keyboard", ".", Q_NULLPTR));
        enterButton->setText(QApplication::translate("Keyboard", "\347\241\256\345\256\232", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class Keyboard: public Ui_Keyboard {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_KEYBOARD_H

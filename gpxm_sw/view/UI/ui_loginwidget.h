/********************************************************************************
** Form generated from reading UI file 'loginwidget.ui'
**
** Created by: Qt User Interface Compiler version 5.7.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_LOGINWIDGET_H
#define UI_LOGINWIDGET_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_LoginWidget
{
public:
    QVBoxLayout *verticalLayout;
    QSpacerItem *topSpacer;
    QGroupBox *loginGroupBox;
    QFormLayout *formLayout;
    QLabel *usernameLabel;
    QLineEdit *usernameLineEdit;
    QLabel *passwordLabel;
    QLineEdit *passwordLineEdit;
    QHBoxLayout *horizontalLayout;
    QCheckBox *rememberPasswordCheckBox;
    QCheckBox *showPasswordCheckBox;
    QGroupBox *changeGroupBox;
    QFormLayout *formLayout_2;
    QLabel *newUsernameLabel;
    QLineEdit *newUsernameLineEdit;
    QLabel *newPasswordLabel;
    QLineEdit *newPasswordLineEdit;
    QLabel *confirmPasswordLabel;
    QLineEdit *confirmPasswordLineEdit;
    QHBoxLayout *buttonLayout;
    QSpacerItem *buttonSpacer;
    QPushButton *loginButton;
    QPushButton *cancelButton;
    QPushButton *changeCredentialsButton;
    QSpacerItem *buttonSpacer_2;

    void setupUi(QWidget *LoginWidget)
    {
        if (LoginWidget->objectName().isEmpty())
            LoginWidget->setObjectName(QStringLiteral("LoginWidget"));
        LoginWidget->resize(400, 300);
        verticalLayout = new QVBoxLayout(LoginWidget);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        topSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout->addItem(topSpacer);

        loginGroupBox = new QGroupBox(LoginWidget);
        loginGroupBox->setObjectName(QStringLiteral("loginGroupBox"));
        formLayout = new QFormLayout(loginGroupBox);
        formLayout->setObjectName(QStringLiteral("formLayout"));
        usernameLabel = new QLabel(loginGroupBox);
        usernameLabel->setObjectName(QStringLiteral("usernameLabel"));

        formLayout->setWidget(0, QFormLayout::LabelRole, usernameLabel);

        usernameLineEdit = new QLineEdit(loginGroupBox);
        usernameLineEdit->setObjectName(QStringLiteral("usernameLineEdit"));

        formLayout->setWidget(0, QFormLayout::FieldRole, usernameLineEdit);

        passwordLabel = new QLabel(loginGroupBox);
        passwordLabel->setObjectName(QStringLiteral("passwordLabel"));

        formLayout->setWidget(1, QFormLayout::LabelRole, passwordLabel);

        passwordLineEdit = new QLineEdit(loginGroupBox);
        passwordLineEdit->setObjectName(QStringLiteral("passwordLineEdit"));

        formLayout->setWidget(1, QFormLayout::FieldRole, passwordLineEdit);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        rememberPasswordCheckBox = new QCheckBox(loginGroupBox);
        rememberPasswordCheckBox->setObjectName(QStringLiteral("rememberPasswordCheckBox"));

        horizontalLayout->addWidget(rememberPasswordCheckBox);

        showPasswordCheckBox = new QCheckBox(loginGroupBox);
        showPasswordCheckBox->setObjectName(QStringLiteral("showPasswordCheckBox"));

        horizontalLayout->addWidget(showPasswordCheckBox);


        formLayout->setLayout(2, QFormLayout::SpanningRole, horizontalLayout);


        verticalLayout->addWidget(loginGroupBox);

        changeGroupBox = new QGroupBox(LoginWidget);
        changeGroupBox->setObjectName(QStringLiteral("changeGroupBox"));
        formLayout_2 = new QFormLayout(changeGroupBox);
        formLayout_2->setObjectName(QStringLiteral("formLayout_2"));
        newUsernameLabel = new QLabel(changeGroupBox);
        newUsernameLabel->setObjectName(QStringLiteral("newUsernameLabel"));

        formLayout_2->setWidget(0, QFormLayout::LabelRole, newUsernameLabel);

        newUsernameLineEdit = new QLineEdit(changeGroupBox);
        newUsernameLineEdit->setObjectName(QStringLiteral("newUsernameLineEdit"));

        formLayout_2->setWidget(0, QFormLayout::FieldRole, newUsernameLineEdit);

        newPasswordLabel = new QLabel(changeGroupBox);
        newPasswordLabel->setObjectName(QStringLiteral("newPasswordLabel"));

        formLayout_2->setWidget(1, QFormLayout::LabelRole, newPasswordLabel);

        newPasswordLineEdit = new QLineEdit(changeGroupBox);
        newPasswordLineEdit->setObjectName(QStringLiteral("newPasswordLineEdit"));
        newPasswordLineEdit->setEchoMode(QLineEdit::Password);

        formLayout_2->setWidget(1, QFormLayout::FieldRole, newPasswordLineEdit);

        confirmPasswordLabel = new QLabel(changeGroupBox);
        confirmPasswordLabel->setObjectName(QStringLiteral("confirmPasswordLabel"));

        formLayout_2->setWidget(2, QFormLayout::LabelRole, confirmPasswordLabel);

        confirmPasswordLineEdit = new QLineEdit(changeGroupBox);
        confirmPasswordLineEdit->setObjectName(QStringLiteral("confirmPasswordLineEdit"));
        confirmPasswordLineEdit->setEchoMode(QLineEdit::Password);

        formLayout_2->setWidget(2, QFormLayout::FieldRole, confirmPasswordLineEdit);


        verticalLayout->addWidget(changeGroupBox);

        buttonLayout = new QHBoxLayout();
        buttonLayout->setObjectName(QStringLiteral("buttonLayout"));
        buttonSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        buttonLayout->addItem(buttonSpacer);

        loginButton = new QPushButton(LoginWidget);
        loginButton->setObjectName(QStringLiteral("loginButton"));
        loginButton->setMinimumWidth(80);

        buttonLayout->addWidget(loginButton);

        cancelButton = new QPushButton(LoginWidget);
        cancelButton->setObjectName(QStringLiteral("cancelButton"));
        cancelButton->setMinimumWidth(80);

        buttonLayout->addWidget(cancelButton);

        changeCredentialsButton = new QPushButton(LoginWidget);
        changeCredentialsButton->setObjectName(QStringLiteral("changeCredentialsButton"));
        changeCredentialsButton->setMinimumWidth(80);

        buttonLayout->addWidget(changeCredentialsButton);

        buttonSpacer_2 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        buttonLayout->addItem(buttonSpacer_2);


        verticalLayout->addLayout(buttonLayout);

        verticalLayout->setStretch(2, 1);

        retranslateUi(LoginWidget);
        QObject::connect(loginButton, SIGNAL(clicked()), LoginWidget, SLOT(on_loginButton_clicked()));
        QObject::connect(cancelButton, SIGNAL(clicked()), LoginWidget, SLOT(on_cancelButton_clicked()));
        QObject::connect(showPasswordCheckBox, SIGNAL(toggled(bool)), LoginWidget, SLOT(on_showPasswordCheckBox_toggled(bool)));
        QObject::connect(changeCredentialsButton, SIGNAL(clicked()), LoginWidget, SLOT(on_changeCredentialsButton_clicked()));

        QMetaObject::connectSlotsByName(LoginWidget);
    } // setupUi

    void retranslateUi(QWidget *LoginWidget)
    {
        LoginWidget->setWindowTitle(QApplication::translate("LoginWidget", "\347\224\250\346\210\267\347\231\273\345\275\225", Q_NULLPTR));
        loginGroupBox->setTitle(QApplication::translate("LoginWidget", "\347\231\273\345\275\225\344\277\241\346\201\257", Q_NULLPTR));
        usernameLabel->setText(QApplication::translate("LoginWidget", "\347\224\250\346\210\267\345\220\215\357\274\232", Q_NULLPTR));
        usernameLineEdit->setPlaceholderText(QApplication::translate("LoginWidget", "\350\257\267\350\276\223\345\205\245\347\224\250\346\210\267\345\220\215", Q_NULLPTR));
        passwordLabel->setText(QApplication::translate("LoginWidget", "\345\257\206\347\240\201\357\274\232", Q_NULLPTR));
        passwordLineEdit->setPlaceholderText(QApplication::translate("LoginWidget", "\350\257\267\350\276\223\345\205\245\345\257\206\347\240\201", Q_NULLPTR));
        rememberPasswordCheckBox->setText(QApplication::translate("LoginWidget", "\350\256\260\344\275\217\345\257\206\347\240\201", Q_NULLPTR));
        showPasswordCheckBox->setText(QApplication::translate("LoginWidget", "\346\230\276\347\244\272\345\257\206\347\240\201", Q_NULLPTR));
        changeGroupBox->setTitle(QApplication::translate("LoginWidget", "\344\277\256\346\224\271\347\224\250\346\210\267\345\220\215/\345\257\206\347\240\201", Q_NULLPTR));
        newUsernameLabel->setText(QApplication::translate("LoginWidget", "\346\226\260\347\224\250\346\210\267\345\220\215\357\274\232", Q_NULLPTR));
        newUsernameLineEdit->setPlaceholderText(QApplication::translate("LoginWidget", "\350\257\267\350\276\223\345\205\245\346\226\260\347\224\250\346\210\267\345\220\215", Q_NULLPTR));
        newPasswordLabel->setText(QApplication::translate("LoginWidget", "\346\226\260\345\257\206\347\240\201\357\274\232", Q_NULLPTR));
        newPasswordLineEdit->setPlaceholderText(QApplication::translate("LoginWidget", "\350\257\267\350\276\223\345\205\245\346\226\260\345\257\206\347\240\201", Q_NULLPTR));
        confirmPasswordLabel->setText(QApplication::translate("LoginWidget", "\347\241\256\350\256\244\345\257\206\347\240\201\357\274\232", Q_NULLPTR));
        confirmPasswordLineEdit->setPlaceholderText(QApplication::translate("LoginWidget", "\350\257\267\345\206\215\346\254\241\350\276\223\345\205\245\346\226\260\345\257\206\347\240\201", Q_NULLPTR));
        loginButton->setText(QApplication::translate("LoginWidget", "\347\231\273\345\275\225", Q_NULLPTR));
        cancelButton->setText(QApplication::translate("LoginWidget", "\345\217\226\346\266\210", Q_NULLPTR));
        changeCredentialsButton->setText(QApplication::translate("LoginWidget", "\344\277\256\346\224\271", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class LoginWidget: public Ui_LoginWidget {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_LOGINWIDGET_H

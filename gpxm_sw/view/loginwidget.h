#ifndef LOGINWIDGET_H
#define LOGINWIDGET_H

#include <QDialog>
#include <QObject>
#include <QLineEdit>
#include <QPushButton>

namespace Ui {
class LoginWidget;
}

class LoginWidget : public QDialog
{
    Q_OBJECT

public:
    explicit LoginWidget(QWidget *parent = nullptr);
    ~LoginWidget();
    bool isLoginSucess();

private slots:
    void onLoginButton_clicked();
    void on_cancelButton_clicked();
    void on_showPasswordCheckBox_toggled(bool checked);
    void on_changeCredentialsButton_clicked();

private:
    Ui::LoginWidget *ui;

    bool hasUserFile();
    bool loadCredentials(QString &username, QString &password);
    bool saveCredentials(const QString &username, const QString &password);
    bool authenticateUser(const QString &username, const QString &password);
    void loadRememberedPassword();
    void accept();

    void initTouchFriendlyUI();
    void connectInputFocusSignals();
    void showSoftKeyboard();
    void hideSoftKeyboard();
    bool event(QEvent *event) override;
    bool eventFilter(QObject *watched, QEvent *event) override;
    bool m_loginStatus;

};

#endif // LOGINWIDGET_H

#include "loginwidget.h"
#include "ui_loginwidget.h"
#include <QInputMethod>    // 软键盘控制
#include <QGuiApplication> // 获取应用输入方法
#include <QTouchEvent>     // 触摸事件处理
#include <QDebug>
#include <QSettings>       // 新增：记住密码需要
#include <QMessageBox>     // 新增：弹窗需要
#include <QFocusEvent>     // 新增：焦点事件需要

LoginWidget::LoginWidget(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LoginWidget)
{
    ui->setupUi(this);

    // -------------------------- 1. 基础初始化（保留原有逻辑）--------------------------
    ui->passwordLineEdit->setEchoMode(QLineEdit::Password);
    loadRememberedPassword();
    QString username, password;
    if (loadCredentials(username, password)) {
        ui->usernameLineEdit->setText(username);
    } else {
        ui->usernameLineEdit->setText("admin");
    }

    // -------------------------- 2. Pad适配（修正Qt 5.7.1兼容问题）--------------------------
    initTouchFriendlyUI();       // 触摸友好界面
//    connectInputFocusSignals();  // 连接焦点信号（替换focused信号）
//    this->setAttribute(Qt::WA_AcceptTouchEvents, true); // 启用触摸
    ui->changeGroupBox->hide();
    ui->rememberPasswordCheckBox->hide();
    m_loginStatus = false;
    connect(ui->loginButton,SIGNAL(clicked()),this,SLOT(onLoginButton_clicked()));
}

LoginWidget::~LoginWidget()
{
    delete ui;
}

bool LoginWidget::isLoginSucess()
{
    return m_loginStatus;
}

// -------------------------- 1. 初始化触摸友好界面（无修改）--------------------------
void LoginWidget::initTouchFriendlyUI()
{
    // 输入框优化
    QFont editFont = ui->usernameLineEdit->font();
    editFont.setPointSize(16);
    QList<QLineEdit*> allEdits = findChildren<QLineEdit*>();
    foreach (QLineEdit* edit, allEdits) {
        edit->setFont(editFont);
        edit->setMinimumHeight(50);
        edit->setStyleSheet("QLineEdit { padding: 0 15px; border: 1px solid #CCCCCC; border-radius: 8px; }"
                           "QLineEdit:focus { border-color: #2196F3; }");
        edit->setFocusPolicy(Qt::StrongFocus);
    }

    // 按钮优化
    QFont btnFont = ui->loginButton->font();
    btnFont.setPointSize(14);
    QList<QPushButton*> allBtns = findChildren<QPushButton*>();
    foreach (QPushButton* btn, allBtns) {
        btn->setFont(btnFont);
        btn->setMinimumHeight(50);
        btn->setMinimumWidth(120);
        btn->setStyleSheet("QPushButton { background-color: #F5F5F5; border: 1px solid #CCCCCC; border-radius: 8px; }"
                           "QPushButton:hover { background-color: #E0E0E0; }"
                           "QPushButton:pressed { background-color: #D0D0D0; }");
    }

    // 复选框优化
    QFont checkFont = ui->rememberPasswordCheckBox->font();
    checkFont.setPointSize(14);
    ui->rememberPasswordCheckBox->setFont(checkFont);
    ui->showPasswordCheckBox->setFont(checkFont);
}

// -------------------------- 2. 连接焦点信号（修正：用focusInEvent替代focused信号）--------------------------
void LoginWidget::connectInputFocusSignals()
{
    // 方案：给每个输入框安装事件过滤器，监听焦点进入/离开事件（Qt 5.7无focused信号）
    ui->usernameLineEdit->installEventFilter(this);
    ui->passwordLineEdit->installEventFilter(this);
    ui->newUsernameLineEdit->installEventFilter(this);
    ui->newPasswordLineEdit->installEventFilter(this);
    ui->confirmPasswordLineEdit->installEventFilter(this);
}

// -------------------------- 3. 事件过滤器（核心：监听输入框焦点/触摸事件）--------------------------
bool LoginWidget::eventFilter(QObject *watched, QEvent *event)
{
    // 3.1 监听输入框焦点进入事件：显示软键盘
    if (event->type() == QEvent::FocusIn) {
        QLineEdit* edit = qobject_cast<QLineEdit*>(watched);
        if (edit) { // 确认是输入框触发的焦点事件
            showSoftKeyboard();
            return true; // 处理事件，避免重复触发
        }
    }

    // 3.2 监听输入框焦点离开事件：隐藏软键盘
    if (event->type() == QEvent::FocusOut) {
        QLineEdit* edit = qobject_cast<QLineEdit*>(watched);
        if (edit) {
            hideSoftKeyboard();
            return true;
        }
    }

    // 其他事件交给父类处理
    return QDialog::eventFilter(watched, event);
}

// -------------------------- 4. 显示/隐藏软键盘（无修改）--------------------------
void LoginWidget::showSoftKeyboard()
{

    QInputMethod* inputMethod = QGuiApplication::inputMethod();
    if (inputMethod) {
        inputMethod->show();
        qDebug() << QString::fromLocal8Bit("[SoftKeyboard] 显示系统软键盘");
    } else {
        qWarning() << QString::fromLocal8Bit( "[SoftKeyboard] 未找到系统软键盘");
    }
}

void LoginWidget::hideSoftKeyboard()
{
    QInputMethod* inputMethod = QGuiApplication::inputMethod();
    if (inputMethod) {
        inputMethod->hide();
        qDebug() << QString::fromLocal8Bit("[SoftKeyboard] 隐藏系统软键盘");
    }
}

// -------------------------- 5. 重写触摸事件（修正：移除Qt 5.7无的TouchFocusReason）--------------------------
bool LoginWidget::event(QEvent* event)
{
    // 处理触摸开始事件（触摸输入框时让其获取焦点）
    if (event->type() == QEvent::TouchBegin) {
        QTouchEvent* touchEvent = static_cast<QTouchEvent*>(event);
        if (!touchEvent->touchPoints().isEmpty()) {
            QPointF touchPos = touchEvent->touchPoints().first().pos();
            QList<QLineEdit*> allEdits = findChildren<QLineEdit*>();
            foreach (QLineEdit* edit, allEdits) {
                if (edit->geometry().contains(touchPos.toPoint())) {
                    // 修正：Qt 5.7无TouchFocusReason，用MouseFocusReason替代（兼容触摸）
                    edit->setFocus(Qt::MouseFocusReason);
                    return true;
                }
            }
            hideSoftKeyboard(); // 触摸非输入框，隐藏软键盘
        }
    }

    return QDialog::event(event);
}

// -------------------------- 6. 登录按钮逻辑（修正：补充头文件后无错误）--------------------------
void LoginWidget::onLoginButton_clicked()
{
    QString username = ui->usernameLineEdit->text().trimmed();
    QString password = ui->passwordLineEdit->text();

    if (username.isEmpty() || password.isEmpty()) {
        QMessageBox::warning(this, QString::fromLocal8Bit("输入错误"), QString::fromLocal8Bit("用户名和密码不能为空！"));
        return;
    }

    if (authenticateUser(username, password)) {
        // 记住密码（QSettings需包含头文件）
        QSettings settings("MyApp", "LoginSystem");
        if (ui->rememberPasswordCheckBox->isChecked()) {
            settings.setValue("rememberPassword", true);
            settings.setValue("username", username);
            settings.setValue("password", password);
        } else {
            settings.setValue("rememberPassword", false);
            settings.remove("username");
            settings.remove("password");
        }

        QMessageBox::information(this, QString::fromLocal8Bit("登录成功"), QString::fromLocal8Bit("欢迎使用！"));
        accept();
    } else {
        QMessageBox::critical(this, QString::fromLocal8Bit("登录失败"), QString::fromLocal8Bit("用户名或密码错误！"));
    }
}

// -------------------------- 7. 其他原有逻辑（无修改，补充头文件后正常）--------------------------
void LoginWidget::on_cancelButton_clicked()
{
    close();
}

void LoginWidget::on_showPasswordCheckBox_toggled(bool checked)
{
    ui->passwordLineEdit->setEchoMode(checked ? QLineEdit::Normal : QLineEdit::Password);
}

void LoginWidget::on_changeCredentialsButton_clicked()
{
    QString currentUsername = ui->usernameLineEdit->text().trimmed();
    QString currentPassword = ui->passwordLineEdit->text();
    QString newUsername = ui->newUsernameLineEdit->text().trimmed();
    QString newPassword = ui->newPasswordLineEdit->text();
    QString confirmPassword = ui->confirmPasswordLineEdit->text();

    if (!authenticateUser(currentUsername, currentPassword)) {
        QMessageBox::critical(this, QString::fromLocal8Bit("验证失败"), QString::fromLocal8Bit("当前用户名或密码错误，无法修改！"));
        return;
    }

    if (newUsername.isEmpty() || newPassword.isEmpty()) {
        QMessageBox::warning(this, QString::fromLocal8Bit("输入错误"), QString::fromLocal8Bit("新用户名和密码不能为空！"));
        return;
    }

    if (newPassword != confirmPassword) {
        QMessageBox::warning(this, QString::fromLocal8Bit("输入错误"), QString::fromLocal8Bit("两次输入的新密码不一致！"));
        return;
    }

    if (saveCredentials(newUsername, newPassword)) {
        QMessageBox::information(this, QString::fromLocal8Bit("修改成功"), QString::fromLocal8Bit("用户名和密码已更新！"));
        ui->usernameLineEdit->setText(newUsername);
        ui->passwordLineEdit->setText(newPassword);
        ui->newUsernameLineEdit->clear();
        ui->newPasswordLineEdit->clear();
        ui->confirmPasswordLineEdit->clear();
    } else {
        QMessageBox::critical(this, QString::fromLocal8Bit("修改失败"), QString::fromLocal8Bit("保存新凭据时出错！"));
    }
}

bool LoginWidget::hasUserFile()
{
    QFile file("user_credentials.dat");
    return file.exists();
}

bool LoginWidget::loadCredentials(QString &username, QString &password)
{
    QFile file("user_credentials.dat");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) return false;

    QTextStream in(&file);
    username = in.readLine();
    password = in.readLine();
    file.close();

    return !username.isEmpty() && !password.isEmpty();
}

bool LoginWidget::saveCredentials(const QString &username, const QString &password)
{
    QFile file("user_credentials.dat");
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) return false;

    QTextStream out(&file);
    out << username << endl;
    out << password << endl;
    file.close();

    return true;
}

bool LoginWidget::authenticateUser(const QString &username, const QString &password)
{
    if (hasUserFile()) {
        QString storedUsername, storedPassword;
        if (loadCredentials(storedUsername, storedPassword)) {
            return (username == storedUsername && password == storedPassword);
        }
    }
    return (username == "admin" && password == "admin");
}

void LoginWidget::loadRememberedPassword()
{
    QSettings settings("MyApp", "LoginSystem");
    if (settings.value("rememberPassword", false).toBool()) {
        ui->usernameLineEdit->setText(settings.value("username").toString());
        ui->passwordLineEdit->setText(settings.value("password").toString());
        ui->rememberPasswordCheckBox->setChecked(true);
    }
}

void LoginWidget::accept()
{
    m_loginStatus = true;
    this->hide(); // 登录成功后隐藏窗口
}

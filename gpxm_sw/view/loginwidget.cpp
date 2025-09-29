#include "loginwidget.h"
#include "ui_loginwidget.h"
#include <QInputMethod>    // ����̿���
#include <QGuiApplication> // ��ȡӦ�����뷽��
#include <QTouchEvent>     // �����¼�����
#include <QDebug>
#include <QSettings>       // ��������ס������Ҫ
#include <QMessageBox>     // ������������Ҫ
#include <QFocusEvent>     // �����������¼���Ҫ

LoginWidget::LoginWidget(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LoginWidget)
{
    ui->setupUi(this);

    // -------------------------- 1. ������ʼ��������ԭ���߼���--------------------------
    ui->passwordLineEdit->setEchoMode(QLineEdit::Password);
    loadRememberedPassword();
    QString username, password;
    if (loadCredentials(username, password)) {
        ui->usernameLineEdit->setText(username);
    } else {
        ui->usernameLineEdit->setText("admin");
    }

    // -------------------------- 2. Pad���䣨����Qt 5.7.1�������⣩--------------------------
    initTouchFriendlyUI();       // �����Ѻý���
//    connectInputFocusSignals();  // ���ӽ����źţ��滻focused�źţ�
//    this->setAttribute(Qt::WA_AcceptTouchEvents, true); // ���ô���
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

// -------------------------- 1. ��ʼ�������Ѻý��棨���޸ģ�--------------------------
void LoginWidget::initTouchFriendlyUI()
{
    // ������Ż�
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

    // ��ť�Ż�
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

    // ��ѡ���Ż�
    QFont checkFont = ui->rememberPasswordCheckBox->font();
    checkFont.setPointSize(14);
    ui->rememberPasswordCheckBox->setFont(checkFont);
    ui->showPasswordCheckBox->setFont(checkFont);
}

// -------------------------- 2. ���ӽ����źţ���������focusInEvent���focused�źţ�--------------------------
void LoginWidget::connectInputFocusSignals()
{
    // ��������ÿ�������װ�¼��������������������/�뿪�¼���Qt 5.7��focused�źţ�
    ui->usernameLineEdit->installEventFilter(this);
    ui->passwordLineEdit->installEventFilter(this);
    ui->newUsernameLineEdit->installEventFilter(this);
    ui->newPasswordLineEdit->installEventFilter(this);
    ui->confirmPasswordLineEdit->installEventFilter(this);
}

// -------------------------- 3. �¼������������ģ���������򽹵�/�����¼���--------------------------
bool LoginWidget::eventFilter(QObject *watched, QEvent *event)
{
    // 3.1 ��������򽹵�����¼�����ʾ�����
    if (event->type() == QEvent::FocusIn) {
        QLineEdit* edit = qobject_cast<QLineEdit*>(watched);
        if (edit) { // ȷ��������򴥷��Ľ����¼�
            showSoftKeyboard();
            return true; // �����¼��������ظ�����
        }
    }

    // 3.2 ��������򽹵��뿪�¼������������
    if (event->type() == QEvent::FocusOut) {
        QLineEdit* edit = qobject_cast<QLineEdit*>(watched);
        if (edit) {
            hideSoftKeyboard();
            return true;
        }
    }

    // �����¼��������ദ��
    return QDialog::eventFilter(watched, event);
}

// -------------------------- 4. ��ʾ/��������̣����޸ģ�--------------------------
void LoginWidget::showSoftKeyboard()
{

    QInputMethod* inputMethod = QGuiApplication::inputMethod();
    if (inputMethod) {
        inputMethod->show();
        qDebug() << QString::fromLocal8Bit("[SoftKeyboard] ��ʾϵͳ�����");
    } else {
        qWarning() << QString::fromLocal8Bit( "[SoftKeyboard] δ�ҵ�ϵͳ�����");
    }
}

void LoginWidget::hideSoftKeyboard()
{
    QInputMethod* inputMethod = QGuiApplication::inputMethod();
    if (inputMethod) {
        inputMethod->hide();
        qDebug() << QString::fromLocal8Bit("[SoftKeyboard] ����ϵͳ�����");
    }
}

// -------------------------- 5. ��д�����¼����������Ƴ�Qt 5.7�޵�TouchFocusReason��--------------------------
bool LoginWidget::event(QEvent* event)
{
    // ��������ʼ�¼������������ʱ�����ȡ���㣩
    if (event->type() == QEvent::TouchBegin) {
        QTouchEvent* touchEvent = static_cast<QTouchEvent*>(event);
        if (!touchEvent->touchPoints().isEmpty()) {
            QPointF touchPos = touchEvent->touchPoints().first().pos();
            QList<QLineEdit*> allEdits = findChildren<QLineEdit*>();
            foreach (QLineEdit* edit, allEdits) {
                if (edit->geometry().contains(touchPos.toPoint())) {
                    // ������Qt 5.7��TouchFocusReason����MouseFocusReason��������ݴ�����
                    edit->setFocus(Qt::MouseFocusReason);
                    return true;
                }
            }
            hideSoftKeyboard(); // ��������������������
        }
    }

    return QDialog::event(event);
}

// -------------------------- 6. ��¼��ť�߼�������������ͷ�ļ����޴���--------------------------
void LoginWidget::onLoginButton_clicked()
{
    QString username = ui->usernameLineEdit->text().trimmed();
    QString password = ui->passwordLineEdit->text();

    if (username.isEmpty() || password.isEmpty()) {
        QMessageBox::warning(this, QString::fromLocal8Bit("�������"), QString::fromLocal8Bit("�û��������벻��Ϊ�գ�"));
        return;
    }

    if (authenticateUser(username, password)) {
        // ��ס���루QSettings�����ͷ�ļ���
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

        QMessageBox::information(this, QString::fromLocal8Bit("��¼�ɹ�"), QString::fromLocal8Bit("��ӭʹ�ã�"));
        accept();
    } else {
        QMessageBox::critical(this, QString::fromLocal8Bit("��¼ʧ��"), QString::fromLocal8Bit("�û������������"));
    }
}

// -------------------------- 7. ����ԭ���߼������޸ģ�����ͷ�ļ���������--------------------------
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
        QMessageBox::critical(this, QString::fromLocal8Bit("��֤ʧ��"), QString::fromLocal8Bit("��ǰ�û�������������޷��޸ģ�"));
        return;
    }

    if (newUsername.isEmpty() || newPassword.isEmpty()) {
        QMessageBox::warning(this, QString::fromLocal8Bit("�������"), QString::fromLocal8Bit("���û��������벻��Ϊ�գ�"));
        return;
    }

    if (newPassword != confirmPassword) {
        QMessageBox::warning(this, QString::fromLocal8Bit("�������"), QString::fromLocal8Bit("��������������벻һ�£�"));
        return;
    }

    if (saveCredentials(newUsername, newPassword)) {
        QMessageBox::information(this, QString::fromLocal8Bit("�޸ĳɹ�"), QString::fromLocal8Bit("�û����������Ѹ��£�"));
        ui->usernameLineEdit->setText(newUsername);
        ui->passwordLineEdit->setText(newPassword);
        ui->newUsernameLineEdit->clear();
        ui->newPasswordLineEdit->clear();
        ui->confirmPasswordLineEdit->clear();
    } else {
        QMessageBox::critical(this, QString::fromLocal8Bit("�޸�ʧ��"), QString::fromLocal8Bit("������ƾ��ʱ����"));
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
    this->hide(); // ��¼�ɹ������ش���
}

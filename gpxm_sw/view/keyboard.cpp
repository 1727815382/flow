#include "keyboard.h"
#include "ui_keyboard.h"

#include <QMouseEvent>

Keyboard::Keyboard(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Keyboard)
{
    ui->setupUi(this);
    setWindowFlags(Qt::Window | Qt::FramelessWindowHint);

    setAttribute(Qt::WA_OpaquePaintEvent);
    setStyleSheet("QDialog { background-color: rgb(179, 179, 179); }");
    setStyleSheet("QPushButton {background-color: rgb(135, 135, 202); font: bold 12pt \"黑体\"};");

    connect(ui->pushButton_0, &QPushButton::clicked, this, &Keyboard::onButtonClicked);
    connect(ui->pushButton, &QPushButton::clicked, this, &Keyboard::onButtonClicked);
    connect(ui->pushButton_2, &QPushButton::clicked, this, &Keyboard::onButtonClicked);
    connect(ui->pushButton_3, &QPushButton::clicked, this, &Keyboard::onButtonClicked);
    connect(ui->pushButton_4, &QPushButton::clicked, this, &Keyboard::onButtonClicked);
    connect(ui->pushButton_5, &QPushButton::clicked, this, &Keyboard::onButtonClicked);
    connect(ui->pushButton_6, &QPushButton::clicked, this, &Keyboard::onButtonClicked);
    connect(ui->pushButton_7, &QPushButton::clicked, this, &Keyboard::onButtonClicked);
    connect(ui->pushButton_8, &QPushButton::clicked, this, &Keyboard::onButtonClicked);
    connect(ui->pushButton_9, &QPushButton::clicked, this, &Keyboard::onButtonClicked);
    connect(ui->pushButton_x, &QPushButton::clicked, this, &Keyboard::onBackspaceClicked);
    connect(ui->pushButton_ce, &QPushButton::clicked, this, &Keyboard::onClearClicked);
    connect(ui->pushButton_dian, &QPushButton::clicked, this, &Keyboard::onDotClicked);
    connect(ui->pushButton_jian, &QPushButton::clicked, this, &Keyboard::onMinusClicked);
    connect(ui->pushButton_esc, &QPushButton::clicked, this, &Keyboard::onCloseClicked);
    connect(ui->enterButton, &QPushButton::clicked, this, &Keyboard::onEnterClicked);

    m_Minimum = -9999999;
    m_Maximum = 999999999999;
    ui->lineEdit->selectAll();

}

Keyboard::~Keyboard()
{
    delete ui;
}

void Keyboard::onButtonClicked()
{
    QPushButton *button = qobject_cast<QPushButton*>(sender());
    if (button)
    {
        if(ui->lineEdit->hasSelectedText()||ui->lineEdit->hasFocus())
        {
            ui->lineEdit->clear();
        }
        ui->lineEdit->insert(button->text());
        bool ok = false;
        const QString t = ui->lineEdit->text();
        const double val = t.toDouble(&ok);

        if (ok)
        {
            const double bounded = qBound(m_Minimum, val, m_Maximum);
            if (val < m_Minimum || val > m_Maximum)
            {
                ui->lineEdit->setText(QString::number(bounded));
            }
        }

//        // 自动钳位到控件的最小/最大值
//        double inputValue = ui->lineEdit->text().toDouble();
//         inputValue = qBound(m_Minimum, inputValue, m_Maximum);
//         ui->lineEdit->setText(QString::number(inputValue));
    }
}

void Keyboard::onBackspaceClicked()
{
    QString currentText = ui->lineEdit->text();
    if (!currentText.isEmpty()) {
        currentText.chop(1);
        ui->lineEdit->setText(currentText);
    }
}

void Keyboard::onClearClicked()
{
    ui->lineEdit->clear();
}

void Keyboard::onDotClicked()
{
    if (!ui->lineEdit->text().contains('.')) {
        ui->lineEdit->insert(".");
    }
}

void Keyboard::onMinusClicked()
{
    QString currentText = ui->lineEdit->text();
    if (!currentText.startsWith('-')) {
        ui->lineEdit->setText('-' + currentText);
    } else {
        ui->lineEdit->setText(currentText.remove(0, 1));
    }
}


void Keyboard::onCloseClicked()
{
    this->close();
}


void Keyboard::onEnterClicked()
{
    emit enterPressed(ui->lineEdit->text());
    accept();
}

void Keyboard::setLabelText(const QString &text)
{
    ui->lineEdit->setText(text);
     ui->lineEdit->selectAll();
     dragPosition = QPoint(0,0);
}

void Keyboard::setMaxMinNum(double max, double min)
{
    m_Minimum = min;
    m_Maximum = max;
}

void Keyboard::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        dragPosition = event->globalPos() - frameGeometry().topLeft();
        event->accept();
    }
}

void Keyboard::mouseMoveEvent(QMouseEvent *event)
{
    if (event->buttons() & Qt::LeftButton)
    {
        move(event->globalPos() - dragPosition);
        event->accept();
    }
}


#include "CustomDoubleSpinBox.h"

#include <QMouseEvent>
#include<QMessageBox>
#include <QAbstractButton>
#include <QDebug>

CustomDoubleSpinBox::CustomDoubleSpinBox(QWidget *parent) : QDoubleSpinBox(parent)
{
     m_Minimum = 0.0;
     m_Maximum = 99.0;
    // 关键：获取内部的 QLineEdit 子控件（输入区域）
       m_lineEdit = findChild<QLineEdit*>();
       if (m_lineEdit) {
           // 给输入框安装事件过滤器，监听其点击事件
           m_lineEdit->installEventFilter(this);
           qDebug() << QString::fromLocal8Bit("[CustomSpinBox] 找到到内部输入框，已安装事件过滤器");
       } else {
           qWarning() << QString::fromLocal8Bit("[CustomSpinBox] 未找到内部输入框！");
       }
}
void CustomDoubleSpinBox::mousePressEvent(QMouseEvent *event)
{
    // 1. 先执行父类默认逻辑（确保箭头按钮点击能正常增减数值）
      QDoubleSpinBox::mousePressEvent(event);

}
// 事件过滤器：捕捉内部输入框的点击事件
bool CustomDoubleSpinBox::eventFilter(QObject *watched, QEvent *event)
{
    // 仅处理内部输入框的鼠标点击事件
    if (watched == m_lineEdit && event->type() == QEvent::MouseButtonPress) {
        qDebug() << "[CustomSpinBox] 捕捉到输入框点击事件";

        // 弹出自定义键盘
        Keyboard *keyboardDialog = new Keyboard(this);
        keyboardDialog->setAttribute(Qt::WA_DeleteOnClose);
        keyboardDialog->setLabelText(this->text()); // 传入当前值
        keyboardDialog->setMaxMinNum(99.0,0.0);
        connect(keyboardDialog, &Keyboard::enterPressed,
                this, &CustomDoubleSpinBox::updateTextFromKeyboard);
        keyboardDialog->exec();

        return true; // 拦截事件，避免重复处理
    }

    // 其他事件交给父类处理
    return QDoubleSpinBox::eventFilter(watched, event);
}

void CustomDoubleSpinBox::updateTextFromKeyboard(const QString &text)
{
    if (text.isEmpty()) {
        return;
    }

    // 2. 转换输入文本为double（处理非法数值，如字母、符号）
    bool isLegal = false;
    double inputValue = text.toDouble(&isLegal);

    // 3. 校验数值合法性（包括是否在控件的数值范围内）
    if (!isLegal) {
        // 非法数值：提示用户
        QMessageBox::warning(this, QString::fromLocal8Bit( "输入错误"),  QString::fromLocal8Bit("请输入合法的数字（如 123.45）！"));
        return;
    } else if (inputValue < m_Minimum || inputValue > m_Maximum) {
        // 数值超出控件范围：提示用户并自动钳位到合法范围
         QString info = QString::fromLocal8Bit("请输入 %1 ~ %2 之间的数值，将自动调整为合法值").arg(m_Minimum).arg(m_Maximum);
        QMessageBox::warning(this, QString::fromLocal8Bit("数值超限"),
           info);
        // 自动钳位到控件的最小/最大值
        inputValue = qBound(m_Minimum, inputValue, m_Maximum);
    }
    if(text.isEmpty())
    {

    }
    else
    {
        setValue(inputValue);
    }
}

#include "CustomDoubleSpinBox.h"

#include <QMouseEvent>
#include<QMessageBox>
#include <QAbstractButton>
#include <QDebug>

CustomDoubleSpinBox::CustomDoubleSpinBox(QWidget *parent) : QDoubleSpinBox(parent)
{
     m_Minimum = 0.0;
     m_Maximum = 99.0;
    // �ؼ�����ȡ�ڲ��� QLineEdit �ӿؼ�����������
       m_lineEdit = findChild<QLineEdit*>();
       if (m_lineEdit) {
           // �������װ�¼������������������¼�
           m_lineEdit->installEventFilter(this);
           qDebug() << QString::fromLocal8Bit("[CustomSpinBox] �ҵ����ڲ�������Ѱ�װ�¼�������");
       } else {
           qWarning() << QString::fromLocal8Bit("[CustomSpinBox] δ�ҵ��ڲ������");
       }
}
void CustomDoubleSpinBox::mousePressEvent(QMouseEvent *event)
{
    // 1. ��ִ�и���Ĭ���߼���ȷ����ͷ��ť���������������ֵ��
      QDoubleSpinBox::mousePressEvent(event);

}
// �¼�����������׽�ڲ������ĵ���¼�
bool CustomDoubleSpinBox::eventFilter(QObject *watched, QEvent *event)
{
    // �������ڲ�������������¼�
    if (watched == m_lineEdit && event->type() == QEvent::MouseButtonPress) {
        qDebug() << "[CustomSpinBox] ��׽����������¼�";

        // �����Զ������
        Keyboard *keyboardDialog = new Keyboard(this);
        keyboardDialog->setAttribute(Qt::WA_DeleteOnClose);
        keyboardDialog->setLabelText(this->text()); // ���뵱ǰֵ
        keyboardDialog->setMaxMinNum(99.0,0.0);
        connect(keyboardDialog, &Keyboard::enterPressed,
                this, &CustomDoubleSpinBox::updateTextFromKeyboard);
        keyboardDialog->exec();

        return true; // �����¼��������ظ�����
    }

    // �����¼��������ദ��
    return QDoubleSpinBox::eventFilter(watched, event);
}

void CustomDoubleSpinBox::updateTextFromKeyboard(const QString &text)
{
    if (text.isEmpty()) {
        return;
    }

    // 2. ת�������ı�Ϊdouble������Ƿ���ֵ������ĸ�����ţ�
    bool isLegal = false;
    double inputValue = text.toDouble(&isLegal);

    // 3. У����ֵ�Ϸ��ԣ������Ƿ��ڿؼ�����ֵ��Χ�ڣ�
    if (!isLegal) {
        // �Ƿ���ֵ����ʾ�û�
        QMessageBox::warning(this, QString::fromLocal8Bit( "�������"),  QString::fromLocal8Bit("������Ϸ������֣��� 123.45����"));
        return;
    } else if (inputValue < m_Minimum || inputValue > m_Maximum) {
        // ��ֵ�����ؼ���Χ����ʾ�û����Զ�ǯλ���Ϸ���Χ
         QString info = QString::fromLocal8Bit("������ %1 ~ %2 ֮�����ֵ�����Զ�����Ϊ�Ϸ�ֵ").arg(m_Minimum).arg(m_Maximum);
        QMessageBox::warning(this, QString::fromLocal8Bit("��ֵ����"),
           info);
        // �Զ�ǯλ���ؼ�����С/���ֵ
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

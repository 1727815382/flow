#ifndef CUSTOMDOUBLESPINBOX_H
#define CUSTOMDOUBLESPINBOX_H

#include <QWidget>
#include "keyboard.h"
#include <QDoubleSpinBox>
#include <QLineEdit>
class CustomDoubleSpinBox : public QDoubleSpinBox
{
    Q_OBJECT
public:
    explicit CustomDoubleSpinBox(QWidget *parent = 0);
    void mousePressEvent(QMouseEvent *event);
    bool eventFilter(QObject *watched, QEvent *event) override;
signals:

public slots:
    void updateTextFromKeyboard(const QString &text);
private:
     QLineEdit *m_lineEdit;
     double m_Minimum;
     double m_Maximum;
};

#endif // CUSTOMDOUBLESPINBOX_H

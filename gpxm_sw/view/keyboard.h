#ifndef KEYBOARD_H
#define KEYBOARD_H

#include <QDialog>

namespace Ui {
class Keyboard;
}

class Keyboard : public QDialog
{
    Q_OBJECT

public:
    explicit Keyboard(QWidget *parent = 0);
    ~Keyboard();

    void mouseMoveEvent(QMouseEvent *event);
    void onButtonClicked();
    void onBackspaceClicked();
    void onClearClicked();
    void onDotClicked();
    void onMinusClicked();
    void onCloseClicked();
    void onEnterClicked();
    void setLabelText(const QString &text);
    void setMaxMinNum(double max,double min);

    void mousePressEvent(QMouseEvent *event);
signals:
    void enterPressed(QString text);


private:
    Ui::Keyboard *ui;
    QPoint dragPosition;
    double m_Minimum;
    double m_Maximum;
};

#endif // KEYBOARD_H

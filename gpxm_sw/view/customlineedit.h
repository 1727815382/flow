#ifndef CUSTOMLINEEDIT_H
#define CUSTOMLINEEDIT_H

#include <QObject>
#include "keyboard.h"
#include <QLineEdit>

class CustomLineEdit : public QLineEdit
{
    Q_OBJECT
public:
    explicit CustomLineEdit(QWidget *parent = 0);

    void mousePressEvent(QMouseEvent *event);

signals:

public slots:
    void updateTextFromKeyboard(const QString &text);
};

#endif // CUSTOMLINEEDIT_H

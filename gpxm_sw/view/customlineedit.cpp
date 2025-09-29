#include "CustomLineEdit.h"
#include <QMouseEvent>
#include "keyboard.h"

CustomLineEdit::CustomLineEdit(QWidget *parent) :
    QLineEdit(parent)
{
}

void CustomLineEdit::mousePressEvent(QMouseEvent *event)
{
    QLineEdit::mousePressEvent(event);

    Keyboard keyboardDialog(this);
    connect(&keyboardDialog, &Keyboard::enterPressed, this, &CustomLineEdit::updateTextFromKeyboard);
    keyboardDialog.setLabelText(this->text());
    keyboardDialog.exec();
}

void CustomLineEdit::updateTextFromKeyboard(const QString &text)
{

    if(text.isEmpty())
    {
        clear();
    }
    else
    {
        setText(text);
    }
}

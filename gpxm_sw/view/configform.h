#ifndef CONFIGFORM_H
#define CONFIGFORM_H

#include <QWidget>
#include "file/inifunc.h"

namespace Ui {
class ConfigForm;
}

class ConfigForm : public QWidget
{
    Q_OBJECT

public:
    explicit ConfigForm(QWidget *parent = 0);
    ~ConfigForm();

    void keyPressEvent(QKeyEvent *event);

    void configSet();

private slots:

    void slotSave ();

private:
    Ui::ConfigForm *ui;
    IniFunc *m_IniFunc;

};

#endif // CONFIGFORM_H

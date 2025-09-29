#ifndef TQRCALCULATIONFORM_H
#define TQRCALCULATIONFORM_H

#include <QWidget>

namespace Ui {
class TQRCalculationForm;
}

class TQRCalculationForm : public QWidget
{
    Q_OBJECT

public:
    explicit TQRCalculationForm(QWidget *parent = 0);
    ~TQRCalculationForm();
    void CalculationShow(QVector<QVector<double> > &m_td);


private:
    Ui::TQRCalculationForm *ui;
};

#endif // TQRCALCULATIONFORM_H

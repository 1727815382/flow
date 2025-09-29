#ifndef MYTABLE_H
#define MYTABLE_H

#include <QWidget>
#include <QTableWidget>
#include <QList>
#include <QString>

#include "aixsim.h"
#include <QStyledItemDelegate>
#include <QApplication>

namespace Ui {
class Mytable;
}

class Mytable : public QWidget
{
    Q_OBJECT

public:
    explicit Mytable(QWidget *parent = 0);
    ~Mytable();
    void setHlabel(QStringList header, int W = 1600);
    void setCurrentRow();
    void showData(QList<TrackMesure> &m_wd);
    void addData(TrackMesure data);
    void deleteData(int row);
    void cleanData();
    void modifyData(int row,TrackMesure data);
    void showData(QVector<QVector<double> > &m_td);

    QVector<TrackMesure> curSelectData ();
    int curSelectRow ();
    void datachack();

    void saveTableToCSV();
    QList<TrackMesure> loadCSVToTable();
signals:
    // �Զ����ź�
    void itemRowClicked(int row);

private:
    Ui::Mytable *ui;
    int column;
    int selectedRow;
};

//class CustomDelegate : public QStyledItemDelegate {
//protected:
//    void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const override {
//        QStyleOptionViewItem opt = option;
//        initStyleOption(&opt, index);

//        // �������л�ȡ����ɫ������洢��Qt::UserRole�У�
//        QColor bgColor = index.data(Qt::UserRole).value<QColor>();
//        if (bgColor.isValid())  {
//            opt.backgroundBrush  = bgColor; // ���Ǳ���ˢ
//        }

//        // ʹ��Ĭ�ϻ����߼���Ӧ��opt�еı�����
//        QApplication::style()->drawControl(QStyle::CE_ItemViewItem, &opt, painter);
//    }
//};
#endif // MYTABLE_H

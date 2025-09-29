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
    // 自定义信号
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

//        // 从数据中获取背景色（假设存储在Qt::UserRole中）
//        QColor bgColor = index.data(Qt::UserRole).value<QColor>();
//        if (bgColor.isValid())  {
//            opt.backgroundBrush  = bgColor; // 覆盖背景刷
//        }

//        // 使用默认绘制逻辑（应用opt中的背景）
//        QApplication::style()->drawControl(QStyle::CE_ItemViewItem, &opt, painter);
//    }
//};
#endif // MYTABLE_H

#ifndef COLORHEADER_H
#define COLORHEADER_H

#include <QHeaderView>
#include <QPainter>
// 自定义表头绘制（实现左右分色）
// 支持按列索引指定颜色的表头
class ColoredHeader : public QHeaderView {
    Q_OBJECT
public:
    explicit ColoredHeader(Qt::Orientation orientation, QWidget* parent = nullptr)
        : QHeaderView(orientation, parent)
    {
//        setSectionsClickable(true);
//        setHighlightSections(true);

        // 指定每列的颜色
        QMap<int, QColor> columnColors;
        columnColors.insert(0, QColor(40, 100, 175)); // 浅绿
        columnColors.insert(1, QColor(40, 100, 175)); // 浅绿
        columnColors.insert(2, QColor(40, 100, 175)); // 浅绿
        columnColors.insert(3, QColor(40, 100, 175)); // 浅绿
        columnColors.insert(4, QColor(40, 100, 175)); // 浅绿
        columnColors.insert(5, QColor(40, 100, 175)); // 浅绿
        columnColors.insert(6, QColor(40, 100, 175)); // 浅绿
        columnColors.insert(7, QColor(40, 100, 0)); // 浅黄
        columnColors.insert(8, QColor(40, 100, 0)); // 浅黄
        columnColors.insert(9, QColor(40, 100, 0)); // 浅黄
        columnColors.insert(10, QColor(40, 100, 0)); // 浅黄
        columnColors.insert(11, QColor(40, 100, 0)); // 浅黄
//        columnColors.insert(3, QColor(255, 255, 200)); // 浅黄
        this->setColumnColors(columnColors);

    }

    // 添加设置列颜色的方法
    void setColumnColors(const QMap<int, QColor>& colors)
    {
        _columnColors = colors;
        update();
    }

protected:
    void paintSection(QPainter* painter, const QRect& rect, int logicalIndex) const override {
        if (!painter->isActive()) return;

        // 查找该列的颜色（如果未指定则使用默认颜色）
        QColor bgColor = _columnColors.contains(logicalIndex)
                         ? _columnColors.value(logicalIndex)
                         : Qt::lightGray;

        // 绘制列背景
        painter->fillRect(rect, bgColor);

        // 绘制标题文本
        QString text = model()->headerData(logicalIndex, orientation(), Qt::DisplayRole).toString();
        painter->setPen(Qt::white);
        painter->drawText(rect, Qt::AlignCenter, text);

        // 绘制边框
        painter->setPen(Qt::gray);
        painter->drawRect(rect.adjusted(0, 0, -1, -1));
    }

private:
    QMap<int, QColor> _columnColors;
};

#endif // COLORHEADER_H

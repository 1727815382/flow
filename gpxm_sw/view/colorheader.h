#ifndef COLORHEADER_H
#define COLORHEADER_H

#include <QHeaderView>
#include <QPainter>
// �Զ����ͷ���ƣ�ʵ�����ҷ�ɫ��
// ֧�ְ�������ָ����ɫ�ı�ͷ
class ColoredHeader : public QHeaderView {
    Q_OBJECT
public:
    explicit ColoredHeader(Qt::Orientation orientation, QWidget* parent = nullptr)
        : QHeaderView(orientation, parent)
    {
//        setSectionsClickable(true);
//        setHighlightSections(true);

        // ָ��ÿ�е���ɫ
        QMap<int, QColor> columnColors;
        columnColors.insert(0, QColor(40, 100, 175)); // ǳ��
        columnColors.insert(1, QColor(40, 100, 175)); // ǳ��
        columnColors.insert(2, QColor(40, 100, 175)); // ǳ��
        columnColors.insert(3, QColor(40, 100, 175)); // ǳ��
        columnColors.insert(4, QColor(40, 100, 175)); // ǳ��
        columnColors.insert(5, QColor(40, 100, 175)); // ǳ��
        columnColors.insert(6, QColor(40, 100, 175)); // ǳ��
        columnColors.insert(7, QColor(40, 100, 0)); // ǳ��
        columnColors.insert(8, QColor(40, 100, 0)); // ǳ��
        columnColors.insert(9, QColor(40, 100, 0)); // ǳ��
        columnColors.insert(10, QColor(40, 100, 0)); // ǳ��
        columnColors.insert(11, QColor(40, 100, 0)); // ǳ��
//        columnColors.insert(3, QColor(255, 255, 200)); // ǳ��
        this->setColumnColors(columnColors);

    }

    // �����������ɫ�ķ���
    void setColumnColors(const QMap<int, QColor>& colors)
    {
        _columnColors = colors;
        update();
    }

protected:
    void paintSection(QPainter* painter, const QRect& rect, int logicalIndex) const override {
        if (!painter->isActive()) return;

        // ���Ҹ��е���ɫ�����δָ����ʹ��Ĭ����ɫ��
        QColor bgColor = _columnColors.contains(logicalIndex)
                         ? _columnColors.value(logicalIndex)
                         : Qt::lightGray;

        // �����б���
        painter->fillRect(rect, bgColor);

        // ���Ʊ����ı�
        QString text = model()->headerData(logicalIndex, orientation(), Qt::DisplayRole).toString();
        painter->setPen(Qt::white);
        painter->drawText(rect, Qt::AlignCenter, text);

        // ���Ʊ߿�
        painter->setPen(Qt::gray);
        painter->drawRect(rect.adjusted(0, 0, -1, -1));
    }

private:
    QMap<int, QColor> _columnColors;
};

#endif // COLORHEADER_H

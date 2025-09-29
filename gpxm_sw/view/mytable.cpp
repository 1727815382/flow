#include "Mytable.h"
#include "ui_Mytable.h"
#include "colorheader.h"
#include <QDebug>
#include <QtCore/qmath.h>
#include "../file/excelfunc.h"

extern ConfigPara m_configPara;

Mytable::Mytable(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Mytable)
{
    ui->setupUi(this);
    ui->tableWidget->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
    //ui->tableWidget->setAlternatingRowColors(true);
    // 创建自定义表头
    ColoredHeader* header = new ColoredHeader(Qt::Horizontal, ui->tableWidget);
    ui->tableWidget->setHorizontalHeader(header);

    // 设置选择行为为整行选择
    ui->tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableWidget->verticalHeader()->setVisible(false);
    ui->tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    selectedRow = -1;

    connect(ui->tableWidget, &QTableWidget::itemClicked, [=](QTableWidgetItem *item) {
        if(selectedRow == item->row())
        {
            selectedRow = -1;
            ui->tableWidget->setCurrentItem(NULL);
            emit itemRowClicked(-1);
        }
        else
        {
            if (item) {
                // 自动选择整行（由于设置了SelectRows行为）
                // 发射行选择信号
                selectedRow = item->row();
                emit itemRowClicked(item->row());
            }
        }

    });
//    connect(ui->tableWidget->verticalHeader(), SIGNAL(sectionClicked(int)), this, SIGNAL(itemRowClicked(int)));
}


Mytable::~Mytable()
{
    delete ui;
}

void Mytable::setHlabel(QStringList header, int W)
{
    column = header.size();
    ui->tableWidget->setColumnCount(column);
    ui->tableWidget->setHorizontalHeaderLabels(header);
    for(int i = 0; i < header.size()-1; i++)
    {
        ui->tableWidget->setColumnWidth(i, W/(header.size()) );
    }

    ui->tableWidget->horizontalHeader()->setStretchLastSection(true);
}

void Mytable::setCurrentRow()
{
}

void Mytable::showData(QList<TrackMesure> & tp)
{
    int rowSize = tp.size();
    ui->tableWidget->setRowCount(rowSize);
    for(int i = 0; i < rowSize; i++)
    {
#if 0
        //ui->tableWidget->setItem(i, 0, new QTableWidgetItem(QString::number(m_wd[i].SerialNumber)));
        ui->tableWidget->setItem(i, 0, new QTableWidgetItem(QString::number(tp[i]._id)));
        ui->tableWidget->setItem(i, 1, new QTableWidgetItem(QString::number(tp[i].Mileage)));
        ui->tableWidget->setItem(i, 2, new QTableWidgetItem(QString::number(tp[i].LRailElevationn)));
        ui->tableWidget->setItem(i, 3, new QTableWidgetItem(QString::number(tp[i].CLinePlann)));
        ui->tableWidget->setItem(i, 4, new QTableWidgetItem(QString::number(tp[i].RRailElevationn)));
        ui->tableWidget->setItem(i, 5, new QTableWidgetItem(QString::number(tp[i].SupDeviationn)));
        ui->tableWidget->setItem(i, 6, new QTableWidgetItem(QString::number(tp[i].GuiDeviationn)));
        ui->tableWidget->setItem(i, 7, new QTableWidgetItem(QString::number(tp[i].LGuiAdjustmentn)));
        ui->tableWidget->setItem(i, 8, new QTableWidgetItem(QString::number(tp[i].CLinedjustmentn)));
        ui->tableWidget->setItem(i, 9, new QTableWidgetItem(QString::number(tp[i].RGuiAdjustmentn)));
#endif
    }

}

void Mytable::showData(QVector<QVector<double>> &m_td)
{
    int rowSize = m_td.size();
    ui->tableWidget->setRowCount(rowSize);

    for(int i = 0; i < rowSize; i++)
    {
        QVector<double> rowData = m_td[i];
        int col = m_td[i].count();
        for(int j= 0; j<col; j++)
        {
            double colData = rowData[j];
            ui->tableWidget->setItem(i, j, new QTableWidgetItem(QString::number(colData)));

        }
    }
}

QVector<TrackMesure> Mytable::curSelectData()
{
    QVector<TrackMesure> result;

    if (!ui->tableWidget) return result;

    // 获取所有选中的行索引
    QList<QTableWidgetItem*> selectedItems = ui->tableWidget->selectedItems();
    QSet<int> selectedRows;

    // 收集所有被选择的行
    for (QTableWidgetItem *item : selectedItems)
    {
        selectedRows.insert(item->row());
    }

    // 遍历每个被选中的行
    for (int row : selectedRows)
    {
        TrackMesure data;

        // 确保行数据完整
        if (ui->tableWidget->columnCount() < 11) continue;

        // 0: 序号
        data._serialNumber = ui->tableWidget->item(row, 0)->text().toDouble();

        // 1: ID
        data._id = ui->tableWidget->item(row, 1)->text().toDouble();

        // 2: 里程
        data._mileage = ui->tableWidget->item(row, 2)->text().toDouble();

        // 3: 左轨高程
        data._lRailElevationn = ui->tableWidget->item(row, 3)->text().toDouble();

        // 4: 中线平面
        data._cLinePlann = ui->tableWidget->item(row, 4)->text().toDouble();

        // 5: 右轨高程
        data._rRailElevationn = ui->tableWidget->item(row, 5)->text().toDouble();

        // 6: 超高偏差
        data._supDeviationn = ui->tableWidget->item(row, 6)->text().toDouble();

        // 7: 轨距偏差
        data._guiDeviationn = ui->tableWidget->item(row, 7)->text().toDouble();

        // 8: 左轨调整值
        data._lGuiAdjustmentn = ui->tableWidget->item(row, 8)->text().toDouble();

        // 9: 中线调整值
        data._cLinedjustmentn = ui->tableWidget->item(row, 9)->text().toDouble();

        // 10: 右轨调整值
        data._rGuiAdjustmentn = ui->tableWidget->item(row, 10)->text().toDouble();

        result.append(data);
    }

    return result;
}

int Mytable::curSelectRow()
{
    QList<QTableWidgetItem*> selectedItems = ui->tableWidget->selectedItems();
    QVector<int> selectedRows;

    // 收集所有被选择的行
    for (QTableWidgetItem *item : selectedItems)
    {
        selectedRows.append(item->row());
    }

    if (selectedRows.size() > 0)
        return selectedRows[0];

    else
        return -1;
}


void Mytable::addData(TrackMesure data)
{
    int row = ui->tableWidget->rowCount();
    ui->tableWidget->insertRow(row);

    //ui->tableWidget->setItem(row, 0, new QTableWidgetItem(QString::number(data.SerialNumber)));
    ui->tableWidget->setItem(row, 0, new QTableWidgetItem(QString::number(data._id)));
    ui->tableWidget->setItem(row, 1, new QTableWidgetItem(QString::number(data._mileage,'g',8)));
    ui->tableWidget->setItem(row, 2, new QTableWidgetItem(QString::number(data._lRailElevationn)));
    ui->tableWidget->setItem(row, 3, new QTableWidgetItem(QString::number(data._cLinePlann)));
    ui->tableWidget->setItem(row, 4, new QTableWidgetItem(QString::number(data._rRailElevationn)));
    ui->tableWidget->setItem(row, 5, new QTableWidgetItem(QString::number(data._supDeviationn)));
    ui->tableWidget->setItem(row, 6, new QTableWidgetItem(QString::number(data._guiDeviationn)));
    ui->tableWidget->setItem(row, 7, new QTableWidgetItem(QString::number(data._lGuiAdjustmentn)));
    ui->tableWidget->setItem(row, 8, new QTableWidgetItem(QString::number(data._cLinedjustmentn)));
    ui->tableWidget->setItem(row, 9, new QTableWidgetItem(QString::number(data._rGuiAdjustmentn)));

//    ui->tableWidget->item(row, 0)->setFlags(Qt::ItemIsEnabled);
//    ui->tableWidget->item(row, 1)->setFlags(Qt::ItemIsEnabled);
//    ui->tableWidget->item(row, 2)->setFlags(Qt::ItemIsEnabled);
//    ui->tableWidget->item(row, 3)->setFlags(Qt::ItemIsEnabled);
//    ui->tableWidget->item(row, 4)->setFlags(Qt::ItemIsEnabled);
//    ui->tableWidget->item(row, 5)->setFlags(Qt::ItemIsEnabled);
//    ui->tableWidget->item(row, 6)->setFlags(Qt::ItemIsEnabled);
//    ui->tableWidget->item(row, 7)->setFlags(Qt::ItemIsEnabled);
//    ui->tableWidget->item(row, 8)->setFlags(Qt::ItemIsEnabled);
//    ui->tableWidget->item(row, 9)->setFlags(Qt::ItemIsEnabled);

    // 遍历所有单元格
    for(int row = 0; row < ui->tableWidget->rowCount(); ++row) {
        for(int col = 0; col < ui->tableWidget->columnCount(); ++col) {
            QTableWidgetItem *item = ui->tableWidget->item(row, col);
            if(!item) {
                item = new QTableWidgetItem(); // 确保单元格存在
                ui->tableWidget->setItem(row, col, item);

            }
            item->setTextAlignment(Qt::AlignCenter); // 居中对齐
        }
    }
}

void Mytable::deleteData(int row)
{
    if(row < ui->tableWidget->rowCount())
        ui->tableWidget->removeRow(row);
}

void Mytable::cleanData()
{
    int rows = ui->tableWidget->rowCount() -1;
    for(int i=rows;i>=0;i--)
        ui->tableWidget->removeRow(i);
}

void Mytable::modifyData(int row,TrackMesure data)
{
    if(row < ui->tableWidget->rowCount())
    {
        QTableWidgetItem *item = ui->tableWidget->item(row, 0);
        item->setText(QString::number(data._id));

        item = ui->tableWidget->item(row, 1);
        item->setText(QString::number(data._mileage));

        item = ui->tableWidget->item(row, 2);
        item->setText(QString::number(data._lRailElevationn));

        item = ui->tableWidget->item(row, 3);
        item->setText(QString::number(data._cLinePlann));

        item = ui->tableWidget->item(row, 4);
        item->setText(QString::number(data._rRailElevationn));

        item = ui->tableWidget->item(row, 5);
        item->setText(QString::number(data._supDeviationn));

        item = ui->tableWidget->item(row, 6);
        item->setText(QString::number(data._guiDeviationn));

        item = ui->tableWidget->item(row, 7);
        item->setText(QString::number(data._lGuiAdjustmentn));

        item = ui->tableWidget->item(row, 8);
        item->setText(QString::number(data._cLinedjustmentn));

        item = ui->tableWidget->item(row, 9);
        item->setText(QString::number(data._rGuiAdjustmentn));
    }
}

void Mytable::datachack()
{
//    ui->tableWidget->setItemDelegate(new CustomDelegate());
    int rowCount = ui->tableWidget->rowCount();
    if (rowCount != 0)
    {
        double num2 = ui->tableWidget->item(0, 2)->text().toDouble();
        double num3 = ui->tableWidget->item(0, 2)->text().toDouble();
        double num4 = ui->tableWidget->item(0, 3)->text().toDouble();
        double num5 = ui->tableWidget->item(0, 3)->text().toDouble();
        double num6 = ui->tableWidget->item(0, 4)->text().toDouble();
        double num7 = ui->tableWidget->item(0, 4)->text().toDouble();
        double num8 = 0.0;
        double num9 = 0.0;
        double num10 = 0.0;
        double num11 = 0.0;
        double num12 = 0.0;
        double num13 = 0.0;

        for (int j = 0; j < rowCount; j++)
        {
            double num16 = ui->tableWidget->item(j, 2)->text().toDouble();
            double num17 = ui->tableWidget->item(j, 3)->text().toDouble();
            double num18 = ui->tableWidget->item(j, 4)->text().toDouble();
            if (ui->tableWidget->item(j,1)->text().toDouble() == 0.0)
            {
                ui->tableWidget->item(j, 1)->setBackground(QBrush(Qt::red));
            }
            else
            {
                ui->tableWidget->item(j, 1)->setBackground(ui->tableWidget->item(j, 0)->background());
            }
            if (num16 > num2)
            {
                num2 = num16;
            }
            if (num16 < num3)
            {
                num3 = num16;
            }
            if (num17 > num4)
            {
                num4 = num17;
            }
            if (num17 < num5)
            {
                num5 = num17;
            }
            if (num18 > num6)
            {
                num6 = num18;
            }
            if (num18 < num7)
            {
                num7 = num18;
            }
        }
        for (int k = 0; k < rowCount; k++)
        {
            num8 += ui->tableWidget->item(k, 2)->text().toDouble();
            num10 += ui->tableWidget->item(k, 3)->text().toDouble();
            num12 += ui->tableWidget->item(k, 4)->text().toDouble();
        }
        num9 = ((num8 - num2) - num3) / ((double) (rowCount - 2));
        num11 = ((num10 - num4) - num5) / ((double) (rowCount - 2));
        num13 = ((num12 - num6) - num7) / ((double) (rowCount - 2));
        for (int m = 0; m < rowCount; m++)
        {
            double num21 = ui->tableWidget->item(m, 2)->text().toDouble();
            double num22 = ui->tableWidget->item(m, 3)->text().toDouble();
            double num23 = ui->tableWidget->item(m, 4)->text().toDouble();
            double num24 = m_configPara.maxhet;//高程行程
            double num25 = m_configPara.maxset;//中线行程
            if (qAbs((double) (num21 - num9)) > m_configPara.maxh1)//高程L最大偏差
            {
                ui->tableWidget->item(m, 3)->setBackground(QBrush(Qt::red));
            }
            else
            {
                ui->tableWidget->item(m, 3)->setBackground(ui->tableWidget->item(m, 0)->background());
            }

            if (qAbs((double) (num22 - num11)) > m_configPara.maxh2)
            {
                ui->tableWidget->item(m, 5)->setBackground(QBrush(Qt::red));
            }
            else
            {
                ui->tableWidget->item(m, 5)->setBackground(ui->tableWidget->item(m, 0)->background());
            }
            if (qAbs((double) (num23 - num13)) > m_configPara.maxs)
            {
                ui->tableWidget->item(m, 4)->setBackground(QBrush(Qt::red));
            }
            else
            {
                ui->tableWidget->item(m, 4)->setBackground(ui->tableWidget->item(m, 0)->background());
            }
            if (qAbs(num21) > num24)
            {
                ui->tableWidget->item(m, 3)->setBackground(QBrush(Qt::yellow));
            }
            else
            {
                ui->tableWidget->item(m, 3)->setBackground(ui->tableWidget->item(m, 0)->background());
            }
            if (qAbs(num22) > num24)
            {
                ui->tableWidget->item(m, 5)->setBackground(QBrush(Qt::yellow));
            }
            else
            {
                ui->tableWidget->item(m, 5)->setBackground(ui->tableWidget->item(m, 0)->background());
            }
            if (qAbs(num23) > num25)
            {
                ui->tableWidget->item(m, 4)->setBackground(QBrush(Qt::yellow));
            }
            else
            {
                ui->tableWidget->item(m, 4)->setBackground(ui->tableWidget->item(m, 0)->background());
            }
        }
    }
    ui->tableWidget->update();
}

void Mytable::saveTableToCSV()
{
    ExcelFunc excelFunc;
    excelFunc.saveTableToCSV(ui->tableWidget);
}
QList<TrackMesure> Mytable::loadCSVToTable()
{
    ExcelFunc excelFunc;
    return excelFunc.loadCSVToTable(ui->tableWidget, -1);
}

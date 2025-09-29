#include "excelfunc.h"
#include <QFile>
#include <QTextStream>
#include <QFileDialog>
#include <QDebug>
#include <QTextCodec>

ExcelFunc::ExcelFunc()
{

}


void ExcelFunc::saveTableToCSV(QTableWidget *table)
{
#ifdef ON_PAD
    QString fileName = QFileDialog::getSaveFileName(nullptr, "Save CSV", "ZT", "CSV Files (*.csv)");
#else
    QString fileName = QFileDialog::getSaveFileName(nullptr, "Save CSV", "E:/WGZ", "CSV Files (*.csv)");

#endif
    if (fileName.isEmpty())  return;

    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly  | QIODevice::Text))
        return;

    QTextStream out(&file);

    int rowCount = table->rowCount();
    int colCount = table->columnCount();

    // 写入表头
    for (int col = 0; col < colCount; ++col) {
        QTableWidgetItem *header = table->horizontalHeaderItem(col);
        if (header) {
            out << "\"" << header->text() << "\"";
        }
        if (col < colCount - 1)
            out << ",";
    }
    out << "\n";

    // 写入数据
    for (int row = 0; row < rowCount; ++row) {
        for (int col = 0; col < colCount; ++col) {
            QTableWidgetItem *item = table->item(row, col);
            if (item) {
                out << "\"" << item->text() << "\"";
            }
            if (col < colCount - 1)
                out << ",";
        }
        out << "\n";
    }

    file.close();
}

void ExcelFunc::loadCSVToTable(QTableWidget *table)
{
#ifdef ON_PAD
    QString fileName = QFileDialog::getOpenFileName(nullptr, "Save CSV", "ZT", "CSV Files (*.csv)");
#else
    QString fileName = QFileDialog::getOpenFileName(nullptr, "Save CSV", "E:/WGZ", "CSV Files (*.csv)");
#endif
    if (fileName.isEmpty())  return;

    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly  | QIODevice::Text))
        return;
    QTextStream in(&file);

    if(in.seek(12))

    // 读取表头,由于表头回车自动换行，因此占用12行
    for(int i = 0;i<12;i++)
    {
        if (!in.atEnd())
        {
            in.readLine();
        }
    }

    // 读取数据行
    int row = 0;
    while (!in.atEnd())  {
        QString line = in.readLine();
        QStringList fields = line.split(',');

        table->insertRow(row);
        for (int col = 0; col < fields.size();  ++col) {
            QString value = fields.at(col);//含双引号
            QTableWidgetItem *item = new QTableWidgetItem(value.mid(1,value.length()-2));
            table->setItem(row, col, item);
        }
        row++;
    }

    file.close();
}

QList<TrackMesure> ExcelFunc::loadCSVToTable(QTableWidget *table,
                                             int headerLines /*=12*/,
                                             const QString &filePath /*=QString()*/)
{
    // 1) 选择文件
    QString fileName = filePath;
    if (fileName.isEmpty()) {
#ifdef ON_PAD
        fileName = QFileDialog::getOpenFileName(nullptr, "Open CSV", "ZT", "CSV Files (*.csv)");
#else
        fileName = QFileDialog::getOpenFileName(nullptr, "Open CSV", "E:/WGZ", "CSV Files (*.csv)");
#endif
    }
    QList<TrackMesure> result;
    if (fileName.isEmpty()) return result;

    // 2) 打开并读入所有行（便于需要时自动判定表头行数）
    QFile f(fileName);
    if (!f.open(QIODevice::ReadOnly | QIODevice::Text)) { qWarning() << "open fail" << fileName; return result; }
    QTextStream in(&f);
    in.setCodec(QTextCodec::codecForName("UTF-8"));
    in.setLocale(QLocale::c()); // 固定小数点 '.'

    QStringList lines;
    while (!in.atEnd()) lines << in.readLine();
    f.close();
    if (lines.isEmpty()) return result;

    // 3) 自动判定表头：当 headerLines == -1 时，找第一行首列能转 double 的行
    if (headerLines == -1) {
        int idx = 0;
        while (idx < lines.size() && lines.at(idx).trimmed().isEmpty())
            ++idx;
        for (; idx < lines.size(); ++idx)
        {
            const QString first = lines.at(idx);
            QString c0 = first.section(',', 0, 0).trimmed();
            if (c0.size() >= 2 && c0.startsWith('"') && c0.endsWith('"'))
                c0 = c0.mid(1, c0.size()-2).trimmed();
            bool ok=false;
            QLocale::c().toDouble(c0, &ok);
            if (ok)
            {
                headerLines = idx;
                break;
            }
        }
        if (headerLines == -1)
            headerLines = 0; // 全都不是数字，就当没表头
    }

    // 4) 解析数据到 QList，同时准备填表
    const int COLS = 10;
    for (int i = headerLines; i < lines.size(); ++i)
    {
        const QString &line = lines.at(i);
        if (line.trimmed().isEmpty()) continue;

        QStringList cols = line.split(',', QString::KeepEmptyParts);
        if (cols.size() < COLS) {
            qWarning() << "bad row (col<11) at" << (i+1) << line; continue;
        }

        double v[COLS] = {0};
        bool okAll = true;
        for (int c = 0; c < COLS; ++c)
        {
            QString s = cols.at(c).trimmed();
            if (s.size() >= 2 && s.startsWith('"') && s.endsWith('"')) s = s.mid(1, s.size()-2).trimmed();
            bool ok=false; v[c] = QLocale::c().toDouble(s, &ok);
            if (!ok)
            {
                okAll = false;
                break;
            }
        }

        if (!okAll)
        {
            qWarning() << "parse fail at line" << (i+1) << line;
            continue;
        }

        TrackMesure m;
        m._serialNumber    = 1;
        m._id              = v[0];
        m._mileage         = v[1];
        m._lRailElevationn = v[2];
        m._cLinePlann      = v[3];
        m._rRailElevationn = v[4];
        m._supDeviationn   = v[5];
        m._guiDeviationn   = v[6];
        m._lGuiAdjustmentn = v[7];
        m._cLinedjustmentn = v[8];
        m._rGuiAdjustmentn = v[9];
        result << m;
    }

    // 5) 写入 QTableWidget（可选：table==NULL 时仅返回数据）
    if (table) {
        for (int i = 0; i < table->rowCount(); i++)
        table->removeRow(i);
//        table->setColumnCount(11);
//        QStringList headers;
//        headers << "serialNumber" << "id" << "mileage"
//                << "lRailElevation" << "cLinePlan" << "rRailElevation"
//                << "supDeviation" << "gaugeDeviation"
//                << "lGaugeAdjustment" << "cLineAdjustment" << "rGaugeAdjustment";
//        table->setHorizontalHeaderLabels(headers);
        table->setRowCount(result.size());

        for (int r = 0; r < result.size(); ++r) {
            const TrackMesure &m = result.at(r);
            const double vs[10] =
            {
//                m._serialNumber,
                m._id,
                m._mileage,
                m._lRailElevationn,
                m._cLinePlann,
                m._rRailElevationn,
                m._supDeviationn,
                m._guiDeviationn,
                m._lGuiAdjustmentn,
                m._cLinedjustmentn,
                m._rGuiAdjustmentn
            };
            for (int c = 0; c < 10; ++c)
            {
                if (c != 0)
                {
                    qDebug () << QLocale::c().toString(vs[c]);

                    table->setItem(r, c, new QTableWidgetItem(QLocale::c().toString(vs[c], 'f', 6)));
                }
                else
                {
                    table->setItem(r, c, new QTableWidgetItem(QLocale::c().toString(vs[c], 'd', 0)));
                }
            }
        }
    }

    return result;
}

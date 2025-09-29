#ifndef PRECISIONDATAPROCESSING_H
#define PRECISIONDATAPROCESSING_H
#include <QFile>
#include <QTextStream>
#include <QVector>
#include <QDebug>

class PrecisionDataProcessing
{
public:
    PrecisionDataProcessing();
    // 解析单行数据
    QVector<double> parseDataLine(const QString& line);

    // 从文件读取并解析所有数据
    QVector<QVector<double>> parseDataFile(const QString& filePath);
};

#endif // PRECISIONDATAPROCESSING_H

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
    // ������������
    QVector<double> parseDataLine(const QString& line);

    // ���ļ���ȡ��������������
    QVector<QVector<double>> parseDataFile(const QString& filePath);
};

#endif // PRECISIONDATAPROCESSING_H

#include "PrecisionDataProcessing.h"

PrecisionDataProcessing::PrecisionDataProcessing()
{

}

QVector<double> PrecisionDataProcessing::parseDataLine(const QString &line)
{
    QVector<double> result;
    QStringList parts = line.split('\t', QString::SkipEmptyParts);

    for (const QString& part : parts)
    {
        // 去除双引号并检查是否为空
        QString valueStr = part.length() >= 2 ? part.mid(1, part.length() - 2) : "";

        if (valueStr.isEmpty())
        {
            result.append(0.0);
        }
        else
        {
            bool ok;
            double value = valueStr.toDouble(&ok);
            result.append(ok ? value : 0.0);
        }
    }

    return result;
}

QVector<QVector<double>> PrecisionDataProcessing::parseDataFile(const QString &filePath)
{
    QVector<QVector<double>> allData;
    QFile file(filePath);

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qWarning() << "无法打开文件:" << filePath;
        return allData;
    }

    QTextStream in(&file);
    while (!in.atEnd())
    {
        QString line = in.readLine().trimmed();
        if (!line.isEmpty())
        {
            QVector<double> lineData = parseDataLine(line);
            if (!lineData.isEmpty())
            {
                allData.append(lineData);
            }
        }
    }

    file.close();
    return allData;
}

//// 使用示例
//void testFileParser() {
//    QString filePath = "data.txt"; // 替换为你的文件路径
//    QVector<QVector<double>> parsedData = parseDataFile(filePath);

//    qDebug() << "共解析" << parsedData.size() << "行数据:";
//    for (int i = 0; i < parsedData.size(); ++i) {
//        qDebug() << "第" << i+1 << "行:";
//        for (double val : parsedData[i]) {
//            qDebug() << val;
//        }
//        qDebug() << "-----";
//    }
//}



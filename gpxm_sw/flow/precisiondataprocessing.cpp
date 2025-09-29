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
        // ȥ��˫���Ų�����Ƿ�Ϊ��
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
        qWarning() << "�޷����ļ�:" << filePath;
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

//// ʹ��ʾ��
//void testFileParser() {
//    QString filePath = "data.txt"; // �滻Ϊ����ļ�·��
//    QVector<QVector<double>> parsedData = parseDataFile(filePath);

//    qDebug() << "������" << parsedData.size() << "������:";
//    for (int i = 0; i < parsedData.size(); ++i) {
//        qDebug() << "��" << i+1 << "��:";
//        for (double val : parsedData[i]) {
//            qDebug() << val;
//        }
//        qDebug() << "-----";
//    }
//}



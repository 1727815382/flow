#ifndef EXCELFUNC_H
#define EXCELFUNC_H


#include <QObject>
#include <QString>
#include <QTableWidget>

#include "aixsim.h"

class ExcelFunc
{
public:
    ExcelFunc();
    void saveTableToCSV(QTableWidget *table);
    void loadCSVToTable(QTableWidget *table);
    QList<TrackMesure> loadCSVToTable(QTableWidget *table,
                                      int headerLines =12,
                                      const QString &filePath=QString());

};

#endif // EXCELFUNC_H

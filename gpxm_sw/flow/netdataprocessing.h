#ifndef NETDATAPROCESSING_H
#define NETDATAPROCESSING_H

#include <QObject>

#include "aixsim.h"


class HttpServer;

class NetDataProcessing : public QObject
{
    Q_OBJECT

public:

    NetDataProcessing();

signals:

    void signMesureUpdate (TrackMesure);

public slots:

    void slotReceiveMessage (const QString&);

private:

    HttpServer* _server;
};

#endif // NETDATAPROCESSING_H

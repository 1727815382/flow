#include "netdataprocessing.h"
#include "network/httpserver.h"

NetDataProcessing::NetDataProcessing()
{
    _server = new HttpServer();

    connect(_server, SIGNAL(signMessageSent(QString)),
            this, SLOT(slotReceiveMessage(QString)));

    if (!_server->listen(QHostAddress::Any,  12345))
    {
        qDebug() << "Server could not start!";
    }
    else
    {
        qDebug() << "Server started!";
    }
}

void NetDataProcessing::slotReceiveMessage(const QString &message)
{
    TrackMesure data;
    QList<QString> datalist;
    if(message.contains("数据采集结果"))
    {
        QRegExp rx("\\d+\\.\\d+|(\\d+)");
        int pos = 0;
        while ((pos = rx.indexIn(message, pos)) != -1)
        {
            qDebug() << rx.cap(0);
            pos += rx.matchedLength();
            datalist.append(rx.cap(0));
        }
    }

    if(datalist.count() >=8)
    {
        data._serialNumber = 0;                // 序号
        data._id = datalist.at(0).toDouble();                //id
        data._mileage = datalist.at(1).toDouble()/100.0;           //里程
        data._lRailElevationn = datalist.at(2).toDouble()/100.0;   //左轨高程
        data._cLinePlann = datalist.at(3).toDouble()/100.0;        //中线平面
        data._rRailElevationn = datalist.at(4).toDouble()/100.0;   //右轨高程
        data._supDeviationn = datalist.at(5).toDouble()/100.0;     //超高偏差
        data._guiDeviationn = datalist.at(6).toDouble()/100.0;     //轨距偏差
        data._lGuiAdjustmentn = 0;                           //左轨调整值
        data._cLinedjustmentn = 0;                           //中线调整值
        data._rGuiAdjustmentn = 0;                           //右轨调整值

        //发送数据到界面
        emit signMesureUpdate (data);
    }

}

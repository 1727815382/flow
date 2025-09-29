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
    if(message.contains("���ݲɼ����"))
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
        data._serialNumber = 0;                // ���
        data._id = datalist.at(0).toDouble();                //id
        data._mileage = datalist.at(1).toDouble()/100.0;           //���
        data._lRailElevationn = datalist.at(2).toDouble()/100.0;   //���߳�
        data._cLinePlann = datalist.at(3).toDouble()/100.0;        //����ƽ��
        data._rRailElevationn = datalist.at(4).toDouble()/100.0;   //�ҹ�߳�
        data._supDeviationn = datalist.at(5).toDouble()/100.0;     //����ƫ��
        data._guiDeviationn = datalist.at(6).toDouble()/100.0;     //���ƫ��
        data._lGuiAdjustmentn = 0;                           //������ֵ
        data._cLinedjustmentn = 0;                           //���ߵ���ֵ
        data._rGuiAdjustmentn = 0;                           //�ҹ����ֵ

        //�������ݵ�����
        emit signMesureUpdate (data);
    }

}

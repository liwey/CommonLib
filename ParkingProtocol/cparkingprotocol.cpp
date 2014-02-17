#include "cparkingprotocol.h"
#include "cparkingprotocolprivate.h"

CParkingProtocol::CParkingProtocol(QObject *parent) :
    QObject(parent)
{
    pp = new CParkingProtocolPrivate();

    connect(pp, &CParkingProtocolPrivate::errorOccur, this, &CParkingProtocol::errorOccur);
    connect(pp, &CParkingProtocolPrivate::respondData, this, &CParkingProtocol::respondData);
}

CParkingProtocol::~CParkingProtocol()
{
    delete pp;
}

void CParkingProtocol::setDevices(const QStringList &list)
{
    pp->setDevices(list);
}

void CParkingProtocol::addDevices(const QStringList &list)
{
    pp->addDevices(list);
}

void CParkingProtocol::addDevice(const QString &sn)
{
    addDevices(QStringList(sn));
}

void CParkingProtocol::removeDevices(const QStringList &list)
{
    pp->removeDevices(list);
}

void CParkingProtocol::removeDevice(const QString &sn)
{
    removeDevices(QStringList(sn));
}

void CParkingProtocol::openBarrier(const QString &sn, int retry, int timeout)
{
    ParkingData pd;
    pd.SN = sn;
    pd.FUN = char(0x11);
    pd.data = QByteArray(1, char(1));
    pd.retry = retry;
    pd.timeout = timeout;
    pd.respond = true;
    pp->sendData(pd);
}

void CParkingProtocol::closeBarrier(const QString &sn, int retry, int timeout)
{
    ParkingData pd;
    pd.SN = sn;
    pd.FUN = char(0x11);
    pd.data = QByteArray(1, char(0));
    pd.retry = retry;
    pd.timeout = timeout;
    pd.respond = true;
    pp->sendData(pd);
}

void CParkingProtocol::transparent(const QString &sn, const QByteArray &data, int retry, int timeout)
{
    ParkingData pd;
    pd.SN = sn;
    pd.FUN = char(0x35);
    pd.data = data;
    pd.retry = retry;
    pd.timeout = timeout;
    pd.respond = true;
    pp->sendData(pd);
}

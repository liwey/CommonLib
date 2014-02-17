#include "cserialpolling.h"
#include "cpollingthread.h"
#include "cserialportprivate.h"

CSerialPolling::CSerialPolling(QObject *parent) :
    QObject(parent)
{
    sp = new CSerialPortPrivate();
    connect(sp, &CSerialPortPrivate::dataReady, this, &CSerialPolling::dataReady);
    connect(sp, &CSerialPortPrivate::respondTimeout, this, &CSerialPolling::respondTimeout);
    connect(sp, &CSerialPortPrivate::dataRequest, this, &CSerialPolling::dataRequest);

    pt = new CPollingThread(sp);
    pt->start();
}

bool CSerialPolling::open(const QString &portName)
{
    return sp->open(portName);
}

void CSerialPolling::close()
{
    sp->close();
}

void CSerialPolling::setCharTimeout(qint64 us)
{
    pt->setCharTimeout(us);
}

void CSerialPolling::write(const QByteArray &data, int timeout, int retry)
{
    pt->write(data, timeout, retry);
}

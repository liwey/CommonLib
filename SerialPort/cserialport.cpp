#include "cserialport.h"
#include "cserialportprivate.h"
#include "creadthread.h"
#include "cwritethread.h"

CSerialPort::CSerialPort(QObject *parent) :
    QObject(parent)
{
    sp = new CSerialPortPrivate();
    connect(sp, &CSerialPortPrivate::dataReady, this, &CSerialPort::dataReady);

    rt = new CReadThread(sp);
    wt = new CWriteThread(sp);

    wt->start();
    rt->start();
}

bool CSerialPort::open(const QString &portName)
{
    return sp->open(portName);
}

void CSerialPort::setAutoConnect()
{
    sp->setAutoConnect();
}

void CSerialPort::close()
{
    sp->close();
}

void CSerialPort::write(const QByteArray &data)
{
    wt->write(data);
}

void CSerialPort::setCharTimeout(qint64 us)
{
    rt->setCharTimeout(us);
}

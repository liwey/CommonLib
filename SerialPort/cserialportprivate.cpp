#include "cserialportprivate.h"
#include "qextserialenumerator.h"
#include <QDebug>

CSerialPortPrivate::CSerialPortPrivate(QObject *parent)
    : QObject(parent)
{
    autoConn = false;
    setStatus(false);
    ::SecureZeroMemory(&ovRead, sizeof(OVERLAPPED));
    ::SecureZeroMemory(&ovWrite, sizeof(OVERLAPPED));

    ovRead.hEvent = ::CreateEvent(NULL,true,false,NULL);
    ovWrite.hEvent = ::CreateEvent(NULL,true,false,NULL);
}

void CSerialPortPrivate::setAutoConnect()
{
    if(autoConn)
        return;

    autoConn = true;

    if (getStatus()){
        connect(QextSerialEnumerator::instance(), &QextSerialEnumerator::deviceDiscovered,
                this, &CSerialPortPrivate::portFind);
    }
}

void CSerialPortPrivate::portFind(const QString &portName)
{
    if (portName == port)
        open_(portName);
}

void CSerialPortPrivate::portRemove(const QString &portName)
{
    if (portName == port)
        close_();
}

bool CSerialPortPrivate::open_(const QString &portName)
{
    close();

    //qDebug() << "open";

    QString port = portName;
    if ("//./" != port.left(4))
        port.insert(0, "//./");

    //设置并打开端口
    hCom = ::CreateFile((wchar_t *) port.utf16(), GENERIC_READ
                      | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_FLAG_OVERLAPPED, NULL);

    if (hCom != INVALID_HANDLE_VALUE)
    {
        DCB wdcb;
        ::GetCommState(hCom, &wdcb);
        wdcb.BaudRate = 9600;
        wdcb.ByteSize = 8;
        wdcb.Parity = NOPARITY;
        wdcb.StopBits =ONESTOPBIT;
        wdcb.fRtsControl = RTS_CONTROL_DISABLE;

        ::SetCommState(hCom, &wdcb);
        ::PurgeComm(hCom, PURGE_RXCLEAR | PURGE_RXABORT | PURGE_TXCLEAR | PURGE_TXABORT);


        //异步要设置超时时间
        COMMTIMEOUTS m_timeout;
        m_timeout.ReadIntervalTimeout = MAXDWORD;
        m_timeout.ReadTotalTimeoutConstant = 0;
        m_timeout.ReadTotalTimeoutMultiplier = 0;
        m_timeout.WriteTotalTimeoutConstant = 0;
        m_timeout.WriteTotalTimeoutMultiplier = 0;
        ::SetCommTimeouts(hCom,&m_timeout);

        ::ResetEvent(ovRead.hEvent);
        ::ResetEvent(ovWrite.hEvent);

        setStatus(true);
    }

    return getStatus();
}

bool CSerialPortPrivate::open(const QString &portName)
{
    if(open_(portName)) {
        port = portName;
        if (autoConn){
            connect(QextSerialEnumerator::instance(), &QextSerialEnumerator::deviceDiscovered,
                    this, &CSerialPortPrivate::portFind);
        }

        connect(QextSerialEnumerator::instance(), &QextSerialEnumerator::deviceRemoved,
                this, &CSerialPortPrivate::portRemove);
        qDebug() << "open" << portName << "successed";
        return true;
    }

    qDebug() << "open" << portName << "failed";
    port.clear();
    return false;
}

void CSerialPortPrivate::close()
{
    close_();
    QextSerialEnumerator::instance()->disconnect(this);
}

void CSerialPortPrivate::close_()
{
    //qDebug() << "close";
    if (getStatus()){
        setStatus(false);

        //防止等待
        ::SetEvent(ovRead.hEvent);
        ::SetEvent(ovWrite.hEvent);

        ::CloseHandle(hCom);
    }
}

void CSerialPortPrivate::setStatus(bool valid)
{
    rwLock.lockForWrite();
    this->valid = valid;
    rwLock.unlock();
}

bool CSerialPortPrivate::getStatus()
{
    bool status;

    rwLock.lockForRead();
    status = valid;
    rwLock.unlock();

    return status;
}

bool CSerialPortPrivate::write(LPCVOID lpBuffer, DWORD nNumberOfBytesToWrite)
{
    if (!getStatus()) {
        return false;
    }

    DWORD wCount = 0;
    if (!WriteFile(hCom, lpBuffer, nNumberOfBytesToWrite, &wCount, &ovWrite))
    {
        if (GetLastError() == ERROR_IO_PENDING)
        {
            GetOverlappedResult(hCom, &ovWrite, &wCount, true);
        }
        else
        {
            return false;
        }
    }

    return true;
}

DWORD CSerialPortPrivate::read(LPVOID lpBuffer, DWORD nBufferSize)
{
    DWORD wCount = 0;
    DWORD dwError = 0;
    COMSTAT Status;

    bool success = getStatus();
    if(success)
        success = ClearCommError(hCom, &dwError, &Status);

    if (success && (Status.cbInQue > 0))
    {
        if(Status.cbInQue < nBufferSize)
        {
            nBufferSize = Status.cbInQue;
        }
        ReadFile(hCom, lpBuffer, nBufferSize, &wCount, &ovRead);
    }

    return wCount;
}

void CSerialPortPrivate::cleanRx()
{
    if(getStatus())
        PurgeComm(hCom, PURGE_RXCLEAR | PURGE_RXABORT);
}

void CSerialPortPrivate::cleanTx()
{
    if(getStatus())
        PurgeComm(hCom, PURGE_TXCLEAR | PURGE_TXABORT);
}

void CSerialPortPrivate::cleanRxTx()
{
    if(getStatus())
        PurgeComm(hCom, PURGE_RXCLEAR | PURGE_RXABORT | PURGE_TXCLEAR | PURGE_TXABORT);
}

void CSerialPortPrivate::readReady(const QByteArray &data)
{
    emit dataReady(data);
}

void CSerialPortPrivate::requesting()
{
    emit dataRequest();
}

void CSerialPortPrivate::packTimeout()
{
    emit respondTimeout();
}

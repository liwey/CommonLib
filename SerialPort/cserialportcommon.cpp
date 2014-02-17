#include "cserialportcommon.h"

CSerialPortCommon::CSerialPortCommon()
{
    comopen = false;

    memset(&ovRead,0,sizeof(OVERLAPPED));
    memset(&ovWrite,0,sizeof(OVERLAPPED));

    ovRead.hEvent = CreateEvent(NULL,true,false,NULL);
    ovWrite.hEvent = CreateEvent(NULL,true,false,NULL);
}

bool CSerialPortCommon::write(LPCVOID lpBuffer, DWORD nNumberOfBytesToWrite)
{
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

DWORD CSerialPortCommon::read(LPVOID lpBuffer, DWORD nBufferSize)
{
    DWORD wCount = 0;
    DWORD dwError = 0;
    COMSTAT Status;

    bool success = ClearCommError(hCom, &dwError, &Status);
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

void CSerialPortCommon::cleanRx()
{
    PurgeComm(hCom, PURGE_RXCLEAR);
}

void CSerialPortCommon::cleanTx()
{
    PurgeComm(hCom, PURGE_TXCLEAR);
}

void CSerialPortCommon::cleanRxTx()
{
    PurgeComm(hCom, PURGE_RXCLEAR | PURGE_TXCLEAR);
}

bool CSerialPortCommon::open(const QString &port, int baudRate)
{
    close();

    //设置并打开端口
    hCom = CreateFile((LPCWSTR)port.utf16(), GENERIC_READ
        | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_FLAG_OVERLAPPED, NULL);

    if (hCom != (HANDLE)-1)
    {
        DCB wdcb;
        GetCommState(hCom, &wdcb);
        wdcb.BaudRate = baudRate;
        wdcb.ByteSize = 8;
        wdcb.Parity = NOPARITY;
        wdcb.StopBits =ONESTOPBIT;
        wdcb.fRtsControl = RTS_CONTROL_DISABLE;

        SetCommState(hCom, &wdcb);
        PurgeComm(hCom, PURGE_TXCLEAR|PURGE_RXCLEAR);


        //异步要设置超时时间
        COMMTIMEOUTS m_timeout;
        m_timeout.ReadIntervalTimeout = MAXDWORD;
        m_timeout.ReadTotalTimeoutConstant = 0;
        m_timeout.ReadTotalTimeoutMultiplier = 0;
        m_timeout.WriteTotalTimeoutConstant = 0;
        m_timeout.WriteTotalTimeoutMultiplier = 0;
        SetCommTimeouts(hCom,&m_timeout);

        ResetEvent(ovRead.hEvent);
        ResetEvent(ovWrite.hEvent);

        comopen = true;
    }

    return comopen;
}

void CSerialPortCommon::close()
{
    if (comopen == true)
    {
        comopen = false;

        //防止等待
        SetEvent(ovRead.hEvent);
        SetEvent(ovWrite.hEvent);

        CloseHandle(hCom);
    }
}

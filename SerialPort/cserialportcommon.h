#ifndef CSERIALPORTCOMMON_H
#define CSERIALPORTCOMMON_H

#include <Windows.h>
#include <QString>

class CSerialPortCommon
{
public:
    CSerialPortCommon();
    bool isOpen() { return comopen; }
    void cleanRx();
    bool write(LPCVOID lpBuffer, DWORD nNumberOfBytesToWrite);
    DWORD read(LPVOID lpBuffer, DWORD nBufferSize);

protected:
    virtual bool open(const QString &port, int baudRate=9600);
    virtual void close();
    void cleanRxTx();
    void cleanTx();

private:
    OVERLAPPED ovRead;
    OVERLAPPED ovWrite;
    HANDLE hCom;
    bool comopen;
};

#endif // CSERIALPORTCOMMON_H

#ifndef CSERIALPORTPRIVATE_H
#define CSERIALPORTPRIVATE_H
#include <QReadWriteLock>
#include <Windows.h>
#include <QObject>

class CSerialPortPrivate : public QObject
{
    Q_OBJECT
public:
    CSerialPortPrivate(QObject *parent = 0);
    bool open(const QString &portName);
    void close();

    void setAutoConnect();

    void setStatus(bool valid);
    bool getStatus();

    bool write(LPCVOID lpBuffer, DWORD nNumberOfBytesToWrite);
    DWORD read(LPVOID lpBuffer, DWORD nBufferSize);

    void cleanRx();
    void cleanTx();
    void cleanRxTx();

    void readReady(const QByteArray &data);
    void packTimeout();
    void requesting();

signals:
    void dataReady(const QByteArray &data);
    void dataRequest();
    void respondTimeout();

private:
    bool open_(const QString &portName);
    void close_();

private slots:
    void portFind(const QString &portName);
    void portRemove(const QString &portName);

public:
    static const int STATE_WRITING = 0;
    static const int STATE_POLLING = 1;
    static const int STATE_READING = 2;
    static const int STATE_NOTIFY_DATA = 3;
    static const int STATE_NOTIFY_TIMEOUT = 4;
    static const int STATE_REQUESTING = 5;

private:
    bool autoConn;

    bool valid;
    QReadWriteLock rwLock;

    OVERLAPPED ovRead;
    OVERLAPPED ovWrite;
    HANDLE hCom;
    QString port;
};

#endif // CSERIALPORTPRIVATE_H

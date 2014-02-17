#ifndef CPOLLINGTHREAD_H
#define CPOLLINGTHREAD_H

#include <QThread>
#include <QQueue>
#include "cticktimer.h"

class CSerialPortPrivate;
class CPollingThread;
class CPollingThread : public QThread
{
    Q_OBJECT
public:
    explicit CPollingThread(CSerialPortPrivate *sp, QObject *parent = 0);
    void setCharTimeout(qint64 us);
    void setPackTimeout(qint64 ms);
    void write(const QByteArray &data, int timeout = 0, int retry = 0);

signals:

public slots:

protected:
    void run();


private:
    bool checkStop();
    void stop();
    void doPoll();
    void requesting();
    void writing(); // 0
    void polling(); // 1
    void reading(); // 2
    void notifying(); // 3
    void notifyingTimeout(); // 4

private:
    CSerialPortPrivate *sp;
    QMutex mutex;
    bool stopFlag;
    int state;

    int read_len;
    char read_buf[128];
    CTickTimer timer;
    qint64 charTimeout;
    qint64 packTimeout;

    int data_timeout;
    int data_retry;
    QByteArray data;
};

#endif // CPOLLINGTHREAD_H

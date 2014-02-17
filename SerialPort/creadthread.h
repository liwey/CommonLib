#ifndef CREADTHREAD_H
#define CREADTHREAD_H

#include <QThread>
#include "cticktimer.h"

class CSerialPortPrivate;
class CReadThread : public QThread
{
    Q_OBJECT
public:
    explicit CReadThread(CSerialPortPrivate *sp, QObject *parent = 0);
    void setCharTimeout(qint64 us);

signals:

public slots:

protected:
    void run();


private:
    bool checkStop();
    void stop();
    void readData();
    void polling();
    void reading();
    void notifying();

private:
    CSerialPortPrivate *sp;
    QMutex mutex;
    bool stopFlag;
    QByteArray data;
    int state;

    int read_len;
    char read_buf[128];
    CTickTimer timer;
    qint64 charTimeout;
};

#endif // CREADTHREAD_H

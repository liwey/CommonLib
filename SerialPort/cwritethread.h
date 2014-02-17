#ifndef CWRITETHREAD_H
#define CWRITETHREAD_H

#include <QThread>
#include <QQueue>
#include <QMutex>

class CSerialPortPrivate;
class CWriteThread : public QThread
{
    Q_OBJECT
public:
    explicit CWriteThread(CSerialPortPrivate *sp, QObject *parent = 0);
    void write(const QByteArray &data);

signals:

public slots:

protected:
    void run();

private:
    bool checkStop();
    void stop();

private:
    CSerialPortPrivate *sp;
    QQueue<QByteArray> dataQueue;
    QMutex mutex;
    bool stopFlag;
};

#endif // CWRITETHREAD_H

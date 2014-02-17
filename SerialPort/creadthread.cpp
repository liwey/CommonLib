#include "creadthread.h"
#include "cserialportprivate.h"

CReadThread::CReadThread(CSerialPortPrivate *sp, QObject *parent) :
    QThread(parent)
{
    stopFlag = false;
    this->sp = sp;
    charTimeout = 18000;
}

void CReadThread::setCharTimeout(qint64 us)
{
    if (!sp->getStatus())
        charTimeout = us;
}

void CReadThread::run()
{
    state = CSerialPortPrivate::STATE_POLLING;
    forever {
        if (checkStop()) {
            break;
        } else if (!sp->getStatus()){
            msleep(30);
        } else {
            readData();
        }
    }
}

void CReadThread::readData()
{
    switch (state) {
    case CSerialPortPrivate::STATE_POLLING:
        polling();
        break;
    case CSerialPortPrivate::STATE_READING:
        reading();
        break;
    case CSerialPortPrivate::STATE_NOTIFY_DATA:
        notifying();
        break;
    default:
        break;
    }

    usleep(100);
}

void CReadThread::polling()
{
    read_len = sp->read(read_buf, 128);
    if (read_len > 0){
        data = QByteArray(read_buf, read_len);
        state = CSerialPortPrivate::STATE_READING;
        timer.Start();
    }
}

void CReadThread::reading()
{
    read_len = sp->read(read_buf, 128);
    if (read_len > 0){
        data.append(QByteArray(read_buf, read_len));
        timer.Start();
    } else if (timer.TimeoutMicro(charTimeout)) {
        state = CSerialPortPrivate::STATE_NOTIFY_DATA;
    }
}

void CReadThread::notifying()
{
    sp->readReady(data);
    state = CSerialPortPrivate::STATE_POLLING;
}

bool CReadThread::checkStop()
{
    mutex.lock();
    bool ret = stopFlag;
    mutex.unlock();

    return ret;
}

void CReadThread::stop()
{
    if(!checkStop()) {
        mutex.lock();
        stopFlag = true;
        mutex.unlock();
    }
}

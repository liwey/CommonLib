#include "cwritethread.h"
#include "cserialportprivate.h"

CWriteThread::CWriteThread(CSerialPortPrivate *sp, QObject *parent) :
    QThread(parent)
{
    this->sp = sp;
    stopFlag = false;
}

void CWriteThread::write(const QByteArray &data)
{
    mutex.lock();
    dataQueue.enqueue(data);
    mutex.unlock();
}

void CWriteThread::run()
{
    forever {
        if (checkStop()) {
            break;
        } else if (!sp->getStatus()){
            msleep(30);
        } else {
            mutex.lock();
            if(!dataQueue.isEmpty()) {
                QByteArray data = dataQueue.dequeue();
                sp->write(data.data(), data.size());
                mutex.unlock();
                msleep(5);
            } else {
                mutex.unlock();
                usleep(100);
            }
        }
    }
}

bool CWriteThread::checkStop()
{
    mutex.lock();
    bool ret = stopFlag;
    mutex.unlock();

    return ret;
}

void CWriteThread::stop()
{
    if(!checkStop()) {
        mutex.lock();
        stopFlag = true;
        mutex.unlock();
    }
}

#include "cpollingthread.h"
#include "cserialportprivate.h"
#include <QDebug>

CPollingThread::CPollingThread(CSerialPortPrivate *sp, QObject *parent) :
    QThread(parent)
{
    this->sp = sp;
    stopFlag = false;
    charTimeout = 18000;
    packTimeout = 100;
}

void CPollingThread::setCharTimeout(qint64 us)
{
    if (!sp->getStatus())
        charTimeout = us;
}

void CPollingThread::setPackTimeout(qint64 ms)
{
    packTimeout = ms;
}

void CPollingThread::run()
{
    state = CSerialPortPrivate::STATE_REQUESTING; //CSerialPortPrivate::STATE_WRITING;
    forever {
        if (checkStop()) {
            break;
        } else if (!sp->getStatus()){
            msleep(30);
        } else {
            doPoll();
        }
    }
    qDebug() << "stop";
}

void CPollingThread::doPoll()
{
    //qDebug() << state;
    switch (state) {
    case CSerialPortPrivate::STATE_REQUESTING:
        requesting();
        break;
    case CSerialPortPrivate::STATE_WRITING:
        writing();
        break;
    case CSerialPortPrivate::STATE_POLLING:
        polling();
        break;
    case CSerialPortPrivate::STATE_READING:
        reading();
        break;
    case CSerialPortPrivate::STATE_NOTIFY_DATA:
        notifying();
        break;
    case CSerialPortPrivate::STATE_NOTIFY_TIMEOUT:
        notifyingTimeout();
        break;
    default:
        break;
    }

    usleep(100);
}

void CPollingThread::write(const QByteArray &data, int timeout, int retry)
{
    //qDebug() << "write";
    mutex.lock();
    this->data = data;
    data_timeout = timeout;
    if(data_timeout == 0)
        data_timeout = packTimeout;
    data_retry = retry;
    mutex.unlock();
}

// 请求轮询数据
void CPollingThread::requesting()
{
    sp->requesting();
    state = CSerialPortPrivate::STATE_WRITING;
}

// 查询是否有数据要发送, 有则发送
void CPollingThread::writing()
{
    mutex.lock();
    if (!data.isEmpty()) {
        sp->write(data.data(), data.size());
        state = CSerialPortPrivate::STATE_POLLING;
        timer.Start();
    }
    mutex.unlock();
}

// 查询是否数据查询
void CPollingThread::polling()
{
    read_len = sp->read(read_buf, 128);
    //qDebug()<< read_len << "poll" << data_timeout;
    if (read_len > 0){
        data = QByteArray(read_buf, read_len);
        state = CSerialPortPrivate::STATE_READING;
        timer.Start();
    } else if(timer.Timeout(data_timeout)) {
        if (data_retry > 0){
            data_retry--;
            state = CSerialPortPrivate::STATE_WRITING;
        } else {
            data.clear();
            state = CSerialPortPrivate::STATE_NOTIFY_TIMEOUT;
        }
    }
}

// 读数据
void CPollingThread::reading()
{
    read_len = sp->read(read_buf, 128);
    if (read_len > 0){
        data.append(QByteArray(read_buf, read_len));
        timer.Start();
    } else if (timer.TimeoutMicro(charTimeout)) {
        state = CSerialPortPrivate::STATE_NOTIFY_DATA;
    }
}

// 读到完整数据包
void CPollingThread::notifying()
{
    sp->readReady(data);
    data.clear();
    state = CSerialPortPrivate::STATE_REQUESTING;
}

// 等待响应超时
void CPollingThread::notifyingTimeout()
{
    sp->packTimeout();
    state = CSerialPortPrivate::STATE_REQUESTING;
}

bool CPollingThread::checkStop()
{
    mutex.lock();
    bool ret = stopFlag;
    mutex.unlock();

    return ret;
}

void CPollingThread::stop()
{
    if(!checkStop()) {
        mutex.lock();
        stopFlag = true;
        mutex.unlock();
    }
}

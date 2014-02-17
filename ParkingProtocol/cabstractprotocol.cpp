#include "cabstractprotocol.h"
#include "../SerialPort/cserialpolling.h"

#include <QThread>
#include <QDebug>
#include "../SerialPort/qextserialenumerator.h"

CAbstractProtocol::CAbstractProtocol(QObject *parent)
    : QObject(parent)
{
    testFlag = true;
    sp = new CSerialPolling(this);

    connect(sp, &CSerialPolling::dataReady, this, &CAbstractProtocol::dataReady);
    connect(sp, &CSerialPolling::dataRequest, this, &CAbstractProtocol::dataRequest);
    connect(sp, &CSerialPolling::respondTimeout, this, &CAbstractProtocol::respondTimeout);

    timer = new QTimer(this);
    timer->setInterval(1000);
    timer->setSingleShot(true);

    connect(timer, &QTimer::timeout, this, &CAbstractProtocol::testNextPort);
    connect(this, &CAbstractProtocol::deviceSet, this, &CAbstractProtocol::setSlot);
    connect(this, &CAbstractProtocol::deviceAdd, this, &CAbstractProtocol::addSlot);
    connect(this, &CAbstractProtocol::deviceRemove, this, &CAbstractProtocol::removeSlot);
}

void CAbstractProtocol::setDevices(const QStringList &list)
{
    emit deviceSet(list);
}

void CAbstractProtocol::setSlot(const QStringList &list)
{
    qDebug() << "set:" << list << "old:" << deviceList;
    QStringList tmpList = list;

    qSort(tmpList);

    if (tmpList != deviceList){
        deviceList = tmpList;
        resetDevice();
    }
    qDebug() << "set:" << deviceList;
}

void CAbstractProtocol::addDevices(const QStringList &list)
{
    emit deviceAdd(list);
}

void CAbstractProtocol::addSlot(const QStringList &list)
{
    qDebug() << "add:" << list << "old:" << deviceList;
    QStringList tmpList = list;
    tmpList.append(deviceList);
    tmpList.removeDuplicates();

    qSort(tmpList);

    if (tmpList != deviceList){
        deviceList = tmpList;
        resetDevice();
    }
    qDebug() << "add:" << deviceList;
}

void CAbstractProtocol::removeDevices(const QStringList &list)
{
    emit deviceRemove(list);
}

void CAbstractProtocol::removeSlot(const QStringList &list)
{
    qDebug() << "remove:" << list << "old:" << deviceList;

    QStringList tmpList = deviceList;

    foreach (QString sn, list) {
        tmpList.removeOne(sn);
    }

    if (tmpList != deviceList){
        deviceList = tmpList;
        resetDevice();
    }

    qDebug() << "remove:" << deviceList;
}

void CAbstractProtocol::resetDevice()
{
    deviceStatusMap.clear();

    foreach (QString sn, deviceList) {
        deviceStatusMap.insert(sn, true);
    }

    resetFlag = true;

    testNextPort();
}

void CAbstractProtocol::testNextPort()
{
    testFlag = true;

    if(deviceList.isEmpty())
        return;

    if(portIndex >= portList.size()) { // 越界后调整
        reflashPortList();
    }

    if(portList.isEmpty() || !sp->open(portList.at(portIndex++))) { // 无串口/串口打开失败, 则定期查询
        timer->start();
        return;
    }

    deviceIndex = 0;
    resetFlag = false;

    dataRequest(); // 主动发数据
}

void CAbstractProtocol::dataRequest()
{
    AbstractPollData data;

    //qDebug() << ""
    if(resetFlag) {
        //testNextPort();
        timer->start(); // 避免频繁打开关闭串口
        return;
    }

    if (testFlag) {
        data = testData(getNextSn()); // 测试数据
    } else {
        data = queueData(); // 队列数据

        if (data.data.isEmpty()) {
            QString sn = getNextSn();

            if (deviceStatusMap.value(sn, false)) {
                data = comData(sn); // 令牌数据
            } else {
                data = testData(sn); // 设置地址
            }
        }
    }

    sp->write(data.data, data.timeout, data.retry);
}


void CAbstractProtocol::dataReady(const QByteArray &data)
{
    //qDebug() << "data recv:" << data.toHex() << isAnyOther() << testFlag;
    if (isAnyOther())
        return;

    QString sn;
    bool flag;


    if (resetFlag)
        return;

    if (testFlag)
        processTestData(data, sn, &flag);
    else
        processComData(data, sn, &flag);

    qDebug() << flag;
    setComFlag(sn, flag);
}



void CAbstractProtocol::respondTimeout()
{
    if (isAnyOther())
        return;

    QString sn;
    bool flag;

    if (resetFlag)
        return;

    processTimeout(sn, &flag);

    setComFlag(sn, flag);
}

void CAbstractProtocol::setComFlag(const QString &sn, bool flag)
{
    if (deviceStatusMap.contains(sn)) {
        if(testFlag && flag) {
            testFlag = false;
        }

        deviceStatusMap.insert(sn, flag);
        if (sn == deviceList.last() &&
                deviceStatusMap.values().count() == deviceStatusMap.values().count(false)) {
            resetFlag = true;
        }
    }
}

CAbstractProtocol::~CAbstractProtocol()
{
    delete sp;
    delete timer;
}

QString CAbstractProtocol::getNextSn()
{
    QString sn = deviceList.at(deviceIndex++);

    deviceIndex = deviceIndex % (deviceList.size());

    return sn;
}

void CAbstractProtocol::reflashPortList()
{
    portList = QextSerialEnumerator::getPorts();
    portIndex = 0;
}

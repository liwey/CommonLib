#include "cparkingprotocolprivate.h"
#include <QThread>
#include <QDebug>
#include <QMetaType>

CParkingProtocolPrivate::CParkingProtocolPrivate(QObject *parent) :
    CAbstractProtocol(parent)
{
    if (!QMetaType::isRegistered(QMetaType::type("ParkingData")))
        qRegisterMetaType<ParkingData>("ParkingData");

    anyFlag = false;

    thread = new QThread();
    this->moveToThread(thread);


    QByteArray pass = QByteArray::fromHex("0123456789abcdef0123456789");

    for(int i = 0; i < 13; i++) {
        _password[i] = (unsigned char)pass.at(i);
    }

    connect(this, SIGNAL(parkingAdd(ParkingData)), SLOT(addParking(ParkingData)));
    connect(this, SIGNAL(anyAdd(AnyData)), SLOT(addAny(AnyData)));

    thread->start();
}

ParkingData CParkingProtocolPrivate::mkData(const QString &sn, char funCode, const QByteArray &data, int timeout, bool respond, int retry)
{
    ParkingData pd;
    pd.SN = sn;
    pd.FUN = funCode;
    pd.respond = respond;
    pd.data = data;
    pd.retry = retry;
    pd.timeout = timeout;

    return pd;
}

void CParkingProtocolPrivate::sendData(const AnyData &data)
{
    emit anyAdd(data);
}

void CParkingProtocolPrivate::sendData(const ParkingData &data)
{
    emit parkingAdd(data);
}

void CParkingProtocolPrivate::addAny(const AnyData &data)
{
    anyQueue.enqueue(data);
}

void CParkingProtocolPrivate::addParking(const ParkingData &data)
{
    parkingQueue.enqueue(data);
}

AbstractPollData CParkingProtocolPrivate::queueData()
{
    AbstractPollData data;

    anyFlag = !(anyQueue.isEmpty());
    //qDebug() << anyQueue.size() << "any";
    if (anyFlag){
        currentAny = anyQueue.dequeue();
        data.data = currentAny.data;
        data.retry = 0;
        data.timeout = currentAny.timeout;
    } else if(!parkingQueue.isEmpty()){
        currentParking = parkingQueue.dequeue();
        data.data = mkpack();
        data.retry = currentParking.retry;
        data.timeout = currentParking.timeout;
    } else {
        data.data.clear();
    }

    return data;
}

AbstractPollData CParkingProtocolPrivate::comData(const QString &sn)
{
    AbstractPollData data;

    currentParking = mkData(sn, FUN_TOKEN, QByteArray(), 100, true, 0);
    data.data = mkpack();
    data.retry = currentParking.retry;
    data.timeout = currentParking.timeout;

    return data;
}

AbstractPollData CParkingProtocolPrivate::testData(const QString &sn)
{
    AbstractPollData data;

    currentParking = mkData(sn, FUN_ADDRESS, sn.toLatin1(), 100, true, 0);
    data.data = mkpack();
    data.retry = currentParking.retry;
    data.timeout = currentParking.timeout;

    return data;
}

QByteArray CParkingProtocolPrivate::mkpack()
{
    QByteArray data;
    data.clear();

    // 报文头, 目前报文头直接硬编码, 会忽略 CommunicateDataInfo STX设置
    data.append(char(0x2D));

    char addr;
    if (currentParking.SN == "FFFFFF") {
        addr = char(0xFF);
        currentParking.respond= false;
    } else {
        addr = this->addr(currentParking.SN);
    }
    // 设置地址
    if(currentParking.FUN == FUN_ADDRESS)
    {
        addr |= char(0x80);
    }

    data.append(addr);
    data.append(TID);
    data.append(currentParking.FUN);
    data.append(char(currentParking.data.size()));
    data.append(currentParking.data);

    cryption(&data);

    // BCC 校验
    char bcc = 0;
    for(int i = 0; i < data.size(); i++)
        bcc ^= data.at(i);

    data.append(bcc);

    return data;
}

// 解包
bool CParkingProtocolPrivate::unpack(const QByteArray &pack)
{
    int size = pack.size();
    if(size < 6 || char(0x2D) != pack.at(0)) {
        return false;
    }

    // 计算 BCC
    char bcc =  0;
    for(int i = 0; i < (pack.size()-1); i++)
        bcc ^= pack.at(i);
    // 检测
    if(bcc != pack.at(pack.size()-1))
        return false;

    QByteArray data = pack;
    // 解密
    cryption(&data, false);
    // 检查数据长度
    if(data.size() != (data.at(4) + 6))
        return false;
    qDebug() << "1";
    // 检查交易码
    if(data.at(2) != TID)
        return false;

    // 地址无对应设备
    QString sn = deviceSn(data.at(1));
    if(sn != currentParking.SN || sn.isEmpty())
    {
        return false;
    }

    returnParking.SN = sn;
    returnParking.FUN = data.at(3);
    returnParking.data = data.mid(5, data.at(4));

    return true;
}

void CParkingProtocolPrivate::cryption(QByteArray *data, bool encrypt)
{
    unsigned char x;
    unsigned char y;
    unsigned char xorIndex;
    unsigned char state[256];

    if(data->size() >= 5)
    {
        _password[13] = (unsigned char)data->at(1);
        _password[14] = (unsigned char)data->at(2);
        _password[15] = _password[13] ^ _password[14];

        QByteArray temp;
        if(encrypt)
            temp = data->mid(3);
        else
            temp = data->mid(3, data->size()-4);

        // 初始化
        for(int i = 0; i < 256; i++)
        {
            state[i] = i;
        }

        // 生成伪随机序列, 用作子密钥序列
        x = 0;
        y = 0;
        for(int i = 0; i < 256; i++)
        {
            y = (_password[x] + state[i] + y) % 256;
            qSwap(state[i], state[y]);

            x = (x + 1) % 16;
        }

        // 用得到的子密钥和 明文/密文 进行XOR运算, 得到 密文/明文(加密/解密)
        x = 0;
        y = 0;

        for(int i = 0; i < temp.size(); i++)
        {
             x = (x + 1) % 256;
             y = (state[x] + y) % 256;
             qSwap(state[x], state[y]);

             xorIndex = (state[x] + state[y]) % 256;

             temp[i] = temp.at(i)^state[xorIndex];
        }
        data->replace(3, temp.size(), temp);
    }
}

void CParkingProtocolPrivate::processComData(const QByteArray &data, QString &sn, bool *ok)
{
    if(unpack(data)) {
        sn = currentParking.SN;
        *ok = true;
        // emit
    } else {
        sn = currentParking.SN;
        *ok = false;
        // emit
    }
}

// 寻找设备, 即设置地址不向上层接口报告
void CParkingProtocolPrivate::processTestData(const QByteArray &data, QString &sn, bool *ok)
{
    if(unpack(data)) {
        sn = currentParking.SN;
        *ok = true;
        if (currentParking.FUN != FUN_ADDRESS)
            emit errorOccur(sn, (int) returnParking.FUN, returnParking.data);
    } else {
        sn = currentParking.SN;
        *ok = false;
        if (currentParking.FUN != FUN_ADDRESS)
            emit errorOccur(sn, (int) currentParking.FUN, currentParking.data);
    }
}

void CParkingProtocolPrivate::processTimeout(QString &sn, bool *ok)
{
    sn = currentParking.SN;
    *ok = (currentParking.respond == false);
    //emit
    if (currentParking.respond && currentParking.FUN != FUN_ADDRESS)
        emit errorOccur(sn, (int) currentParking.FUN, currentParking.data);
}

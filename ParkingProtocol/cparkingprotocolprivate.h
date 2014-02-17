#ifndef CPARKINGPROTOCOLPRIVATE_H
#define CPARKINGPROTOCOLPRIVATE_H

#include "cabstractprotocol.h"
#include <QQueue>

typedef struct ParkingData {
    char FUN;
    QString SN;
    QByteArray data;
    int timeout;
    int retry;
    bool respond;
}ParkingData;

typedef struct AnyData {
    QByteArray data;
    int timeout;
    //int retry; // 总是不重试
    //bool respond; // 总是不需回应
}AnyData;

class CParkingProtocolPrivate : public CAbstractProtocol
{
    Q_OBJECT
public:
    explicit CParkingProtocolPrivate(QObject *parent = 0);

    void sendData(const ParkingData &data);
    void sendData(const AnyData &data);

    static ParkingData mkData(const QString &sn, char funCode, const QByteArray &data, int timeout, bool respond, int retry);

signals:
    void parkingAdd(const ParkingData &data);
    void anyAdd(const AnyData &data);
    void errorOccur(const QString &sn, int code, const QByteArray &data);
    void respondData(const QString &sn, int code, const QByteArray &data);

private slots:
    void addParking(const ParkingData &data);
    void addAny(const AnyData &data);

private:
    void cryption(QByteArray *data, bool encrypt = true);
    QByteArray mkpack();
    bool unpack(const QByteArray &data);
    void processComData(const QByteArray &data, QString &sn, bool *ok);
    void processTestData(const QByteArray &data, QString &sn, bool *ok);
    void processTimeout(QString &sn, bool *ok);
    AbstractPollData comData(const QString &sn);
    AbstractPollData testData(const QString &sn);
    AbstractPollData queueData();
    bool isAnyOther() { return anyFlag; }

public slots:

private:
    QQueue<ParkingData> parkingQueue;
    QQueue<AnyData> anyQueue;
    QThread *thread;

    bool anyFlag;
    AnyData currentAny;
    ParkingData currentParking;
    ParkingData returnParking;

    static const char FUN_TOKEN = char(0x00);
    static const char FUN_ADDRESS = char(0x01);
    static const char FUN_STATUS = char(0x12);
    static const char STATUS_MASK_ZHA = char(0x03);
    static const char STATUS_MASK_KAJI = char(0x3C);
    static const char STATUS_MASK_JILU = char(0x80);

    char TID;
    char _password[16];
};

#endif // CPARKINGPROTOCOLPRIVATE_H

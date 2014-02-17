#ifndef CABSTRACTPROTOCOL_H
#define CABSTRACTPROTOCOL_H

#include <QObject>
#include <QMap>
#include <QString>
#include <QStringList>
#include <QTimer>

typedef struct AbstractPollData {
    QByteArray data;
    int timeout;
    int retry;
}AbstractPollData;

class QThread;
class CSerialPolling;

class CAbstractProtocol : public QObject
{
    Q_OBJECT

public:
    CAbstractProtocol(QObject *parent = 0);
    ~CAbstractProtocol();

    void setDevices(const QStringList &list);
    void addDevices(const QStringList &list);
    void removeDevices(const QStringList &list);

    char addr(const QString &sn) { return char(deviceList.indexOf(sn) + 1); }
    QString deviceSn(char addr) { return deviceList.value(((int)addr) - 1, QString()); }

signals:
    void deviceAdd(const QStringList &list);
    void deviceSet(const QStringList &list);
    void deviceRemove(const QStringList &list);

private slots:
    void dataReady(const QByteArray &data);
    void dataRequest();
    void respondTimeout();

    void addSlot(const QStringList &list);
    void setSlot(const QStringList &list);
    void removeSlot(const QStringList &list);

protected:
    virtual void processComData(const QByteArray &data, QString &sn, bool *ok) = 0;
    virtual void processTestData(const QByteArray &data, QString &sn, bool *ok) = 0;
    virtual void processTimeout(QString &sn, bool *ok) = 0;
    virtual AbstractPollData comData(const QString &sn) = 0;
    virtual AbstractPollData testData(const QString &sn) = 0;
    virtual AbstractPollData queueData() = 0;
    virtual bool isAnyOther() { return false; }

private:
    void setComFlag(const QString &sn, bool flag);
    void resetDevice();
    void testNextPort();
    void reflashPortList();
    QString getNextSn();

public slots:

private:
    CSerialPolling *sp;
    QStringList deviceList;
    bool testFlag;
    bool resetFlag;

    QMap<QString, bool> deviceStatusMap;

    int deviceIndex;
    int portIndex;

    QStringList portList;

    QTimer *timer;
};

#endif // CABSTRACTPROTOCOL_H

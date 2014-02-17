#ifndef CPARKINGPROTOCOL_H
#define CPARKINGPROTOCOL_H

#include<QObject>
#include<QStringList>
#include<QByteArray>

class CParkingProtocolPrivate;

class CParkingProtocol : public QObject
{
    Q_OBJECT
    Q_CLASSINFO("ClassID", "{FC3D600D-F30D-C6D7-97ED-FC16A968FCE3}")
    Q_CLASSINFO("InterfaceID", "{966818DA-1A1D-F817-C651-B7493B6644ED}")
    Q_CLASSINFO("EventsID", "{B0F8BDE7-DAD4-36B0-2B71-0C8143F34C97}")

public:
    explicit CParkingProtocol(QObject *parent = 0);
    ~CParkingProtocol();

signals:
    void errorOccur(const QString &sn, int code, const QByteArray &data);
    void respondData(const QString &sn, int code, const QByteArray &data);

public slots:
    // 设置设备列表, 即添加多个设备, 添加前会清空之前添加的设备
    void setDevices(const QStringList &list);
    // 添加一个设备, 不影响之前添加的设备
    void addDevice(const QString &sn);
    // 添加多个设备, 不影响之前添加的设备
    void addDevices(const QStringList &list);
    // 删除一个设备
    void removeDevice(const QString &sn);
    // 删除多个设备. 如果要删除所有设备可以给setDevices传递一个空的设备列表
    void removeDevices(const QStringList &list);

    void openBarrier(const QString &sn, int retry, int timeout);
    void closeBarrier(const QString &sn, int retry, int timeout);

    void transparent(const QString &sn, const QByteArray &data, int retry, int timeout);
private:
    CParkingProtocolPrivate *pp;
};

#endif // CPARKINGPROTOCOL_H

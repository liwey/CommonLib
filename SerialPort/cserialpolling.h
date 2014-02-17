#ifndef CSERIALPOLLING_H
#define CSERIALPOLLING_H

#include <QObject>

class CSerialPortPrivate;
class CPollingThread;
class CSerialPolling : public QObject
{
    Q_OBJECT
public:
    explicit CSerialPolling(QObject *parent = 0);
    bool open(const QString &portName);
    void close();
    void write(const QByteArray &data, int timeout = 0, int retry = 0);
    // 必须在串口打开前调用才生效, 否则无效
    void setCharTimeout(qint64 us);

signals:
    void dataReady(const QByteArray &data);
    void dataRequest();
    void respondTimeout();

public slots:

private:
    CSerialPortPrivate *sp;
    CPollingThread *pt;
};

#endif // CSERIALPOLLING_H

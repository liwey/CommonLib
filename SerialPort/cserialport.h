#ifndef CSERIALPORT_H
#define CSERIALPORT_H

#include <QObject>

class CSerialPortPrivate;
class CReadThread;
class CWriteThread;
class CSerialPort : public QObject
{
    Q_OBJECT
public:
    explicit CSerialPort(QObject *parent = 0);
    bool open(const QString &portName);
    void close();
    void setAutoConnect();
    void write(const QByteArray &data);
    // 必须在串口打开前调用才生效, 否则无效
    void setCharTimeout(qint64 us);

signals:
    void dataReady(const QByteArray &data);

public slots:

private:
    CSerialPortPrivate *sp;
    CReadThread *rt;
    CWriteThread *wt;
};

#endif // CSERIALPORT_H

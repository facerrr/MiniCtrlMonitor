#ifndef RXTHREAD_H
#define RXTHREAD_H

#include <QObject>
#include <QThread>
#include <QDateTime>
#include <QSerialPort>
#include <xserialport.h>
#include <QDoubleSpinBox>
#include <QLabel>
#include <QDebug>



class xQObjectRx : public QObject
{
    Q_OBJECT
public:
    explicit xQObjectRx(QObject *parent = nullptr);
    ~xQObjectRx(void);
    QDoubleSpinBox *rxPmSpinBoxs[14] = {nullptr};
    QLabel *qLabel = nullptr;

    /*------------串口基础类，必须初始化------------*/
    xSerialPort   *xSerial = nullptr;
    void xSerialAddrSet(xSerialPort   *xSerial_t)
    {
        xSerial = xSerial_t;
    }
    /*------------串口基础类，必须初始化------------*/
    void xConnectRx(void);
    void xDisConnectRx(void);
    void setPmSpinBoxAddr(QDoubleSpinBox *addrs[14]);
    void setQLabelAddr(QLabel *addr);

public slots:
    void xReadSlot(void);
private:
signals:
    void RxShow(const QByteArray &str);
    void graphUpdate(const QByteArray &str);
};


class RxThread : public QThread
{
    Q_OBJECT
public:
    explicit RxThread(QObject *parent = nullptr);
public:
    void run(void)
    {
        qDebug()<<"main tid:xQObjectRx run"<< QThread::currentThreadId();
        exec();
    }
signals:
public slots:
};

#endif // RXTHREA_H

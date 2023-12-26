#ifndef TXTHREAD_H
#define TXTHREAD_H

#include <QObject>
#include <QThread>
#include <QDebug>
#include <QSerialPort>
#include <xserialport.h>
#include <QPushButton>
#include <QRadioButton>
#include <QDoubleSpinBox>
#include <QLineEdit>
#include <QLabel>


class xQObjectTx : public QObject
{
    Q_OBJECT
public:
    explicit xQObjectTx(QObject *parent = nullptr);
    ~xQObjectTx(void);
    xSerialPort   *xSerial = nullptr;
    QPushButton   *modeButton[4] ={nullptr};
    QPushButton   *RunButton  = nullptr;
    QRadioButton  *radioButtons[3] = {nullptr};
    QDoubleSpinBox *txPmSpinBoxs[14] = {nullptr};
    QDoubleSpinBox   *ctrlSpinBox[3] = {nullptr};
    QByteArray    sendBuff;
    QLabel *qLabel = nullptr;
    void xSerialAddrSet(xSerialPort   *xSerial_t)
    {
        xSerial = xSerial_t;
    }
    void xConnectTx(void);
    void xDisConnectTx(void);
    void setPmSpinBoxAddr(QDoubleSpinBox *addrs[14]);
    void setCtrlSpinBoxAddr(QDoubleSpinBox *addr1,QDoubleSpinBox *addr2,QDoubleSpinBox *addr3);
    void setModeButtonsAddr(QPushButton *addr1,QPushButton *addr2,QPushButton *addr3,QPushButton *addr4);
    void setRunButtonAddr(QPushButton *addr);
    void setRadioButtonsAddr(QRadioButton  *addr1,QRadioButton  *addr2,QRadioButton  *addr3);
    void setQLabelAddr(QLabel *addr);
    void InitConnection(void);

public slots:
    void Mode1Clicked(int value);
    void Mode2Clicked(int value);
    void Mode3Clicked(int value);
    void Mode4Clicked(int value);
    void RunButtonToggled(bool checked);
    void ControlValueChange(double value);
signals:
    void xTxMessage(const QByteArray &str);
    void synTarget(const float value,const int index);
};



class TxThread : public QThread
{
    Q_OBJECT
public:
    explicit TxThread(QObject *parent = nullptr);
public:
    void run(void)
    {
        qDebug()<<"main tid:xQObjectTx run"<< QThread::currentThreadId();
        exec();
    }
signals:
public slots:
};

#endif // TXTHREAD_H

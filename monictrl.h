#ifndef MONICTRL_H
#define MONICTRL_H

#include <QObject>
#include <rxthread.h>
#include <plotthread.h>
#include <txthread.h>

class MoniCtrl : public QObject
{
    Q_OBJECT
public:
    explicit MoniCtrl(QObject *parent = nullptr);
    ~MoniCtrl(void);
    xSerialPort    xSerial;
    xSerialThread  xComThread;
    xQObjectRx   xQObjectRxCtr;
    xQObjectTx   xQObjectTxCtr;
    RxThread rxThread;
    TxThread txThread;
    xQCustomPlot *xQCustPlots = new xQCustomPlot[4];
    PlotThread *xPlotThreads = new PlotThread[4];
signals:

};

#endif // MONICTRL_H

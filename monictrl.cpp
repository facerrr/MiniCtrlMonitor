#include "monictrl.h"

MoniCtrl::MoniCtrl(QObject *parent) : QObject(parent)
{
    /*配置共享类的地址*/
    this->xQObjectRxCtr.xSerialAddrSet(&this->xSerial);
    this->xQObjectTxCtr.xSerialAddrSet(&this->xSerial);
    /***********************基础串口类***********************/
    this->xSerial.setObjectName("xSerial");
    this->xSerial.moveToThread(&this->xComThread);
    this->xSerial.qSerial->moveToThread(&this->xComThread);
    /***********************发送相关***********************/
    this->xQObjectTxCtr.setObjectName("xSerialTx");
    this->xQObjectTxCtr.moveToThread(&this->txThread);
    /***********************接收相关***********************/
    this->xQObjectRxCtr.setObjectName("xSerialRx");
    this->xQObjectRxCtr.moveToThread(&this->rxThread);
    /***********************绘图相关***********************/
    this->xQCustPlots[0].setObjectName("xPositionPlot");
    this->xQCustPlots[1].setObjectName("xVelocityPlot");
    this->xQCustPlots[2].setObjectName("xCurrentPlot");
    this->xQCustPlots[3].setObjectName("xCurrentDqPlot");
    this->xPlotThreads[0].setObjectName("PositionPlotThread");
    this->xPlotThreads[1].setObjectName("VelocityPlotThread");
    this->xPlotThreads[2].setObjectName("CurrentPlotThread");
    this->xPlotThreads[3].setObjectName("CurrentDqPlotThread");
    for(uint8_t i=0;i<4;i++)
    {
        this->xQCustPlots[i].moveToThread(&this->xPlotThreads[i]);
        this->xQCustPlots[i].setWindowNum(i);
        this->xPlotThreads[i].start();
    }
    //开启线程
    this->xComThread.start();
    this->rxThread.start();
    this->txThread.start();
}

MoniCtrl::~MoniCtrl(void)
{
    this->txThread.exit();
    this->txThread.wait();
    this->rxThread.exit();
    this->rxThread.wait();
    this->xComThread.exit();
    this->xComThread.wait();
    for(uint8_t i=0;i<4;i++)
    {
        this->xPlotThreads[i].exit();
        this->xPlotThreads[i].wait();

    }
}

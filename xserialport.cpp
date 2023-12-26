#include "xserialport.h"
/*
 *
 *
 *该文件主要提供可以实现的串口调用接口
 *
 */
#include <QThread>
#include <QDebug>
xSerialPort::xSerialPort(QObject *parent) : QObject(parent)
{
    qDebug()<<"main tid:vSerialPort"<< QThread::currentThreadId();
    this->qSerial        = new QSerialPort;
    this->xSerialConfig  = new xSerialConfigStruct;
    this->xSerialData    = new xSerialDataStruct;
}

xSerialPort::~xSerialPort(void)
{
    delete this->qSerial;
    delete this->xSerialConfig;
    delete this->xSerialData;
    delete [] this->xSerialComInfo;
};

//通过相关事件触发此函数，进行串口设备查询，并更新可用设备列表
void xSerialPort::xSerialComScanf(void)
{
    qint32 SerialComCnt  = 0;
    this->xSerialPortCnt = 0;
    //要删除原有的内存空间
    if(this->xSerialComInfo!=nullptr)
    {
        delete [] this->xSerialComInfo;
    }
    //获取串口设备数量
    this->xSerialPortCnt = QSerialPortInfo::availablePorts().length();
    this->xSerialComInfo = new xSerialComInfoStruct[this->xSerialPortCnt];
    SerialComCnt = 0;
    foreach(const QSerialPortInfo &port,QSerialPortInfo::availablePorts())
    {
        this->xSerialComInfo[SerialComCnt].SerialName      = port.portName();
        this->xSerialComInfo[SerialComCnt].ProductCode     = port.productIdentifier();
        this->xSerialComInfo[SerialComCnt].SyestemPosition = port.systemLocation();
        this->xSerialComInfo[SerialComCnt].SerialNumStr    = port.serialNumber();
        this->xSerialComInfo[SerialComCnt].DescribeStr     = port.description();
        this->xSerialComInfo[SerialComCnt].Manufacturer    = port.manufacturer();
        this->xSerialComInfo[SerialComCnt].SupplierCode    = port.vendorIdentifier();
        SerialComCnt++;
    }
}

//打开选中的串口设备
void xSerialPort::xSerialOpen(bool & isOpen)
{
    //关闭串口
    this->xSerialClose();
    //设置串口名称
    this->qSerial->setPortName(this->xSerialConfig->xSerialPortName);
    //设置串口波特率
    this->qSerial->setBaudRate(this->xSerialConfig->xSerialBaudRate);
    //设置停止位
    this->qSerial->setStopBits(this->xSerialConfig->xSerialStopBits);
    //设置数据位
    this->qSerial->setDataBits(this->xSerialConfig->xSerialDataBits);
    //设置奇偶校验
    this->qSerial->setParity(this->xSerialConfig->xSerialParrity);
    //打开串口设备
    isOpen =  (this->qSerial->open(QIODevice::ReadWrite));
}

void xSerialPort::xSerialClose(void)
{
    //关闭串口
    if(this->qSerial->isOpen())
    {
        this->qSerial->close();
    }
}

///*同一时间只允许一个线程调用*/
void xSerialPort::xWriteData(const QByteArray &str)
{
    if(this->qSerial->isOpen())
    {
        //统计
        this->xSerialData->txByteCnt+=str.length();
        this->qSerial->write(str);
    }
    else
    {
        emit TxError();
    }
}

//清除统计的数据量
void xSerialPort::xClearCntNum(void)
{
    this->xSerialData->txByteCnt = 0;
    this->xSerialData->rxByteCnt = 0;
}

void xSerialPort::xSerialError(QSerialPort::SerialPortError serialPortError)
{

};

//串口线程
xSerialThread::xSerialThread(QObject *parent) : QThread(parent)
{

}












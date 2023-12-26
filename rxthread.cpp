#include "rxthread.h"

xQObjectRx::xQObjectRx(QObject *parent) : QObject(parent)
{

}
xQObjectRx::~xQObjectRx(void)
{

};

//打开接收的信号与槽，方便切换
void xQObjectRx::xConnectRx(void)
{
    connect(this->xSerial->qSerial,&QSerialPort::readyRead,
            this,&xQObjectRx::xReadSlot,Qt::QueuedConnection);
}
//断开接收的信号与槽，方便切换
void xQObjectRx::xDisConnectRx(void)
{
    disconnect(this->xSerial->qSerial,&QSerialPort::readyRead,
            this,&xQObjectRx::xReadSlot);
}

void xQObjectRx::setPmSpinBoxAddr(QDoubleSpinBox *addrs[14])
{
    for(int i=0;i<14;i++)
    {
        this->rxPmSpinBoxs[i] = addrs[i];
        if(this->rxPmSpinBoxs[i]!=nullptr)
        {
            this->rxPmSpinBoxs[i]->setMaximumWidth(100);
            this->rxPmSpinBoxs[i]->setSingleStep(0.001);
            this->rxPmSpinBoxs[i]->setValue(0.1);
            this->rxPmSpinBoxs[i]->setDecimals(3);
        }
    }
}

void xQObjectRx::setQLabelAddr(QLabel *addr)
{
    this->qLabel = addr;
    this->qLabel->setText("This is xMonitor");
}

void xQObjectRx::xReadSlot(void)
{
    QByteArray rxSerialBuff;
    if(this->xSerial!=NULL)
    {
        if(this->xSerial->qSerial->isOpen())
        {
            rxSerialBuff = this->xSerial->qSerial->readAll();
            if(rxSerialBuff.isEmpty())
            {
                return;
            }
            else
            {
                this->xSerial->xSerialData->RxBuff.append((rxSerialBuff));
                if(rxSerialBuff.at(0) == 0x1f)
                {
                    emit graphUpdate(rxSerialBuff);
                }
                else if(rxSerialBuff.at(0) == 0x2f)
                {
                    if(this->qLabel!=nullptr)
                    {
                        this->qLabel->setText("Upload Parameter");
                    }
                    for(int i=0;i<14;i++)
                    {
                        if(this->rxPmSpinBoxs[i]!=nullptr)
                        {
                            QByteArray pm_byte = rxSerialBuff.mid(4*i+1,4*i+5);
                            float pm;
                            memcpy(&pm,pm_byte,4);
                            qDebug("data%d = %f",i,pm);
                            this->rxPmSpinBoxs[i]->setValue((double)pm);
                        }
                    }
                }
                else if(rxSerialBuff.at(0) == 0x3f)
                {
                    if(this->qLabel!=nullptr)
                    {
                        this->qLabel->setText("Calibration Start");
                    }
                }
                else if(rxSerialBuff.at(0) == 0x4f)
                {
                    if(this->qLabel!=nullptr)
                    {
                        float offset = 0.0f;
                        QByteArray data1 = rxSerialBuff.mid(2,6);
                        memcpy(&offset,data1,4);
                        QString info = QString("Calibration over, the ele_offset=%1").arg(QString::number(offset,'f',3));

                        this->qLabel->setText(info);
                    }
                }
                else if(rxSerialBuff.at(0) == 0x5f)
                {
                    if(this->qLabel!=nullptr)
                    {
                        this->qLabel->setText("Upload Parameter");
                    }
                }
                else if(rxSerialBuff.at(0) == 0x6f)
                {
                    if(this->qLabel!=nullptr)
                    {
                        this->qLabel->setText("Run Mode");
                    }
                }
                else if(rxSerialBuff.at(0) == 0x7f)
                {
                    if(rxSerialBuff.at(1)==0x1f)
                    {
                        if(this->qLabel!=nullptr)
                        {
                            this->qLabel->setText("Synchronize PM Sucessfully");
                        }
                    }
                    else if(rxSerialBuff.at(1) == 0x2f)
                    {
                        if(this->qLabel!=nullptr)
                        {
                            this->qLabel->setText("Fail To Synchronize PM");
                        }
                    }

                }
            }
        }

    }
}

RxThread::RxThread(QObject *parent) : QThread(parent)
{

}

#include "txthread.h"

xQObjectTx::xQObjectTx(QObject *parent) : QObject(parent)
{

}
xQObjectTx::~xQObjectTx(void)
{

};

void xQObjectTx::xConnectTx(void)
{
    connect(this,&xQObjectTx::xTxMessage,
            this->xSerial,&xSerialPort::xWriteData,
            Qt::QueuedConnection);
}

void xQObjectTx::xDisConnectTx(void)
{
    disconnect(this,&xQObjectTx::xTxMessage,
            this->xSerial,&xSerialPort::xWriteData);
}

void xQObjectTx::setModeButtonsAddr(QPushButton *addr1, QPushButton *addr2, QPushButton *addr3, QPushButton *addr4)
{
    this->modeButton[0] = addr1;
    this->modeButton[1] = addr2;
    this->modeButton[2] = addr3;
    this->modeButton[3] = addr4;
}

void xQObjectTx::setRunButtonAddr(QPushButton *addr)
{
    this->RunButton = addr;
}

void xQObjectTx::setRadioButtonsAddr(QRadioButton *addr1, QRadioButton *addr2, QRadioButton *addr3)
{
    this->radioButtons[0] = addr1;
    this->radioButtons[1] = addr2;
    this->radioButtons[2] = addr3;
}

void xQObjectTx::setPmSpinBoxAddr(QDoubleSpinBox *addrs[14])
{
    for(int i=0;i<14;i++)
    {
        this->txPmSpinBoxs[i] = addrs[i];
        if(this->txPmSpinBoxs[i]!=nullptr)
        {
            this->txPmSpinBoxs[i]->setMaximumWidth(100);
            this->txPmSpinBoxs[i]->setRange(-200,200);
            this->txPmSpinBoxs[i]->setSingleStep(0.001);
            this->txPmSpinBoxs[i]->setValue(0.1);
            this->txPmSpinBoxs[i]->setDecimals(3);
        }
    }
}

void xQObjectTx::setQLabelAddr(QLabel *addr)
{
    this->qLabel = addr;
    this->qLabel->setText("This is xMonitor");
}


void xQObjectTx::setCtrlSpinBoxAddr(QDoubleSpinBox *addr1, QDoubleSpinBox *addr2, QDoubleSpinBox *addr3)
{
    this->ctrlSpinBox[0] = addr1;
    this->ctrlSpinBox[0]->setValue(3.0);
    this->ctrlSpinBox[0]->setRange(-10,10);
    this->ctrlSpinBox[1] = addr2;
    this->ctrlSpinBox[1]->setValue(10.0);
    this->ctrlSpinBox[1]->setRange(-50,50);
    this->ctrlSpinBox[2] = addr3;
    this->ctrlSpinBox[2]->setValue(0.0);
    this->ctrlSpinBox[2]->setRange(-200,200);
    for(int i=0;i<3;i++)
    {
        this->ctrlSpinBox[i]->setDecimals(2);
        this->ctrlSpinBox[i]->setSingleStep(0.1);
        this->ctrlSpinBox[i]->setKeyboardTracking(false);
    }
}


void xQObjectTx::InitConnection(void)
{
    connect(this->modeButton[0],&QPushButton::clicked,this,&xQObjectTx::Mode1Clicked);
    connect(this->modeButton[1],&QPushButton::clicked,this,&xQObjectTx::Mode2Clicked);
    connect(this->modeButton[2],&QPushButton::clicked,this,&xQObjectTx::Mode3Clicked);
    connect(this->modeButton[3],&QPushButton::clicked,this,&xQObjectTx::Mode4Clicked);
    connect(this->RunButton,&QPushButton::toggled,this,&xQObjectTx::RunButtonToggled);
    connect(this->ctrlSpinBox[0],SIGNAL(valueChanged(double)),this,SLOT(ControlValueChange(double)));
    connect(this->ctrlSpinBox[1],SIGNAL(valueChanged(double)),this,SLOT(ControlValueChange(double)));
    connect(this->ctrlSpinBox[2],SIGNAL(valueChanged(double)),this,SLOT(ControlValueChange(double)));
    // radioButton发生变化，使能对应spinbox
    for(int i=0;i<3;i++)
    {
        connect(this->radioButtons[i],&QRadioButton::toggled,[=]()
        {
            if(this->radioButtons[i]->isChecked())
            {
                if(this->RunButton->isChecked())
                {
                    this->RunButton->setChecked(false);
                }
                this->ctrlSpinBox[i]->setEnabled(true);
                if(i==0){this->qLabel->setText("TORQUE  MODE");}
                if(i==1){this->qLabel->setText("VELOCITY  MODE");}
                if(i==2){this->qLabel->setText("POSITION  MODE");}
            }
            else{this->ctrlSpinBox[i]->setEnabled(false);}
        });
    }


}

void xQObjectTx::Mode1Clicked(int val)
{
    this->sendBuff.clear();
    this->sendBuff.append(0x4f);
    this->sendBuff.append(0xff);
    emit xTxMessage(this->sendBuff);
}

void xQObjectTx::Mode2Clicked(int val)
{
    this->sendBuff.clear();
    this->sendBuff.append(0x5f);
    this->sendBuff.append(0xff);
    float value_to_send = 0.0f;
    uint8_t floatarr[4];
    for(int i=0;i<14;i++)
    {
        value_to_send = this->txPmSpinBoxs[i]->value();
        *(float *)floatarr = value_to_send;
        this->sendBuff.append(floatarr[3]);
        this->sendBuff.append(floatarr[2]);
        this->sendBuff.append(floatarr[1]);
        this->sendBuff.append(floatarr[0]);

    }
    emit xTxMessage(this->sendBuff);
}

void xQObjectTx::Mode3Clicked(int val)
{
    this->sendBuff.clear();
    this->sendBuff.append(0x6f);
    this->sendBuff.append(0xff);
    emit xTxMessage(this->sendBuff);
}

void xQObjectTx::Mode4Clicked(int val)
{
    this->sendBuff.clear();
    this->sendBuff.append(0x7f);
    this->sendBuff.append(0xff);
    emit xTxMessage(this->sendBuff);
}

void xQObjectTx::RunButtonToggled(bool checked)
{
    if(checked)
    {
        this->sendBuff.clear();
        if(this->radioButtons[0]->isChecked())
        {

            this->sendBuff.append(0x8f);
            this->sendBuff.append(0x1f);
            float ref = (float)this->ctrlSpinBox[0]->value();
            emit synTarget(ref,1);
            uint8_t floatarr[4];
            *(float *)floatarr = ref;
            this->sendBuff.append(floatarr[3]);
            this->sendBuff.append(floatarr[2]);
            this->sendBuff.append(floatarr[1]);
            this->sendBuff.append(floatarr[0]);
            emit xTxMessage(this->sendBuff);
        }
        else if(this->radioButtons[1]->isChecked())
        {
            this->sendBuff.append(0x8f);
            this->sendBuff.append(0x2f);
            float ref = (float)this->ctrlSpinBox[1]->value();
            emit synTarget(ref,2);
            uint8_t floatarr[4];
            *(float *)floatarr = ref;
            this->sendBuff.append(floatarr[3]);
            this->sendBuff.append(floatarr[2]);
            this->sendBuff.append(floatarr[1]);
            this->sendBuff.append(floatarr[0]);
            emit xTxMessage(this->sendBuff);
        }
        else if(this->radioButtons[2]->isChecked())
        {
            this->sendBuff.append(0x8f);
            this->sendBuff.append(0x3f);
            float ref = (float)this->ctrlSpinBox[2]->value();
            emit synTarget(ref,3);
            uint8_t floatarr[4];
            *(float *)floatarr = ref;
            this->sendBuff.append(floatarr[3]);
            this->sendBuff.append(floatarr[2]);
            this->sendBuff.append(floatarr[1]);
            this->sendBuff.append(floatarr[0]);
            emit xTxMessage(this->sendBuff);
        }
        for(int i=0;i<4;i++)
        {
            if(this->modeButton[i]->isEnabled()){this->modeButton[i]->setEnabled(false);}
        }

    }
    else
    {
        for(int i=0;i<4;i++)
        {
            if(!this->modeButton[i]->isEnabled()){this->modeButton[i]->setEnabled(true);}
        }
        this->sendBuff.clear();
        this->sendBuff.append(0x9f);
        this->sendBuff.append(0xff);
        emit xTxMessage(this->sendBuff);
        qDebug("stop");
    }
}



void xQObjectTx::ControlValueChange(double value)
{
    if(this->RunButton->isChecked())
    {
        this->RunButtonToggled(true);
    }
    qDebug()<<"ControlValueChange";
}

TxThread::TxThread(QObject *parent) : QThread(parent)
{

}


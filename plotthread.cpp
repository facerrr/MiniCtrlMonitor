#include "plotthread.h"

xQCustomPlot::xQCustomPlot(QObject *parent) : QObject(parent)
{

}

xQCustomPlot::~xQCustomPlot(void)
{
    this->TimerStop();
};


//同步地址，前三个checkbox控制数据可见性，最后一个为使能x轴可控
void xQCustomPlot::setQCheckBoxAddr(QCheckBox *addr1,QCheckBox *addr2,QCheckBox *addr3,QCheckBox *addr4)
{
    if(this->windowNum < 2)
    {
        this->qCheckBoxs[0] = addr1;
        this->qCheckBoxs[1] = addr2;
    }
    else
    {
        this->qCheckBoxs[0] = addr1;
        this->qCheckBoxs[1] = addr2;
        this->qCheckBoxs[2] = addr3;
    }
    this->scaleCheckBox = addr4;
}

void xQCustomPlot::setQSpinBoxAddr(QSpinBox *addr)
{
    this->qSpinbox = addr;
    this->qSpinbox->setKeyboardTracking(false);
}

void xQCustomPlot::setQSliderAddr(QAbstractSlider *addr)
{
    this->qAbstractSlider = addr;
}

void xQCustomPlot::setQCustomAddr(QCustomPlot *addr)
{
    this->qCusPlot = addr;
    this->qCusPlot->yAxis->setRange(-10,10);
    this->qCusPlot->legend->setVisible(true);
    QPen pen;
    switch (this->windowNum)
    {
        case 0:
            this->pGraph1 = this->qCusPlot->addGraph();
            this->pGraph1->setName("angle");
            this->pGraph2 = this->qCusPlot->addGraph();
            pen.setColor(Qt::red);
            this->pGraph2->setName("angle_target");
            this->pGraph2->setPen(pen);
            break;
        case 1:
            this->pGraph1 = this->qCusPlot->addGraph();
            this->pGraph1->setName("velocity");
            this->pGraph2 = this->qCusPlot->addGraph();
            pen.setColor(Qt::red);
            this->pGraph2->setName("vel_target");
            this->pGraph2->setPen(pen);
            break;
        case 2:
            this->pGraph1 = this->qCusPlot->addGraph();
            this->pGraph1->setPen(pen);
            this->pGraph2 = this->qCusPlot->addGraph();
            pen.setColor(Qt::red);
            this->pGraph2->setPen(pen);
            this->pGraph3 = this->qCusPlot->addGraph();
            pen.setColor(Qt::blue);
            this->pGraph3->setPen(pen);
            this->pGraph1->setName("ia");
            this->pGraph2->setName("ib");
            this->pGraph3->setName("ib");
            break;
        case 3:
            this->pGraph1 = this->qCusPlot->addGraph();
            this->pGraph1->setPen(pen);
            this->pGraph2 = this->qCusPlot->addGraph();
            pen.setColor(Qt::blue);
            this->pGraph2->setPen(pen);
            this->pGraph3 = this->qCusPlot->addGraph();
            pen.setColor(Qt::red);
            this->pGraph3->setPen(pen);
            this->pGraph1->setName("Id");
            this->pGraph2->setName("Iq");
            this->pGraph3->setName("Iq_target");
    }
    this->qCusPlot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables);
    this->TimerCfg = 100;
    this->plotTimer.setTimerType(Qt::PreciseTimer);
    connect(&this->plotTimer,&QTimer::timeout,
                this,&xQCustomPlot::xTimerOut);
    this->isGraphInitialized = true;
    this->target.angle_target = 3.14;
    this->target.vel_target = 0;
    this->target.iq_traget = 0;
}

void xQCustomPlot::TimerStart(void)
{
    this->TimerEnable = true;
    if(!this->plotTimer.isActive())
    {
        this->plotTimer.start(this->TimerCfg);
    }
}
//1.不是当前页面 2.plotEnable = false
void xQCustomPlot::TimerStop(void)
{
    this->TimerEnable = false;
    if(this->plotTimer.isActive())
    {
        this->plotTimer.stop();
    }
}

void xQCustomPlot::setWindowNum(uint8_t num)
{
    this->windowNum = num;
}


void xQCustomPlot::initialConection(void)
{
    connect(this->qCheckBoxs[0],SIGNAL(stateChanged(int)),this,SLOT(graph1ViewEnable(int)));
    connect(this->qCheckBoxs[1],SIGNAL(stateChanged(int)),this,SLOT(graph2ViewEnable(int)));
    if(this->qCheckBoxs[2]!=nullptr)
    {
        connect(this->qCheckBoxs[2],SIGNAL(stateChanged(int)),this,SLOT(graph2ViewEnable(int)));
    }
    connect(this->qAbstractSlider,SIGNAL(valueChanged(int)),this->qSpinbox,SLOT(setValue(int)));
    connect(this->qAbstractSlider,SIGNAL(valueChanged(int)),this,SLOT(setScale(int)));
    connect(this->qSpinbox,SIGNAL(valueChanged(int)),this->qAbstractSlider,SLOT(setValue(int)));
    connect(this->scaleCheckBox,SIGNAL(stateChanged(int)),this,SLOT(setScaleAdjustable(int)));
}

void xQCustomPlot::update(const QByteArray &str){
    if(this->plotEnable && this->graphEnable){
        QByteArray tmpQB = str;
        uint8_t frame = str.length()/32;
        for(uint8_t i=0;i<frame;i++)
        {
            if(this->windowNum == 0)
            {
                QByteArray data1 = tmpQB.mid(1,5);
                memcpy(&this->rxData.position,data1,4);
                this->pGraph1->addData(rxCnt,this->rxData.position);
                this->pGraph2->addData(rxCnt,this->target.angle_target);
            }
            else if(this->windowNum == 1)
            {
                QByteArray data1 = tmpQB.mid(5,9);
                memcpy(&this->rxData.velocity,data1,4);
                this->pGraph1->addData(rxCnt,this->rxData.velocity);
                this->pGraph2->addData(rxCnt,this->target.vel_target);
            }
            else if(this->windowNum == 2)
            {
                QByteArray data1 = tmpQB.mid(9,13);
                QByteArray data2 = tmpQB.mid(13,17);
                QByteArray data3 = tmpQB.mid(17,21);
                memcpy(&this->rxData.ia,data1,4);
                memcpy(&this->rxData.ib,data2,4);
                memcpy(&this->rxData.ic,data3,4);
                this->pGraph1->addData(rxCnt,this->rxData.ia);
                this->pGraph2->addData(rxCnt,this->rxData.ib);
                this->pGraph3->addData(rxCnt,this->rxData.ic);
            }
            else
            {
                QByteArray data1 = tmpQB.mid(21,25);
                QByteArray data2 = tmpQB.mid(25,29);
                memcpy(&this->rxData.id,data1,4);
                memcpy(&this->rxData.iq,data2,4);
                this->pGraph1->addData(rxCnt,this->rxData.id);
                this->pGraph2->addData(rxCnt,this->rxData.iq);
                this->pGraph3->addData(rxCnt,this->target.iq_traget);
            }
            tmpQB.remove(0,32);
            this->rxCnt++;
        }
    }
}

void xQCustomPlot::clearGraph(void)
{
    this->rxCnt = 0;
    if(this->windowNum<2)
    {
        if(this->isGraphInitialized){
        this->pGraph1->data().data()->clear();
        this->pGraph2->data().data()->clear();
        }
    }
    else
    {
        if(this->isGraphInitialized)
        {
            this->pGraph1->data().data()->clear();
            this->pGraph2->data().data()->clear();
            this->pGraph3->data().data()->clear();
        }
    }
}

void xQCustomPlot::xTimerOut(void)
{
    this->qCusPlot->rescaleAxes();
    double rangeMargin = 0.3 * (this->qCusPlot->yAxis->range().upper - this->qCusPlot->yAxis->range().lower);
    this->qCusPlot->yAxis->setRange(this->qCusPlot->yAxis->range().lower-rangeMargin, this->qCusPlot->yAxis->range().upper + rangeMargin);
    this->qCusPlot->xAxis->setRange((this->rxCnt>this->xscale)?(this->rxCnt-this->xscale):0, this->rxCnt);
    this->qCusPlot->replot(QCustomPlot::rpQueuedReplot);
}

void xQCustomPlot::tabChange(int index)
{
    if(index == this->windowNum)
    {
        this->clearGraph();
        this->graphEnable = true;
    }
    else
    {
        this->graphEnable = false;
    }
}


void xQCustomPlot::graph1ViewEnable(int enbale)
{
    if(this->qCheckBoxs[0]!=nullptr)
    {
        if(enbale)
        {
            this->pGraph1->setVisible(true);
            this->graph1Viewable = true;
            qDebug()<<"setVisible";
        }
        else
        {
            this->pGraph1->setVisible(false);
            this->graph1Viewable = false;
        }
    }
}

void xQCustomPlot::graph2ViewEnable(int enbale)
{
    if(this->qCheckBoxs[1]!=nullptr)
    {
        if(enbale)
        {
            this->pGraph2->setVisible(true);
            this->graph2Viewable = true;
        }
        else
        {
            this->pGraph2->setVisible(false);
            this->graph2Viewable = false;
        }
    }
}
void xQCustomPlot::graph3ViewEnable(int enbale)
{
    if(this->qCheckBoxs[2]!=nullptr)
    {
        if(enbale)
        {
            this->pGraph3->setVisible(true);
            this->graph3Viewable = true;
        }
        else
        {
            this->pGraph3->setVisible(false);
            this->graph3Viewable = false;
        }
    }
}

void xQCustomPlot::setScaleAdjustable(int enable)
{
    if(enable)
    {
        this->qSpinbox->setEnabled(true);
        this->qAbstractSlider->setEnabled(true);
    }
    else
    {
        this->qSpinbox->setEnabled(false);
        this->qAbstractSlider->setEnabled(false);
    }
}

void xQCustomPlot::setScale(int num)
{
    this->xscale = num;
}

void xQCustomPlot::setTarget(const float value,const int index)
{
    switch (index) {
        case 1:
            this->target.iq_traget = value;
            break;
        case 2:
            this->target.vel_target = value;
            break;
        case 3:
            this->target.angle_target = value;
            break;
    }
}

PlotThread::PlotThread(QObject *parent) : QThread(parent)
{

}






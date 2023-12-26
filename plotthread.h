#ifndef PLOTTHREAD_H
#define PLOTTHREAD_H

#include <qcustomplot.h>
#include <QObject>
#include <QThread>
#include <QDebug>
#include <QTimer>
#include <QVector>

typedef struct
{
   float position;
   float velocity;
   float ia;
   float ib;
   float ic;
   float id;
   float iq;
}qRxData;

typedef struct
{
    float angle_target;
    float vel_target;
    float iq_traget;
}targetS;

class xQCustomPlot : public QObject
{
    Q_OBJECT
public:
    explicit xQCustomPlot(QObject *parent = nullptr);
    ~xQCustomPlot(void);
    uint8_t windowNum;  // 0:位置窗口  1：速度窗口  2：三相电流窗口  3：dq轴电流窗口
    int64_t rxCnt = 0;  //接收计数
    int xscale = 20000; //x轴可见区域范围，可以交互控制
    bool plotEnable = false;  //通过控制PLot按钮改变值
    bool graphEnable = false; //用于判断是否为当前窗口，当前绘图，其他窗口关闭，减少绘图负担
    bool TimerEnable; // 绘图定时器使能
    bool isGraphInitialized = false;  //用于判断Graph是否初始化，以免出现空指针错误
    bool graph1Viewable = false;
    bool graph2Viewable = false;
    bool graph3Viewable = false;
    qRxData rxData; //绘图数据
    targetS target;
    QTimer plotTimer;   // 绘图定时器
    qint32 TimerCfg;  //定时器配置，也就多久刷新
    QCPGraph *pGraph1;
    QCPGraph *pGraph2;
    QCPGraph *pGraph3;

    QCustomPlot *qCusPlot = nullptr;  //在主函数中配置地址
    QSpinBox *qSpinbox = nullptr;  //控制x轴可见范围
    QAbstractSlider *qAbstractSlider = nullptr;
    QCheckBox *qCheckBoxs[3] = {nullptr};   //控制哪些数据可见
    QCheckBox *scaleCheckBox = nullptr;  //x轴范围可控性
    void TimerStart(void);
    void TimerStop(void);
    void setQCustomAddr(QCustomPlot *addr);
    void setQCheckBoxAddr(QCheckBox *addr1,QCheckBox *addr2,QCheckBox *addr3,QCheckBox *addr4);
    void setQSpinBoxAddr(QSpinBox *addr);
    void setQSliderAddr(QAbstractSlider *addr);
    void setWindowNum(uint8_t num);
    void initialConection(void);

public slots:
    void update(const QByteArray &str); // 在rxthread中触发
    void clearGraph(void);  //清楚绘图数据
    void xTimerOut(void);  //定时器中断
    void tabChange(int index);  //检测页面是否切换
    void graph1ViewEnable(int enbale);
    void graph2ViewEnable(int enbale);
    void graph3ViewEnable(int enbale);
    void setScaleAdjustable(int enable);
    void setScale(int num);
    void setTarget(const float value,const int index);
};



class PlotThread : public QThread
{
    Q_OBJECT
public:
    explicit PlotThread(QObject *parent = nullptr);
public:
    void run(void)
    {
        qDebug()<<"main tid:"<<this->objectName()<<"run"<<QThread::currentThreadId();
        exec();
    }
signals:
public slots:
};

#endif // PLOTTHREAD_H

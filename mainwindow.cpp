#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setWindowIcon(QIcon(":/img/MiniCtrl.png"));
    InitPlainText();
    InitTabWidget();
    InitTreeWidget();
    InitDetection();
    UpdateComInfo();
    UpdateSerial();
    InitSerialRx();
    InfoChangedInit();
    QPlotInit();
    PlainTextTimerCfg();
    UpdateShow();
    qDebug()<<"main tid:MainWindow"<< QThread::currentThreadId();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::WindownInit(void)
{
    static const GUID GUID_DEVINTERFACE_LIST[] =
    {
    {0xA5DCBF10, 0x6530, 0x11D2, { 0x90, 0x1F, 0x00, 0xC0, 0x4F, 0xB9, 0x51, 0xED } },
    };
    //注册插拔事件
    HDEVNOTIFY hDevNotify;
    DEV_BROADCAST_DEVICEINTERFACE NotifacationFiler;
    ZeroMemory(&NotifacationFiler,sizeof(DEV_BROADCAST_DEVICEINTERFACE));
    NotifacationFiler.dbcc_size = sizeof(DEV_BROADCAST_DEVICEINTERFACE);
    NotifacationFiler.dbcc_devicetype = DBT_DEVTYP_DEVICEINTERFACE;
    for(unsigned int i=0;i<sizeof(GUID_DEVINTERFACE_LIST)/sizeof(GUID);i++)
    {
        NotifacationFiler.dbcc_classguid = GUID_DEVINTERFACE_LIST[i];
        //GetCurrentUSBGUID();
        hDevNotify = RegisterDeviceNotification(HANDLE(this->winId()),&NotifacationFiler,DEVICE_NOTIFY_WINDOW_HANDLE);
        if(!hDevNotify)
        {
            GetLastError();
        }
    }
}

void MainWindow::InitPlainText(void)
{
    ui->plainTextRx->SetShowBuffAddr(&this->xMonitor.xSerial.xSerialData->RxBuff);
    ui->plainTextRx->setHexEnableAddr(&this->xMonitor.xSerial.xSerialData->rxHexEnable);
}

void MainWindow::InitTabWidget(void)
{
    ui->tabWidget->setCurrentIndex(0);

    this->xMonitor.xQCustPlots[0].setQCustomAddr(ui->widgetPlot1);
    this->xMonitor.xQCustPlots[1].setQCustomAddr(ui->widgetPlot2);
    this->xMonitor.xQCustPlots[2].setQCustomAddr(ui->widgetPlot3);
    this->xMonitor.xQCustPlots[3].setQCustomAddr(ui->widgetPlot4);
    this->xMonitor.xQCustPlots[0].setQCheckBoxAddr(ui->checkboxData1_1,ui->checkboxData1_2,nullptr,ui->xScale_1);
    this->xMonitor.xQCustPlots[1].setQCheckBoxAddr(ui->checkboxData2_1,ui->checkboxData2_2,nullptr,ui->xScale_2);
    this->xMonitor.xQCustPlots[2].setQCheckBoxAddr(ui->checkboxData3_1,ui->checkboxData3_2,ui->checkboxData3_3,ui->xScale_3);
    this->xMonitor.xQCustPlots[3].setQCheckBoxAddr(ui->checkboxData4_1,ui->checkboxData4_2,ui->checkboxData4_3,ui->xScale_4);
    this->xMonitor.xQCustPlots[0].setQSpinBoxAddr(ui->xScale_spinBox1);
    this->xMonitor.xQCustPlots[1].setQSpinBoxAddr(ui->xScale_spinBox2);
    this->xMonitor.xQCustPlots[2].setQSpinBoxAddr(ui->xScale_spinBox3);
    this->xMonitor.xQCustPlots[3].setQSpinBoxAddr(ui->xScale_spinBox4);
    this->xMonitor.xQCustPlots[0].setQSliderAddr(ui->horizontalSlider_1);
    this->xMonitor.xQCustPlots[1].setQSliderAddr(ui->horizontalSlider_2);
    this->xMonitor.xQCustPlots[2].setQSliderAddr(ui->horizontalSlider_3);
    this->xMonitor.xQCustPlots[3].setQSliderAddr(ui->horizontalSlider_4);
    for(int i=0;i<4;i++)
    {
        this->xMonitor.xQCustPlots[i].initialConection();
    }

    //连接页面变化的信号和槽
    for(uint8_t i=0;i<4;i++)
    {
        connect(ui->tabWidget,SIGNAL(currentChanged(int)),&this->xMonitor.xQCustPlots[i],SLOT(tabChange(int)));
    }
    connect(ui->tabWidget,SIGNAL(currentChanged(int)),this,SLOT(TabChange(int)));
    //初始化滑条和spinbox的范围，也就是x轴的可选范围
    ui->xScale_spinBox1->setRange(500,50000);
    ui->xScale_spinBox2->setRange(500,50000);
    ui->xScale_spinBox3->setRange(500,50000);
    ui->xScale_spinBox4->setRange(500,50000);
    ui->horizontalSlider_1->setRange(500,50000);
    ui->horizontalSlider_2->setRange(500,50000);
    ui->horizontalSlider_3->setRange(500,50000);
    ui->horizontalSlider_4->setRange(500,50000);
}

void MainWindow::InitTreeWidget(void)
{
    QDoubleSpinBox *addrs[14] = {nullptr};
    ui->treeWidget->setHeaderLabels(QStringList()<<"key"<<"value");
    QTreeWidgetItem *Global=new QTreeWidgetItem(QStringList()<<"控制器设置");
    QTreeWidgetItem* PosiLoop=new QTreeWidgetItem(QStringList()<<"位置环");
    Global->addChild(PosiLoop);
    ui->treeWidget->addTopLevelItem(Global);
    //位置环
    QTreeWidgetItem* PosiPGain=new QTreeWidgetItem(QStringList()<<"PGain");
    PosiLoop->addChild(PosiPGain);
    QTreeWidgetItem* pOutPutMax=new QTreeWidgetItem(QStringList()<<"OutPutMax");
    PosiLoop->addChild(pOutPutMax);
    QDoubleSpinBox *PosiPGainIn = new QDoubleSpinBox();
    addrs[0] = PosiPGainIn;
    QDoubleSpinBox *pOutPutMaxIn = new QDoubleSpinBox();
    addrs[1] = pOutPutMaxIn;
    ui->treeWidget->setItemWidget(PosiPGain,1,PosiPGainIn);
    ui->treeWidget->setItemWidget(pOutPutMax,1,pOutPutMaxIn);

    //速度环
    QTreeWidgetItem* SpeedLoop=new QTreeWidgetItem(QStringList()<<"速度环");
    Global->addChild(SpeedLoop);
    QTreeWidgetItem* SpeedPGain=new QTreeWidgetItem(QStringList()<<"PGain");
    SpeedLoop->addChild(SpeedPGain);
    QTreeWidgetItem* SpeedIGain=new QTreeWidgetItem(QStringList()<<"IGain");
    SpeedLoop->addChild(SpeedIGain);
    QTreeWidgetItem* SpeedIMax=new QTreeWidgetItem(QStringList()<<"IMax");
    SpeedLoop->addChild(SpeedIMax);
    QTreeWidgetItem* vOutPutMax=new QTreeWidgetItem(QStringList()<<"OutPutMax");
    SpeedLoop->addChild(vOutPutMax);
    QDoubleSpinBox *SpeedPGainIn = new QDoubleSpinBox();
    addrs[2] = SpeedPGainIn;
    QDoubleSpinBox *SpeedIGainIn = new QDoubleSpinBox();
    addrs[3] = SpeedIGainIn;
    QDoubleSpinBox *SpeedIMaxIn = new QDoubleSpinBox();
    addrs[4] = SpeedIMaxIn;
    QDoubleSpinBox *vOutPutMaxIn = new QDoubleSpinBox();
    addrs[5] = vOutPutMaxIn;
    ui->treeWidget->setItemWidget(SpeedPGain,1,SpeedPGainIn);
    ui->treeWidget->setItemWidget(SpeedIGain,1,SpeedIGainIn);
    ui->treeWidget->setItemWidget(SpeedIMax,1,SpeedIMaxIn);
    ui->treeWidget->setItemWidget(vOutPutMax,1,vOutPutMaxIn);

    //电流环
    QTreeWidgetItem* CurrentLoop=new QTreeWidgetItem(QStringList()<<"电流环");
    Global->addChild(CurrentLoop);
    QTreeWidgetItem* CurrentPGain=new QTreeWidgetItem(QStringList()<<"PGain");
    CurrentLoop->addChild(CurrentPGain);
    QTreeWidgetItem* CurrentIGain=new QTreeWidgetItem(QStringList()<<"IGain");
    CurrentLoop->addChild(CurrentIGain);
    QTreeWidgetItem* CurrentIMax=new QTreeWidgetItem(QStringList()<<"IMax");
    CurrentLoop->addChild(CurrentIMax);
    QTreeWidgetItem* iOutPutMax=new QTreeWidgetItem(QStringList()<<"OutPutMax");
    CurrentLoop->addChild(iOutPutMax);
    QDoubleSpinBox *CurrentPGainIn = new QDoubleSpinBox();
    addrs[6] = CurrentPGainIn;
    QDoubleSpinBox *CurrentIGainIn = new QDoubleSpinBox();
    addrs[7] = CurrentIGainIn;
    QDoubleSpinBox *CurrentIMaxIn = new QDoubleSpinBox();
    addrs[8] = CurrentIMaxIn;
    QDoubleSpinBox *iOutPutMaxIn = new QDoubleSpinBox();
    addrs[9] = iOutPutMaxIn;
    ui->treeWidget->setItemWidget(CurrentPGain,1,CurrentPGainIn);
    ui->treeWidget->setItemWidget(CurrentIGain,1,CurrentIGainIn);
    ui->treeWidget->setItemWidget(CurrentIMax,1,CurrentIMaxIn);
    ui->treeWidget->setItemWidget(iOutPutMax,1,iOutPutMaxIn);

    //电机常规配置
    QTreeWidgetItem *Motor=new QTreeWidgetItem(QStringList()<<"电机参数配置");
    ui->treeWidget->addTopLevelItem(Motor);
    //母线电压
    QTreeWidgetItem* Vdc=new QTreeWidgetItem(QStringList()<<"母线电压");
    Motor->addChild(Vdc);
    QDoubleSpinBox *VdcIn = new QDoubleSpinBox();
    addrs[10] = VdcIn;
    VdcIn->setSuffix(" V");
    ui->treeWidget->setItemWidget(Vdc,1,VdcIn);
    //电机极对数
    QTreeWidgetItem* poleNum=new QTreeWidgetItem(QStringList()<<"电机极对数");
    Motor->addChild(poleNum);
    QDoubleSpinBox *poleNumIn = new QDoubleSpinBox();
    addrs[11] = poleNumIn;
    ui->treeWidget->setItemWidget(poleNum,1,poleNumIn);
    //相电阻
    QTreeWidgetItem* resis=new QTreeWidgetItem(QStringList()<<"相电阻");
    Motor->addChild(resis);
    QDoubleSpinBox *resisIn = new QDoubleSpinBox();
    addrs[12] = resisIn;
    resisIn->setSuffix(" R");
    ui->treeWidget->setItemWidget(resis,1,resisIn);
    //相电感
    QTreeWidgetItem* inductance=new QTreeWidgetItem(QStringList()<<"相电感");
    Motor->addChild(inductance);
    QDoubleSpinBox *inductanceIn = new QDoubleSpinBox();
    addrs[13] = inductanceIn;
    inductanceIn->setSuffix(" uH");
    ui->treeWidget->setItemWidget(inductance,1,inductanceIn);
    this->xMonitor.xQObjectTxCtr.setPmSpinBoxAddr(addrs);
    this->xMonitor.xQObjectRxCtr.setPmSpinBoxAddr(addrs);
    this->xMonitor.xQObjectTxCtr.setRunButtonAddr(ui->pushButtonRun);
    this->xMonitor.xQObjectTxCtr.setModeButtonsAddr(ui->pushButtonSynPm,ui->pushButtonUpload,ui->pushButtonCalibrate,ui->pushButton_4);
    this->xMonitor.xQObjectTxCtr.setRadioButtonsAddr(ui->tRadioButton,ui->vRadioButton,ui->pRadioButton);
    this->xMonitor.xQObjectTxCtr.setCtrlSpinBoxAddr(ui->torqueSpinBox,ui->velocitySpinBox,ui->positionSpinBox);
    this->xMonitor.xQObjectTxCtr.InitConnection();
    ui->treeWidget->expandAll();
}

void MainWindow::InitDetection(void)
{
    WindownInit();
    connect(ui->pushButtonOpen,&QPushButton::released,this,&MainWindow::SerialOpen);
    connect(this,&MainWindow::openSerial,&this->xMonitor.xSerial,&xSerialPort::xSerialOpen,Qt::BlockingQueuedConnection);
    connect(this,&MainWindow::closeSerial,&this->xMonitor.xSerial,&xSerialPort::xSerialClose,Qt::QueuedConnection);
//    connect(&this->vSerialCtr.vSerial,&vSerialPort::vTxError,this,&MainWindow::vTxError);
}

void MainWindow::UpdateComInfo(void)
{
    ui->comboBoxCom2->addItem("2000000",int(2000000));
    ui->comboBoxCom2->addItem("1382400",int(1382400));
    ui->comboBoxCom2->addItem("921600",int(921600));
    ui->comboBoxCom2->addItem("460800",int(460800));
    ui->comboBoxCom2->addItem("256000",int(256000));
    ui->comboBoxCom2->addItem("230400",int(230400));
    ui->comboBoxCom2->addItem("128000",int(128000));
    ui->comboBoxCom2->addItem("115200",int(115200));
    ui->comboBoxCom2->addItem("76800",int(76800));
    ui->comboBoxCom2->addItem("57600",int(57600));
    ui->comboBoxCom2->addItem("43000",int(43000));
    ui->comboBoxCom2->addItem("38400",int(38400));
    ui->comboBoxCom2->addItem("19200",int(19200));
    ui->comboBoxCom2->addItem("14400",int(14400));
    ui->comboBoxCom2->addItem("9600",int(9600));
    ui->comboBoxCom2->addItem("4800",int(4800));
    ui->comboBoxCom2->addItem("2400",int(2400));
    ui->comboBoxCom2->addItem("1200",int(1200));
    ui->comboBoxCom2->setCurrentText("115200");

    ui->comboBoxCom3->addItem("1",QSerialPort::StopBits(QSerialPort::OneStop));
    ui->comboBoxCom3->addItem("1.5",QSerialPort::StopBits(QSerialPort::OneAndHalfStop));
    ui->comboBoxCom3->addItem("2",QSerialPort::StopBits(QSerialPort::TwoStop));

    ui->comboBoxCom4->addItem("8",QSerialPort::DataBits(QSerialPort::Data8));
    ui->comboBoxCom4->addItem("7",QSerialPort::DataBits(QSerialPort::Data7));
    ui->comboBoxCom4->addItem("6",QSerialPort::DataBits(QSerialPort::Data6));
    ui->comboBoxCom4->addItem("5",QSerialPort::DataBits(QSerialPort::Data5));

    ui->comboBoxCom5->addItem(QString::fromLocal8Bit("None"),QSerialPort::Parity(QSerialPort::NoParity));
    ui->comboBoxCom5->addItem(QString::fromLocal8Bit("EvenParity"),QSerialPort::Parity(QSerialPort::EvenParity));
    ui->comboBoxCom5->addItem(QString::fromLocal8Bit("OddParity"),QSerialPort::Parity(QSerialPort::OddParity));

    ui->comboBoxCom6->addItem("NoFlowControl",QSerialPort::Parity(QSerialPort::NoFlowControl));
    ui->comboBoxCom6->addItem("RTS/CTS",QSerialPort::Parity(QSerialPort::HardwareControl));
    ui->comboBoxCom6->addItem("XON/XOFF",QSerialPort::Parity(QSerialPort::SoftwareControl));
}

void MainWindow::UpdateSerial(void)
{
    bool haveIt = false;
    static qint16 index;//记录上次打开选中
    this->xMonitor.xSerial.xSerialComScanf();
    ui->comboBoxCom1->clear();
    haveIt = false;
    index  = 0;
    if(this->xMonitor.xSerial.xSerialPortCnt!=0)
    {
        for(qint16 i=0;i<this->xMonitor.xSerial.xSerialPortCnt;i++)
        {
            QString str ="("
                    +this->xMonitor.xSerial.xSerialComInfo[i].SerialName
                    +")"
                    +this->xMonitor.xSerial.xSerialComInfo[i].DescribeStr;
            ui->comboBoxCom1->addItem(str,
                this->xMonitor.xSerial.xSerialComInfo[i].SerialName);

            if(this->xMonitor.xSerial.xSerialComInfo[i].SerialName ==
               this->xMonitor.xSerial.xSerialConfig->xSerialPortName)
            {
                haveIt = true;
                index = i;
            }
        }
        //检测上次打开的串口是否存在
        if(haveIt != false)
        {
            ui->comboBoxCom1->setCurrentIndex(index);
        }
        else
        {
            //如果之前串口为打开状态，在串口拔出时，串口应变为关闭，因此按钮需要同步
            this->SerialClose();
            this->xMonitor.xSerial.xSerialConfig->xSerialPortName =
                    ui->comboBoxCom1->currentData().toString();
        }
    }
    else
    {
        this->SerialClose();
        ui->comboBoxCom1->clear();
        this->xMonitor.xSerial.xSerialConfig->xSerialPortName =" ";
    }
}

void MainWindow::InitSerialRx(void)
{
    connect(ui->pushButtonRx2,&QPushButton::released,ui->plainTextRx,&vPlainTextEdit::clearBuff);
    connect(ui->checkBoxRx1,&QCheckBox::released,this,&MainWindow::RxHexEnableCfg);
    connect(ui->checkBoxRx2,&QCheckBox::released,this,&MainWindow::RxTimerStampChanged);
    this->xMonitor.xQObjectRxCtr.setQLabelAddr(ui->labelInfo);
    this->xMonitor.xQObjectTxCtr.setQLabelAddr(ui->labelInfo);
}

void MainWindow::InfoChangedInit(void)
{
    this->xMonitor.xSerial.xSerialConfig->xSerialBaudRate = ui->comboBoxCom2->currentData().toInt();
    connect(ui->comboBoxCom2,&QComboBox::currentTextChanged,[=]()
    {
        this->xMonitor.xSerial.xSerialConfig->xSerialBaudRate = ui->comboBoxCom2->currentData().toInt();
    });
    this->xMonitor.xSerial.xSerialConfig->xSerialStopBits = QSerialPort::StopBits(ui->comboBoxCom3->currentData().toInt());
    connect(ui->comboBoxCom3,&QComboBox::currentTextChanged,[=]()
    {
        this->xMonitor.xSerial.xSerialConfig->xSerialStopBits = QSerialPort::StopBits(ui->comboBoxCom3->currentData().toInt());
    });
    this->xMonitor.xSerial.xSerialConfig->xSerialDataBits = QSerialPort::DataBits(ui->comboBoxCom4->currentData().toInt());
    connect(ui->comboBoxCom4,&QComboBox::currentTextChanged,[=]()
    {
        this->xMonitor.xSerial.xSerialConfig->xSerialDataBits = QSerialPort::DataBits(ui->comboBoxCom4->currentData().toInt());
    });
    this->xMonitor.xSerial.xSerialConfig->xSerialParrity  = QSerialPort::Parity(ui->comboBoxCom5->currentData().toInt());
    connect(ui->comboBoxCom5,&QComboBox::currentTextChanged,[=]()
    {
        this->xMonitor.xSerial.xSerialConfig->xSerialParrity  = QSerialPort::Parity(ui->comboBoxCom5->currentData().toInt());
    });
}

void MainWindow::PlainTextTimerCfg(void)
{
    ui->plainTextRx->TimerStop();

    ui->plainTextRx->setTimerCfg(100);

    ui->plainTextRx->TimerStart();
}

void MainWindow::QPlotInit(void)
{
    ui->widgetPlot1->setObjectName("Position");
//    ui->vQPlotwidget->moveToThread(&this->vSerialCtr.vQPlotThread);
    connect(&this->xMonitor.xQObjectRxCtr,SIGNAL(graphUpdate(const QByteArray)),&this->xMonitor.xQCustPlots[0],SLOT(update(const QByteArray)));
    ui->widgetPlot1->setObjectName("Velocity");
    connect(&this->xMonitor.xQObjectRxCtr,SIGNAL(graphUpdate(const QByteArray)),&this->xMonitor.xQCustPlots[1],SLOT(update(const QByteArray)));
    ui->widgetPlot1->setObjectName("Current");
    connect(&this->xMonitor.xQObjectRxCtr,SIGNAL(graphUpdate(const QByteArray)),&this->xMonitor.xQCustPlots[2],SLOT(update(const QByteArray)));
    ui->widgetPlot1->setObjectName("Current_DQ");
    connect(&this->xMonitor.xQObjectRxCtr,SIGNAL(graphUpdate(const QByteArray)),&this->xMonitor.xQCustPlots[3],SLOT(update(const QByteArray)));

    connect(ui->pushButtonPlot,&QPushButton::released,this,&MainWindow::PlotEnableCfg);
    connect(&this->xMonitor.xQObjectTxCtr,SIGNAL(synTarget(const float,const int)),&this->xMonitor.xQCustPlots[0],SLOT(setTarget(const float,const int)));
    connect(&this->xMonitor.xQObjectTxCtr,SIGNAL(synTarget(const float,const int)),&this->xMonitor.xQCustPlots[1],SLOT(setTarget(const float,const int)));
    connect(&this->xMonitor.xQObjectTxCtr,SIGNAL(synTarget(const float,const int)),&this->xMonitor.xQCustPlots[2],SLOT(setTarget(const float,const int)));
    connect(&this->xMonitor.xQObjectTxCtr,SIGNAL(synTarget(const float,const int)),&this->xMonitor.xQCustPlots[3],SLOT(setTarget(const float,const int)));
}

void MainWindow::ReadSerialSelect(void)
{
    this->xMonitor.xSerial.xSerialConfig->xSerialPortName = ui->comboBoxCom1->currentData().toString();
    this->xMonitor.xSerial.xSerialConfig->xSerialBaudRate = ui->comboBoxCom2->currentData().toInt();
    this->xMonitor.xSerial.xSerialConfig->xSerialStopBits = QSerialPort::StopBits(ui->comboBoxCom3->currentData().toInt());
    this->xMonitor.xSerial.xSerialConfig->xSerialDataBits = QSerialPort::DataBits(ui->comboBoxCom4->currentData().toInt());
    this->xMonitor.xSerial.xSerialConfig->xSerialParrity  = QSerialPort::Parity(ui->comboBoxCom5->currentData().toInt());
}

void MainWindow::SerialOpen(void)
{
    if(ui->pushButtonOpen->isChecked())
    {
        //读取选择的串口配置
        ReadSerialSelect();
        bool isOpen;
        emit openSerial(isOpen);
        if(isOpen)
        {
            ui->pushButtonOpen->setChecked(true);
            ui->pushButtonPlot->setEnabled(true);
        }
        else
        {
            //打开串口失败
            this->doWarning(QString::fromLocal8Bit("Opening the serial port failed, please check whether the serial port is inserted!"));
            this->SerialClose();
        }
    }
    else
    {
        this->SerialClose();
    }
}

void MainWindow::SerialClose(void)
{

    ui->pushButtonOpen->setChecked(false);
    if(ui->pushButtonRun->isChecked())
    {
        ui->pushButtonRun->setChecked(false);
        emit ui->pushButtonRun->toggled(false);
    }
    if(ui->pushButtonPlot->isChecked())
    {
        ui->pushButtonPlot->setChecked(false);
        this->PlotEnableCfg();
    }
    Sleep(50);

    emit closeSerial();
    ui->labelInfo->setText("Lost Connection With Controller");
}



void MainWindow::RxTimerStampChanged(void)
{
    if(ui->checkBoxRx2->isChecked())
    {
        this->xMonitor.xSerial.xSerialData->rxTimeStampEnable = true;
    }
    else
    {
        this->xMonitor.xSerial.xSerialData->rxTimeStampEnable = false;
    }
}

void MainWindow::RxHexEnableCfg(void)
{
    if(ui->checkBoxRx1->isChecked())
    {
        *ui->plainTextRx->hexEnable = true;
    }
    else
    {
        *ui->plainTextRx->hexEnable = false;
    }
}

void MainWindow::PlotEnableCfg(void)
{
    int current_index = ui->tabWidget->currentIndex();
    if(ui->pushButtonPlot->isChecked())
    {
        for(int i=0;i<4;i++)
        {
            this->xMonitor.xQCustPlots[i].plotEnable = true;
        }
        this->TabChange(current_index);
        this->xMonitor.xQCustPlots[current_index].tabChange(current_index);
    }
    else
    {
        this->xMonitor.xQCustPlots[current_index].TimerStop();
        this->xMonitor.xQCustPlots[current_index].graphEnable = false;
        for(int i=0;i<4;i++)
        {
            this->xMonitor.xQCustPlots[i].plotEnable = false;
        }
    }
}

void MainWindow::UpdateShow(void)
{
    this->xMonitor.xQObjectRxCtr.xDisConnectRx();
    this->xMonitor.xQObjectTxCtr.xDisConnectTx();
    this->xMonitor.xQObjectRxCtr.xConnectRx();
    this->xMonitor.xQObjectTxCtr.xConnectTx();
    ReadSerialSelect();
    //以下函数根据参数变化更新显示
    RxTimerStampChanged();  //刷新接收时间戳使能
    RxHexEnableCfg();       //刷新RxHexEnable
}

void MainWindow::TabChange(int index)
{
    for(int i=0;i<4;i++)
    {
        if(i == index)
        {
            if(this->xMonitor.xQCustPlots[i].plotEnable)
            {
                this->xMonitor.xQCustPlots[i].TimerStart();
            }
        }
        else
        {
            this->xMonitor.xQCustPlots[i].TimerStop();
        }
    }
    switch (index) {
        case 0:
            ui->horizontalSlider_1->setValue(20000);
            ui->xScale_2->setChecked(false);
            ui->xScale_3->setChecked(false);
            ui->xScale_4->setChecked(false);
            ui->xScale_1->setChecked(true);
            break;
        case 1:
            ui->horizontalSlider_2->setValue(20000);
            ui->xScale_1->setChecked(false);
            ui->xScale_3->setChecked(false);
            ui->xScale_4->setChecked(false);
            ui->xScale_2->setChecked(true);
            break;
        case 2:
            ui->horizontalSlider_3->setValue(1000);
            ui->xScale_1->setChecked(false);
            ui->xScale_2->setChecked(false);
            ui->xScale_4->setChecked(false);
            ui->xScale_3->setChecked(true);
            break;
        case 3:
            ui->horizontalSlider_3->setValue(1000);
            ui->xScale_1->setChecked(false);
            ui->xScale_2->setChecked(false);
            ui->xScale_3->setChecked(false);
            ui->xScale_4->setChecked(true);
            break;
    }
}


void MainWindow::doWarning(const QString &str)
{
    QMessageBox::warning(this,QString::fromLocal8Bit("Warning"),str);
}


bool MainWindow::nativeEvent(const QByteArray &eventType, void *message, long *result)
{
    MSG* msg = reinterpret_cast<MSG*>(message);//第一层解算
    UINT msgType = msg->message;
    if(msgType==WM_DEVICECHANGE)
    {
        PDEV_BROADCAST_HDR lpdb = PDEV_BROADCAST_HDR(msg->lParam);//第二层解算
        switch (msg->wParam) {
        case DBT_DEVICEARRIVAL:
            if(lpdb->dbch_devicetype == DBT_DEVTYP_DEVICEINTERFACE)
            {
                PDEV_BROADCAST_DEVICEINTERFACE pDevInf  = PDEV_BROADCAST_DEVICEINTERFACE(lpdb);
                UpdateSerial();
            }
            break;
        case DBT_DEVICEREMOVECOMPLETE:
            if(lpdb->dbch_devicetype == DBT_DEVTYP_DEVICEINTERFACE)
            {
                qDebug("DBT_DEVICEREMOVECOMPLETE");
                if(ui->pushButtonOpen->isChecked())
                {
                    ui->pushButtonOpen->setChecked(false);
                    this->SerialClose();
                }
                PDEV_BROADCAST_DEVICEINTERFACE pDevInf  = PDEV_BROADCAST_DEVICEINTERFACE(lpdb);
                UpdateSerial();
            }
            break;
        }
    }
    return false;
}

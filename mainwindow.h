#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <monictrl.h>
#include <QString>
#include <windows.h>
#include <dbt.h>
#include <devguid.h>
#include <setupapi.h>
#include <initguid.h>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    MoniCtrl xMonitor;
    void WindownInit(void);
    void InitPlainText(void);
    void InitTabWidget(void);
    void InitTreeWidget(void);
    void InitDetection(void);
    void UpdateComInfo(void);
    void UpdateSerial(void);
    void InitSerialRx(void);
    void InfoChangedInit(void);
    void PlainTextTimerCfg(void);
    void ReadSerialSelect(void);
    void QPlotInit(void);
    void SerialOpen(void);      //打开串口
    void SerialClose(void);     //关闭串口
public slots:
    void RxTimerStampChanged(void);
    void RxHexEnableCfg(void);
    void PlotEnableCfg(void);
    void UpdateShow(void);
    void TabChange(int index);
signals:
    void openSerial(bool & isOpen);
    void closeSerial(void);

protected:
    void doWarning(const QString &str);
    bool nativeEvent(const QByteArray & eventType, void * message, long*result);
private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H

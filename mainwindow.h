#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QModbusClient>
#include <QModbusDataUnit>
<<<<<<< HEAD
#include <QModbusRtuSerialMaster>
//#include <QModbusRtuSerialClient>
=======
#include <QModbusRtuSerialMaster> //Qt5
//#include <QModbusRtuSerialClient> //Qt6
>>>>>>> main
#include <QScrollBar>
#include <QMutex>
#include <QMutexLocker>
#include <qcustomplot.h>
#include <QElapsedTimer>
<<<<<<< HEAD

=======
#include <QApplication>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFile>
#include <QTextStream>
#include "mythread.h"
#include "myqlineedit.h"
#include "configuredialog.h"
#include "plotdialog.h"
>>>>>>> main

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    void LogMsg(QString str, bool newLine = true);
    void findSeriesPortDevices();
signals:

public slots:
  void periodicWork();
  void makePlot();
  void TempCheck();

private slots:
    void keyPressEvent(QKeyEvent *key);
    void keyReleaseEvent(QKeyEvent *key);
    QString formatHex(int value, int digit);
    void waitForMSec(int msec);
    void panalOnOff(bool IO);

    void showTime();
    void allowSetNextSV();

    void read(QModbusDataUnit::RegisterType type, quint16 adress, int size);
    void readReady();
    void request(QModbusPdu::FunctionCode code, QByteArray cmd); // no PDU reply

    void askTemperature(bool mute = false);
    void askSetPoint(bool mute = false);
    void askMV(bool mute = false);
    void askMVupper(bool mute = false);
    void askMVlower(bool mute = false);
    void getSetting();
    void setAT(int atFlag);
    void setSV(double SV);
    void writeData();
    void waitTimming();
    bool generateSaveFile();
    bool isIgnore(bool check, double temp);
    bool isViolate(QVector<double> vtemp);
    bool isDrop(double diff);

    void on_pushButton_Connect_clicked();
    void on_pushButton_AskStatus_clicked();
    void on_pushButton_SetSV_clicked();
    void on_pushButton_GetPID_clicked();
    void on_pushButton_Control_clicked();
    void on_comboBox_AT_currentIndexChanged(int index);
    void on_comboBox_Mode_currentIndexChanged(int index);
    void on_comboBox_MemAddress_currentTextChanged(const QString &arg1);
    void on_doubleSpinBox_MVlower_valueChanged(double arg1);
    void on_doubleSpinBox_MVupper_valueChanged(double arg1);

    void on_checkBox_dataSave_toggled(bool checked);
    void on_pushButton_Log_toggled(bool checked);
    void on_spinBox_TempRecordTime_valueChanged(int arg1);
    void on_pushButton_RunStop_toggled(bool checked);

    void on_actionOpen_File_triggered();
    void on_action_Setting_parameters_for_TempCheck_triggered();
    void on_action_Setting_plot_triggered();
    void on_actionHelp_Page_triggered();
    void HelpPicNext();

    void fillDataAndPlot(const QDateTime date, const double PV, const double SV, const double MV);
    void Run();
    void Stop();
    void Quit();
    void setIntervalAskMV();
    void setIntervalAskTemp();
    void setSafeLimit();
    void setNumbers();
    void setIgnoreRange();
    void setParametersTempCheck(bool mute = true);
    void setIgnoreEnable();
    double fillDifference(bool mute = true);

    void setColor(int colorindex = 0);

    double calcMovingAve(QVector<double> vtemp);

private:
    Ui::MainWindow *ui;
    QCustomPlot * plot;
    QModbusRtuSerialMaster * omron;
    MyThread * threadMVcheck_;
    MyThread * threadLog_;
    MyThread * threadTempCheck_;
    QMessageBox * LogMsgBox_;

    QString omronPortName;
    int omronID;

    int msgCount;
    int respondType;
    QDateTime dateStart_;
    QString dateStartStr_;

    double temperature, SV, MV;
    double MVupper, MVlower;
    double tempDecimal;

    bool tempControlOnOff;
    bool tempRecordOnOff;
    bool modbusReady;
    bool comboxEnable;
    bool spinBoxEnable;
    bool muteLog;
    bool statusAskTemp_;
    bool statusAskMV_;
    bool statusAskSetPoint_;
    bool statusRun_;

    QVector<QCPGraphData> pvData;
    QVector<QCPGraphData> svData;
    QVector<QCPGraphData> mvData;
    QVector<double> vtemp_;
    QVector<double> valltemp_;
    QVector<double> vdifftemp_;


    double pid_P, pid_I, pid_D;

    QTimer * clock;
    QTimer * waitTimer;
<<<<<<< HEAD
    //QTime totalElapse;
=======
    QString fileName_;
    QString filePath_;
    QFile output_;
>>>>>>> main
    QElapsedTimer totalElapse;
    bool checkDay;
    int dayCounter;
    bool nextSV;

    const size_t vecSize_ = 1000;

    QDialog * helpDialog;
    QLabel * HelpLabel;
    PlotDialog * plotDialog_;
    ConfigureDialog * configureDialog_;
    int picNumber;
    QMutex mutex_;
    //! Counter that TempCheck was executed.
    int countTempCheck_;
    int countDropCheck_;
};

#endif // MAINWINDOW_H

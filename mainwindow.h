//!
//! \file mainwindow.h
//! \brief mainwindow for connection to the E5CC
//! \author Daisuke Miura (Original source code xxxxx)
//! \date 2023/3/15
//! \details The original source code did not include safety measures.
//! To improve safety, functions were added to check if the temperature was rising and to detect unexpected sudden temperature drops.
//! The GUI was also modified to set the necessary parameters for these functions.
//!

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtSerialPort>
#include <QModbusClient>
#include <QModbusDataUnit>
#include <QModbusRtuSerialMaster> //Qt5
//#include <QModbusRtuSerialClient> //Qt6
#include <QScrollBar>
#include <QMutex>
#include <QMutexLocker>
#include <QImage>
#include <qcustomplot.h>
#include <QElapsedTimer>
#include <QApplication>
#include <QCoreApplication>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFile>
#include <QGraphicsView>
#include <QTextStream>
#include <QSslSocket>
#include "mythread.h"
#include "configuredialog.h"
#include "plotdialog.h"
#include "tempdropdialog.h"
#include "notify.h"

//!
//! \namespace Ui
//!
namespace Ui {
  class MainWindow;
}

class Communication;
class Safety;

//! \brief The MainWindow class
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void LogMsg(QString str, bool newLine = true);
    void findSeriesPortDevices();
    QCustomPlot* getPlot() const;
    Ui::MainWindow* getUi() const;


signals:

public slots:
  void makePlot();
  void updateTemperature(double temperature, bool mute);
  void updateMV(double MV, bool mute);
  void updateSV(double SV, bool mute);
  void updateMVupper(double MVupper, bool mute);
  void updateMVlower(double MVlower, bool mute);
  void updatePID_P(double PID_P, bool mute);
  void updatePID_I(double PID_I, bool mute);
  void updatePID_D(double PID_D, bool mute);
  void finishSendAT(int atFlag);
  void finishSendSV(double SV);
  void catchLogMsg(const QString& msg);
  void connectDevice();
  void connectFailed();
  void catchDanger(int type);
  void updateCheckNumber(int checkNumber);
  void cathcEscapeTempCheckChange(int sign);
  void catchStartTempChangeCheck(int checknumber);


private slots:
    void keyPressEvent(QKeyEvent *key);
    void keyReleaseEvent(QKeyEvent *key);
    void waitForMSec(int msec);
    void panalOnOff(bool IO);

    void showTime();
    void allowSetNextSV();
    void getSetting();
    void writeData();
    bool generateSaveFile();
    bool isIgnore(bool check, double temp);
    void setTextTempDrop(bool);

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
    //void on_action_Setting_Temperature_Drop_triggered();
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
    void setParametersTempCheckChange(bool mute = true);
    void setIgnoreEnable();
    double fillDifference(bool mute = true);

    void setColor(int colorindex = 0, bool changerable = true);

    void sendLINE(const QString& message);

    //void sendLine(const QString& message);



private:
    Ui::MainWindow *ui;
    QCustomPlot * plot;
    Communication *com_;
    Safety *safety_;
    Notify *notify_;

    QMessageBox * LogMsgBox_;

    QString omronPortName;

    int msgCount;
    int threadTimerInterval_;
    int connectionTimerInteral_;
    int timing_;
    QDateTime dateStart_;
    QString dateStartStr_;
    bool tempControlOnOff;
    bool tempRecordOnOff;
    bool comboxEnable;
    bool spinBoxEnable;
    bool muteLog;
    bool statusAskTemp_;
    bool statusAskMV_;
    bool statusAskSetPoint_;
    bool statusRun_;
    bool isSettParametersTempCheck_;

    QVector<QCPGraphData> pvData;
    QVector<QCPGraphData> svData;
    QVector<QCPGraphData> mvData;
    QVector<double> vtemp_;
    QVector<double> valltemp_;
    QVector<double> vdifftemp_;

    QTimer * clock;
    QTimer * waitTimer;
    QTimer * connectionTimer_;
    QString fileName_;
    QString filePath_;

    QString LINEToken_ ;
    QUrl LINEurl_;

    QElapsedTimer totalElapse;
    bool checkDay;
    int dayCounter;
    bool nextSV;


    QDialog * helpDialog;
    QLabel * HelpLabel;
    QGraphicsScene *scene_;
    QGraphicsView *view;
    PlotDialog * plotDialog_;
    ConfigureDialog * configureDialog_;
    TempDropDialog * tempDropDialog_;
    int picNumber;
    int countTempCheck_;
    int countDropCheck_;
    bool bkgColorChangeable_;

    void addPortName(QList<QSerialPortInfo> info);

    //The following functions are inplemented in gui.cpp
    void setupPlot();
    void setupCombBox();
    void setEnabledFalse();
    void initializeVariables();

};

#endif // MAINWINDOW_H

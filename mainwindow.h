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
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkReply>
#include <QUrlQuery>
#include <QSslSocket>
#include "mythread.h"
#include "configuredialog.h"
#include "plotdialog.h"
#include "tempdropdialog.h"

//!
//! \namespace Ui
//!
namespace Ui {
class MainWindow;
}


//!
//! \brief The MainWindow class
//!
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
  void checkThreads();

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
    bool isDrop(double diff, int mode);
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
    void setParametersTempCheck(bool mute = true);
    void setIgnoreEnable();
    double fillDifference(bool mute = true);

    void setColor(int colorindex = 0);

    double calcMovingAve(QVector<double> vtemp);

    void sendLineNotify(const QString& message, const QString& token);
    void sendLine(const QString& message);
    void sendLineNotifyConnection();

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
    int threadTimerInterval_;
    //int connectionTimerInteral_;
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
    bool isSettParametersTempCheck_;

    QVector<QCPGraphData> pvData;
    QVector<QCPGraphData> svData;
    QVector<QCPGraphData> mvData;
    QVector<double> vtemp_;
    QVector<double> valltemp_;
    QVector<double> vdifftemp_;


    double pid_P, pid_I, pid_D;

    QTimer * clock;
    QTimer * waitTimer;
    QTimer * threadTimer_;
    //QTimer * connectionTimer_;
    QString fileName_;
    QString filePath_;
    //QFile output_;
    QElapsedTimer totalElapse;
    bool checkDay;
    int dayCounter;
    bool nextSV;

    const size_t vecSize_ = 1000;

    QDialog * helpDialog;
    QLabel * HelpLabel;
    QGraphicsScene *scene_;
    QGraphicsView *view;
    PlotDialog * plotDialog_;
    ConfigureDialog * configureDialog_;
    TempDropDialog * tempDropDialog_;
    int picNumber;
    QMutex mutex_;
    //! Counter that TempCheck was executed.
    int countTempCheck_;
    int countDropCheck_;
};

#endif // MAINWINDOW_H

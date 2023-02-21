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
#include <qcustomplot.h>
#include <QElapsedTimer>
#include <QApplication>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include "mythread.h"
#include "myqlineedit.h"
#include "configuredialog.h"

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
  void periodic_work();
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

    void askTemperature();
    void askSetPoint();
    void askMV();
    void askMVupper();
    void askMVlower();
    void getSetting();
    void setAT(int atFlag);
    void setSV(double SV);

    void on_lineEdit_Cmd_returnPressed();

    void on_pushButton_Connect_clicked();
    void on_pushButton_AskStatus_clicked();
    void on_pushButton_SetSV_clicked();
    void on_pushButton_GetPID_clicked();
    void on_pushButton_RecordTemp_clicked();
    void on_pushButton_ReadRH_clicked();
    void on_pushButton_Control_clicked();

    void on_comboBox_AT_currentIndexChanged(int index);
    void on_comboBox_Mode_currentIndexChanged(int index);
    void on_comboBox_MemAddress_currentTextChanged(const QString &arg1);

    void on_checkBox_MuteLogMsg_clicked(bool checked);

    void on_doubleSpinBox_MVlower_valueChanged(double arg1);
    void on_doubleSpinBox_MVupper_valueChanged(double arg1);

    void on_actionOpen_File_triggered();
    void on_action_Setting_parameters_for_TempCheck_triggered();

    void fillDataAndPlot(const QDateTime date, const double PV, const double SV, const double MV);

    void on_actionHelp_Page_triggered();
    void HelpPicNext();

    void on_radioButton_Run_clicked();
    void on_radioButton_Stop_clicked();

    void Quit();
    //void CheckTemp();

    void setIntervalAskMV();
    void setIntervalAskTemp();
    void setSafeLimit();
    void setNumbers();
    void setIgnoreRange();
    void setParametersTempCheck();
    double diffTemp(double ctemp);

    void on_radioButton_TempCheck_toggled(bool checked);

private:
    Ui::MainWindow *ui;
    QCustomPlot * plot;
    QModbusRtuSerialMaster * omron;
    MyThread * threadMVcheck_;
    MyThread * threadLog_;
    MyThread * threadTempCheck_;

    QString omronPortName;
    int omronID;

    int msgCount;
    int respondType;

    double temperature, SV, MV;
    double MVupper, MVlower;
    double tempDecimal;

    bool tempControlOnOff;
    bool tempRecordOnOff;
    bool modbusReady;
    bool comboxEnable;
    bool spinBoxEnable;
    bool muteLog;

    QVector<QCPGraphData> pvData;
    QVector<QCPGraphData> svData;
    QVector<QCPGraphData> mvData;
    QVector<double> vtemp_;
    QVector<double> vdifftemp_;

    double pid_P, pid_I, pid_D;

    QTimer * clock;
    QTimer * waitTimer;
    QElapsedTimer totalElapse;
    bool checkDay;
    int dayCounter;
    bool nextSV;

    QDialog * helpDialog;
    QLabel * HelpLabel;
    ConfigureDialog * configureDialog_;
    int picNumber;
    QMutex mutex_;
    //! Counter that TempCheck was executed.
    int countTempCheck_;
};

#endif // MAINWINDOW_H

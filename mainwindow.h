/**
 * @file mainwindow.h
 * @brief This file contains the declaration of the MainWindow class and its associated methods.
*/

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
#include "configuredialog.h"
#include "plotdialog.h"
#include "tempdropdialog.h"
#include "helpdialog.h"
#include "joinlinedialog.h"
#include "notify.h"
#include "datasummary.h"

/**
 * @brief The MainWindow class represents the main window of the application.
*/
namespace Ui {
  class MainWindow;
}

class Communication;
class Safety;
class DataSummary;

/**
 *  @brief The MainWindow class represents the main window of the application.
*/
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
  /**
  * @brief Constructor for MainWindow class.
  * @param parent The parent QWidget.
  */
    explicit MainWindow(QWidget *parent = 0);

  /**
   * @brief Destructor for MainWindow class.
   */
    ~MainWindow();

  /**
   * @brief Logs a message to the GUI log.
   * @param str The message to be logged.
   * @param newLine Whether or not to add a new line after the message.
   */
    void LogMsg(QString str, bool newLine = true);

    /**
     * @brief Finds all available serial port devices.
     */
    void findSeriesPortDevices();

signals:

public slots:
  /**
   * @brief makePlot Slot function to generate plot
  */
  void makePlot();

  /**
  @brief updateTemperature Slot function to update the temperature display
  @param temperature The new temperature value to display
  */
  void updateTemperature(double temperature);

  /**
  @brief updateMV Slot function to update the MV display (MV means output power)
  @param MV The new measured value to display.
  */
  void updateMV(double MV);

  /**
  @brief updateSV Slot function to update the set value (SV) display
  @param SV The new set value to display
  */
  void updateSV(double SV);

  /**
  @brief updateMVupper Slot function to update the upper limit of the MV display
  @param MVupper The new upper limit to display
  */
  void updateMVupper(double MVupper);

  /**
  @brief updateMVlower Slot function to update the lower limit of MV display
  @param MVlower The new lower limit to display
  */
  void updateMVlower(double MVlower);

  /**
  @brief updatePID_I Slot function to update the PID controller's integral (I) gain value display
  @param PID_I The new I gain value to display
  */
  void updatePID_P(double PID_P);

  /**
  @brief updatePID_I Slot function to update the PID controller's integral (I) gain value display
  @param PID_I The new I gain value to display
  */
  void updatePID_I(double PID_I);

  /**
  @brief updatePID_D Slot function to update the PID controller's derivative (D) gain value display
  @param PID_D The new D gain value to display  void updatePID_D(double PID_D);
  */
  void updatePID_D(double PID_D);

  /**
  @brief updateStatus Slot function to update the status display
  */
  void updateStatus();

  /**
  @brief finishSendAT Slot function to handle the completion of sending AT command to device
  @param atFlag The flag indicating whether the command was sent successfully
  */
  void finishSendAT(int atFlag);

  /**
  @brief finishSendSV Slot function to handle the completion of sending the set value (SV) to the device
  @param SV The set value that was sent
  */
  void finishSendSV(double SV);

  /**
  @brief catchLogMsg Slot function to catch a log message emitted by a thread and add it to the application log window
  @param msg The log message to add
  */
  void catchLogMsg(const QString& msg);

  /**
  @brief catchLogMsg Slot function to catch a log message emitted by a thread and add it to the application log window
  @param msg The log message to add
  @param color The color of message to display
  */
  void catchLogMsgWithColor(const QString& msg, QColor color);

  /**
  @brief connectDevice Slot function to handle device connection
  */
  void connectDevice();

  /**
  @brief connectFailed Slot function to handle device connection failure
  */
  void connectFailed();

  /**
  @brief catchDanger Slot function to handle a dangerous situation detected by the Safety class
  @param type The type of danger detected
  */
  void catchDanger(int type);

  /**
  @brief updateCheckNumber Slot function to update the number of temperature checks that have been performed
  @param checkNumber The number of checks performed
  */
  void updateCheckNumber(int checkNumber);

  /**
  @brief cathcEscapeTempCheckChange Slot function to handle changes in the escape temperature check checkbox
  @param sign The sign of the change (1 for checked, -1 for unchecked)
  */
  void cathcEscapeTempCheckChange(int sign);

  /**
  @brief catchStartTempChangeCheck Slot function to handle the start of a temperature change check
  @param checknumber The number of the check being started
  */
  void catchStartTempChangeCheck(int checknumber);


private slots:
  /**
  @brief keyPressEvent handles the key press event. Used for keyboard control of the GUI.
  @param key The key that was pressed.
  */
    void keyPressEvent(QKeyEvent *key);

    /**
    @brief keyReleaseEvent handles the key release event. Used for keyboard control of the GUI.
    @param key The key that was released.
    */
    void keyReleaseEvent(QKeyEvent *key);

    /**
    @brief waitForMSec waits for a specified number of milliseconds.
    @param msec The number of milliseconds to wait.
    */
    void waitForMSec(int msec);

    /**

    @brief panalOnOff turns the panel on or off.
    @param IO True to turn the panel on, false to turn it off.
    */
    void panalOnOff(bool IO);

    /**

    @brief showTime updates the current time displayed in the GUI.
    */
    void showTime();

    /**
    @brief getSetting retrieves the current setting values from the temperature controller.
    */
    void getSetting();

    /**
    @brief setTextTempDrop sets the text temperature drop value.
    @param checked True if the text temperature drop is enabled, false otherwise.
    */
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
    void on_pushButton_RunStop_toggled(bool checked);
    void on_actionOpen_File_triggered();
    void on_action_Setting_parameters_for_TempCheck_triggered();
    void on_action_Setting_plot_triggered();
    void on_actionHelp_Page_triggered();
    void on_action_JoinLINE_RIKEN_triggered();
    void on_action_JoinLINE_Kyushu_triggered();
    void fillDataAndPlot(const QDateTime date, const double PV, const double SV, const double MV);
    void Run();
    void Stop();
    void Quit();
    void setIntervalAskMV();
    void setIntervalAskTemp();
    void setIntervalPlot(int interval);
    void setSafeLimit();
    void setNumbers();
    void setIgnoreRange();
    void setParametersTempCheckChange(bool mute = true);
    void setIgnoreEnable();
    double fillDifference(bool mute = true);


    /**
     * @brief Send a message to LINE.
     * @param message The message to send.
     */
    void setColor(int colorindex = 0);

    /**
     * @brief Send a message to LINE.
     * @param message The message to send.
     */
    void sendLINE(const QString& message);
private:
    Ui::MainWindow *ui;                            ///< Pointer to the MainWindow UI object
    QCustomPlot *plot{nullptr};                     ///< Pointer to the QCustomPlot object
    Communication *com_{nullptr};                   ///< Pointer to the Communication object
    Safety *safety_{nullptr};                       ///< Pointer to the Safety object
    Notify *notify_{nullptr};                       ///< Pointer to the Notify object
    DataSummary *data_{nullptr};                    ///< Pointer to the DataSummary object
    HelpDialog *helpDialog_{nullptr};               ///< Pointer to the HelpDialog object
    QGraphicsScene *scene_{nullptr};                ///< Pointer to the QGraphicsScene object
    QGraphicsView *view{nullptr};                   ///< Pointer to the QGraphicsView object
    PlotDialog *plotDialog_{nullptr};               ///< Pointer to the PlotDialog object
    ConfigureDialog *configureDialog_{nullptr};     ///< Pointer to the ConfigureDialog object
    TempDropDialog *tempDropDialog_{nullptr};       ///< Pointer to the TempDropDialog object
    JoinLINEDialog *joinDialog_{nullptr};            ///< Pointer to the JoinLINEDialog object
    JoinLINEDialog *joinDialogK_{nullptr};            ///< Pointer to the JoinLINEDialog object
    QString omronPortName{};                        ///< Name of the Omron port
    int msgCount{0};                                ///< Count of log messages
    int timing_{0};                                 ///< Timing variable
    int intervalPlot_{5000};                        ///< Interval for the plot timer in milliseconds
    bool tempControlOnOff{false};                   ///< Flag indicating temperature control on/off state
    bool tempRecordOnOff{false};                    ///< Flag indicating temperature recording on/off state
    bool comboxEnable{false};                       ///< Flag indicating the enabled state of the combobox
    bool spinBoxEnable{false};                      ///< Flag indicating the enabled state of the spinbox
    bool muteLog{false};                            ///< Flag indicating the log muting state
    QDateTime dateStart_{};                         ///< Start date and time
    QString filePath_{};                            ///< File path
    QVector<QCPGraphData> pvData{};                  ///< Vector containing PV data for the plot
    QVector<QCPGraphData> svData{};                  ///< Vector containing SV data for the plot
    QVector<QCPGraphData> mvData{};                  ///< Vector containing MV data for the plot
    QVector<double> vtemp_{};                        ///< Vector containing temperature values
    QVector<double> valltemp_{};                     ///< Vector containing all temperature values
    QVector<double> vdifftemp_{};                    ///< Vector containing temperature difference values
    QTimer *clock{nullptr};                          ///< Pointer to the QTimer object for clock
    QTimer *waitTimer{nullptr};                      ///< Pointer to the QTimer object for wait timer
    QTimer *plotTimer_{nullptr};                     ///< Pointer to the QTimer object for plot timer

    QString LINEToken_{};                            ///< LINE token
    QUrl LINEurl_{};                                 ///< LINE URL
    QElapsedTimer totalElapse{};                     ///< Elapsed timer for total time
    bool checkDay{false};                            ///< Flag indicating the check day state
    int dayCounter{};                                ///< Counter for days
    bool bkgColorChangeable_{true};                  ///< Flag indicating the changeability of background color
    bool isQuit_{false};                             ///< Flag indicating the quit state

    // Private functions
    void addPortName(QList<QSerialPortInfo> info);                          ///< Function to add port names
    void setupPlot();                                                      ///< Function to setup the plot

    /**
    * @brief setupCombBox Initializes the combobox setup.
    */
    void setupCombBox();

    /**
     * @brief setEnabledFalse Disables certain UI elements.
     */
    void setEnabledFalse();

    /**
     * @brief setupDialog Initializes the dialog setup.
     */
    void setupDialog();

    /**
     * @brief initializeVariables Initializes the variables.
     */
    void initializeVariables();

    /**
     * @brief updateStatusBoxes Updates the state of the checkboxes based on the activity status of the QTimer objects.
     */
    void updateStatusBoxes();

    /**
     * @brief controlStableMode Controls the stable mode based on the target value, temperature tolerance, and temperature step size.
     * @param targetValue The target value.
     * @param tempTorr The temperature tolerance.
     * @param tempStepSize The temperature step size.
     */
    void controlStableMode(double targetValue, double tempTorr, double tempStepSize);

    /**
     * @brief controlFixedTimeMode Controls the fixed time mode based on the target value, temperature tolerance, and temperature step size.
     * @param targetValue The target value.
     * @param tempTorr The temperature tolerance.
     * @param tempStepSize The temperature step size.
     */
    void controlFixedTimeMode(double targetValue, double tempTorr, double tempStepSize);

    /**
     * @brief controlFixedRateMode Controls the fixed rate mode based on the target value, temperature tolerance, and temperature step size.
     * @param targetValue The target value.
     * @param tempTorr The temperature tolerance.
     * @param tempStepSize The temperature step size.
     */
    void controlFixedRateMode(double targetValue, double tempTorr, double tempStepSize);

    /**
     * @brief controlNormalAndFixedRateMode Controls the normal and fixed rate mode based on the target value, temperature tolerance, and temperature step size.
     * @param targetValue The target value.
     * @param tempTorr The temperature tolerance.
     * @param tempStepSize The temperature step size.
     */
    void controlNormalAndFixedRateMode(double targetValue, double tempTorr, double tempStepSize);

    /**
     * @brief calcRate Calculates the rate of temperature change based on the current temperature, target temperature, and time interval.
     * @param temp The current temperature.
     * @param aftertemp The target temperature.
     * @param min The time interval in minutes.
     * @return The calculated rate of temperature change.
     */
    double calcRate(double temp, double aftertemp, int min);

};

#endif // MAINWINDOW_H

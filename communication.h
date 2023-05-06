/**
 * @file communication.h
 * @brief This file defines the Communication class and its nested class E5CC_Address,
 *        which contains an enum class Type for ModBus addresses used for communication
 *        with the E5CC temperature controller.
 *  @author Daisuke Miura
 *  @date 2023/5/5
 *
 */

#ifndef COMMUNICATION_H
#define COMMUNICATION_H

#include <QtSerialBus/QModbusRtuSerialMaster>
#include <QSerialPort>
#include <QStatusBar>
#include <QModbusTcpClient>
#include <QMutex>
#include "mainwindow.h"



/**
 * @brief The Communication class provides functionality for ModBus communication
 *        with the E5CC temperature controller.
 */
class Communication : public QObject{
  Q_OBJECT
  Q_PROPERTY(double Temperature READ getTemperature WRITE setTemperature NOTIFY TemperatureUpdated);
  Q_PROPERTY(double SV READ getSV WRITE setSV NOTIFY SVUpdated);
  Q_PROPERTY(double MV READ getMV WRITE setMV NOTIFY MVUpdated);
  Q_PROPERTY(double MVupper READ getMVupper WRITE setMVupper NOTIFY MVupperUpdated);
  Q_PROPERTY(double MVlower READ getMVlower WRITE setMVlower NOTIFY MVlowerUpdated);
  Q_PROPERTY(double PID_P READ getPID_P WRITE setPID_P NOTIFY PID_PUpdated);
  Q_PROPERTY(double PID_I READ getPID_I WRITE setPID_P NOTIFY PID_IUpdated);
  Q_PROPERTY(double PID_D READ getPID_D WRITE setPID_D NOTIFY PID_DUpdated);
  Q_PROPERTY(int OmronID READ getOmronID WRITE setOmronID NOTIFY OmronIDChanged);
public:
    /**
     * @brief Constructs a Communication object.
     */
    Communication(QMainWindow *parent, QStatusBar *statusBar);
    ~Communication();
    /**
     * @brief The E5CC_Address nested class defines an enum class Type for ModBus addresses
     *        used for communication with the E5CC temperature controller.
     */
    class E5CC_Address {
    public:
        /**
         * @brief The Type enum class defines the ModBus addresses for communication with
         *        the E5CC temperature controller.
         */
        enum class Type {
            PV = 0x0000, /**< Get Temperature */
            MV = 0x0008, /**< Get output power */
            SV = 0x0106, /**< Get set value */
            MVupper = 0x0A0A, /**< Get putput upper */
            MVlower = 0x0A0C, /**< Get output lower */
            PID_P = 0x0A00, /**< Get Proportional */
            PID_I = 0x0A02, /**< Get Integration */
            PID_D = 0x0A04 /**< Get Difference */
        };
    };
    void request(QModbusPdu::FunctionCode code, QByteArray cmd);
    QString formatHex(int value, int digit);
    QString formatE5CCAddress(E5CC_Address::Type address, int width = 4);
    void askTemperature();
    void askMV();
    void askSV();
    void askMVupper();
    void askMVlower();
    void askPID(QString PID);
    void executeConnection(){Connection();}
    void executeRun(){Run();}
    void executeStop(){Stop();}
    void executeSendRequestSV(double SV){sendRequestSV(SV);}
    void executeSendRequestAT(int atFlag){sendRequestAT(atFlag);}
    void changeMVlowerValue(double MVlower);
    void changeMVupperValue(double MVupper);
    void changeSVValue(double SV);


    void read(QModbusDataUnit::RegisterType type, quint16 adress, int size);

    //setter methods
    void setSerialPortName(QString portName);
    void setTemperature(double temperature);
    void setSV(double SV);
    void setMV(double MV);
    void setMVupper(double MVupper);
    void setMVlower(double MVlower);
    void setPID_P(double PID_P);
    void setPID_I(double PID_I);
    void setPID_D(double PID_D);
    void setOmronID(int omronID);

    //getter methods
    QModbusRtuSerialMaster* getOmron() const;
    QList<QSerialPortInfo> getSerialPortDevices() const;
    QString getPortName() const;
    double getTemperature() const;
    double getMV() const;
    double getSV() const;
    double getMVupper() const;
    double getMVlower() const;
    double getPID_P() const;
    double getPID_I() const;
    double getPID_D() const;
    int getOmronID() const;


    /**
     * @brief The timing enum defines the timing values used in the Communication class.
     */
    enum timing{
        modbus = 200, /**< The time in milliseconds between Modbus communication */
        getTempTimer = 500, /**< The time in milliseconds between requests for temperature */
        clockUpdate = 50, /**< The time in milliseconds between clock updates */
        timeUp = 1000*60*10, /**< The maximum time in milliseconds before resetting the clock */
        timeOut = 700 /**< The time in milliseconds before a Modbus communication times out */
    };

signals:
    void TemperatureUpdated(double temperature);
    void MVUpdated(double MV);
    void SVUpdated(double SV);
    void MVupperUpdated(double MVupper);
    void MVlowerUpdated(double MVlopwer);
    void PID_PUpdated(double PID_P);
    void PID_IUpdated(double PID_I);
    void PID_DUpdated(double PID_D);
    void ATSendFinish(int atFlag);
    void SVSendFinish(double SV);
    void connectTimeout();
    //void errorOccurred(QModbusDevice::Error error);
    //void errorOccurred(QString error);
    //void stateChanged(QModbusDevice::State state);
    void logMsg(QString string, bool newline = true);
    void deviceConnect();
    void failedConnect();
    void OmronIDChanged();

private:
    /**
     * @brief Pointer to the QModbusRtuSerialMaster object for communication with the E5CC temperature controller.
     */
    QModbusRtuSerialMaster* omron_;
    QList<QSerialPortInfo> infos_;
    QModbusTcpClient* modbusDevice_;
    QModbusReply *modbusReply_;
    QTimer *connectTimer_;
    MainWindow *mainwindow_;
    QStatusBar *statusBar_;
    QMutex mutex_;
    int respondType_;
    bool modbusReady_;
    int omronID_;
    QString portName_;
    double temperature_, SV_, MV_;
    double MVupper_, MVlower_;
    double tempDecimal_ = 0.1;
    double pid_P_, pid_I_, pid_D_;

    //QModbusDataUnit createDataUnit(QModbusDataUnit::RegisterType type, quint16 address, int size);
    //QModbusReply* sendReadRequest(const QModbusDataUnit& request);

    void Connection();
    void Run();
    void Stop();
    void sendRequestSV(double SV);
    void sendRequestAT(int afFlag);

private slots:
    void waitForMsec(int msec);
    //void read(QModbusDataUnit::RegisterType type, quint16 adress, int size);
    void readReady();


};
#endif // COMMUNICATION_H

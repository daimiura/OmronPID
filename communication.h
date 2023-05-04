/**
 * @file communication.h
 * @brief This file defines the Communication class and its nested class E5CC_Address,
 *        which contains an enum class Type for ModBus addresses used for communication
 *        with the E5CC temperature controller.
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
public:
    /**
     * @brief Constructs a Communication object.
     */
    Communication(QObject *parent, QStatusBar *statusBar);
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

    QModbusRtuSerialMaster *getOmron();
    QList<QSerialPortInfo> getSerialPortDevices();
    void request(QModbusPdu::FunctionCode code, QByteArray cmd);
    QString formatHex(int value, int digit);
    QString formatE5CCAddress(E5CC_Address::Type address, int width = 4);
    void askTemperature();
    void askMV();
    void askSV();
    void askMVupper();
    void askMVlower();
    void askPID(QString PID);
    void executeConnection(QModbusRtuSerialMaster* omron);
    void executeRun();
    void executeStop();
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

    //getter methods
    double getTemperature();
    double getMV();
    double getSV();
    double getMVupper();
    double getMVlower();
    double getPID_P();
    double getPID_I();
    double getPID_D();

    /**
     * @brief The timing enum defines the timing values used in the Communication class.
     */
    enum timing{
        modbus = 100, /**< The time in milliseconds between Modbus communication */
        getTempTimer = 500, /**< The time in milliseconds between requests for temperature */
        clockUpdate = 50, /**< The time in milliseconds between clock updates */
        timeUp = 1000*60*10, /**< The maximum time in milliseconds before resetting the clock */
        timeOut = 700 /**< The time in milliseconds before a Modbus communication times out */
    };

signals:
    void temperatureUpdated(double temperature);
    void MVUpdated(double MV);
    void SVUpdated(double SV);
    void MVupperUpdated(double MVupper);
    void MVlowerUpdated(double MVlopwer);
    void PID_PUpdated(double PID_P);
    void PID_IUpdated(double PID_I);
    void PID_DUpdated(double PID_D);
    void connectTimeout();
    //void errorOccurred(QModbusDevice::Error error);
    //void errorOccurred(QString error);
    //void stateChanged(QModbusDevice::State state);
    void logMsg(QString string);
    void deviceConnect();
    void failedConnect();



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
    int slaveAddress_;
    int respondType_;
    bool modbusReady_;
    int OmronID_;
    QString portName_;
    double temperature_, SV_, MV_;
    double MVupper_, MVlower_;
    double tempDecimal_ = 0.1;
    double pid_P_, pid_I_, pid_D_;

    //QModbusDataUnit createDataUnit(QModbusDataUnit::RegisterType type, quint16 address, int size);
    //QModbusReply* sendReadRequest(const QModbusDataUnit& request);

    void Connection(QModbusRtuSerialMaster* omron);
    void Run();
    void Stop();

private slots:
    void waitForMsec(int msec);
    //void read(QModbusDataUnit::RegisterType type, quint16 adress, int size);
    void readReady();


};
#endif // COMMUNICATION_H


/**
@brief The Communication class provides an interface for communicating
with he E5CC temperature controller via Modbus protocol.
*/


#ifndef COMMUNICATION_H
#define COMMUNICATION_H

#include <QtSerialBus/QModbusRtuSerialMaster>
#include <QSerialPort>
#include <QStatusBar>
#include <QModbusTcpClient>
#include <QMutex>
#include "mainwindow.h"


class Communication : public QObject{
  Q_OBJECT
  Q_PROPERTY(double Temperature READ getTemperature WRITE setTemperature NOTIFY TemperatureUpdated)
  Q_PROPERTY(double SV READ getSV WRITE setSV NOTIFY SVUpdated)
  Q_PROPERTY(double MV READ getMV WRITE setMV NOTIFY MVUpdated)
  Q_PROPERTY(double MVupper READ getMVupper WRITE setMVupper NOTIFY MVupperUpdated)
  Q_PROPERTY(double MVlower READ getMVlower WRITE setMVlower NOTIFY MVlowerUpdated)
  Q_PROPERTY(int OmronID READ getOmronID WRITE setOmronID NOTIFY OmronIDChanged)
  Q_PROPERTY(int intervalUpdate READ getIntervalUpdate WRITE setIntervalUpdate NOTIFY intervalUpdateChanged)
  Q_PROPERTY(int intervalConectionCheck READ getIntervalConectionCheck WRITE setIntervalConectionCheck NOTIFY intervalConectionCheckChanged)
public:
  /**
  * @brief Constructs a Communication object with a parent object and a status bar.
  * @param parent The parent object of this object
  * @param statusBar The status bar widget to show the communication status
  */
    Communication(QMainWindow *parent, QStatusBar *statusBar);

    /**
     * @brief Destructs a Communication object.
     */
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

    /**
    @brief Sends a ModBus request with the specified code and command.
    @param code The ModBus function code.
    @param cmd The command to send.
    */
    void request(QModbusPdu::FunctionCode code, QByteArray cmd);

    /**
    @brief Formats the specified integer value as a hexadecimal string with the specified number of digits.
    @param value The integer value to format.
    @param digit The number of digits to include in the hexadecimal string.
    @return The formatted hexadecimal string.
    */
    QString formatHex(int value, int digit);

    /**
    @brief Formats the specified E5CC temperature controller address as a hexadecimal string.
    @param address The E5CC temperature controller address to format.
    @param width The width of the hexadecimal string.
    @return The formatted E5CC temperature controller address as a hexadecimal string.
    */
    QString formatE5CCAddress(E5CC_Address::Type address, int width = 4);

    /**
    @brief Sends a request to get the temperature from the E5CC temperature controller.
    */
    void askTemperature();

    /**
    @brief Sends a request to get the output power from the E5CC temperature controller.
    */
    void askMV();

    /**
    @brief Sends a request to get the set value from the E5CC temperature controller.
    */
    void askSV();

    /**
    @brief Sends a request to get the output upper limit from the E5CC temperature controller.
    */
    void askMVupper();

    /**
    @brief Sends a request to get the output lower limit from the E5CC temperature controller.
    */
    void askMVlower();

    /**
    @brief Sends a request to get the specified PID parameter from the E5CC temperature controller.
    @param PID The PID parameter to get.
    */
    void askPID(QString PID);

    /**
    @brief Executes the connection to the ModBus device.
    */
    void executeConnection(){Connection();}

    /**
    @brief Executes the Run command on the ModBus device.
    */
    void executeRun(){Run();}

    /**
    @brief Executes the Stop command on the ModBus device.
    */
    void executeStop(){Stop();}

    /**
    @brief Sends a request to set the set value (SV) of the E5CC temperature controller.
    @param SV The value to set as the set value (SV).
    */
    void executeSendRequestSV(double SV){sendRequestSV(SV);}

    /**
    @brief Sends a request to set the AT flag of the E5CC temperature controller.
    @param atFlag The value of the AT flag to set.
    */
    void executeSendRequestAT(int atFlag){sendRequestAT(atFlag);}

    /**
    @brief Changes the value of the output lower limit of the E5CC temperature controller.
    @param MVlower The new value of the output lower limit.
    */
    void changeMVlowerValue(double MVlower);

    /**
    @brief Changes the value of the output upper limit of the E5CC temperature controller.
    @param MVupper The new value of the output upper limit.
    */
    void changeMVupperValue(double MVupper);

    /**
    @brief Changes the set value (SV) of the E5CC temperature controller.
    @param SV The new value of the set value (SV).
    */
    void changeSVValue(double SV);

    /**
    @brief Reads data from the specified register type and address of the ModBus device.
    @param type The type of the register to read.
    @param address The address of the register to read.
    @param size The number of registers to read.
    */
    void read(QModbusDataUnit::RegisterType type, quint16 adress, int size);

    /**
    @brief Sets the name of the serial port to use for Modbus communication
    @param portName The name of the serial port to use
    */

    void setSerialPortName(QString portName);
    /**
    @brief Sets the current temperature
    @param temperature The current temperature to set
    */
    void setTemperature(double temperature);

    /**
    @brief Sets the set value for temperature
    @param SV The set value to set
    */
    void setSV(double SV);

    /**
    @brief Sets the current output power
    @param MV The current output power to set
    */
    void setMV(double MV);

    /**
    @brief Sets the upper limit for the output power
    @param MVupper The upper limit for the output power to set
    */
    void setMVupper(double MVupper);

    /**
    @brief Sets the lower limit for the output power
    @param MVlower The lower limit for the output power to set
    */
    void setMVlower(double MVlower);
    /**

    @brief Sets the Omron ID for Modbus communication
    @param omronID The Omron ID to set
    */
    void setOmronID(int omronID);

    /**
    @brief Sets the interval in milliseconds for updating the temperature and output power values
    @param interval The interval in milliseconds to set
    */
    void setIntervalUpdate(int interval);

    /**
    @brief Sets the interval in milliseconds for checking the Modbus connection status
    @param interval The interval in milliseconds to set
    */
    void setIntervalConectionCheck(int interval);

    /**
    @brief Returns a pointer to the QModbusRtuSerialMaster object used for communication with the Omron PLC.
    @return A pointer to the QModbusRtuSerialMaster object
    **/
    QModbusRtuSerialMaster* getOmron() const;

    /**
    @brief Returns a list of QSerialPortInfo objects containing information about available serial port devices.
    @return A list of QSerialPortInfo objects
    */
    QList<QSerialPortInfo> getSerialPortDevices() const;

    /**
    @brief Returns the name of the currently set serial port.
    @return The name of the currently set serial port
    */
    QString getPortName() const;

    /**
     * @brief Gets the current temperature value.
     * @return The current temperature value
     */
    double getTemperature() const;

    /**
     * @brief Gets the set value.
     * @return The set value
     */
    double getMV() const;

    /**
     * @brief Gets the current output power value.
     * @return The current output power value
     */
    double getSV() const;

    /**
     * @brief Gets the output upper value.
     * @return The output upper value
     */
    double getMVupper() const;

    /**
     * @brief Gets the output lower value.
     * @return The output lower value
     */
    double getMVlower() const;

    /**
    @brief Returns the Proportional value of the PID control for the temperature controller.
    @return The Proportional value of the PID control.
    */
    double getPID_P() const;

    /**
    @brief Returns the Integration value of the PID control for the temperature controller.
    @return The Integration value of the PID control.
    */
    double getPID_I() const;

    /**
    @brief Returns the Difference value of the PID control for the temperature controller.
    @return The Difference value of the PID control.
    */
    double getPID_D() const;

    /**
    @brief Returns the ID of the Omron temperature controller connected to the system.
    @return The ID of the Omron temperature controller connected to the system.
    */
    int getOmronID() const;

    /**
    @brief Returns the interval time in seconds at which the temperature and control data are updated.
    @return The interval time in seconds at which the temperature and control data are updated.
    */
    int getIntervalUpdate() const;

    /**
    @brief Returns the interval time in seconds at which the connection to the Omron temperature controller is checked.
    @return The interval time in seconds at which the connection to the Omron temperature controller is checked.
    */
    int getIntervalConectionCheck() const;

    /**

    @brief The timing enumeration defines various timing intervals in milliseconds.
    */
    enum timing {
    modbus = 300, /**< The time in milliseconds between Modbus communication */
    getTempTimer = 500, /**< The time in milliseconds between requests for temperature */
    clockUpdate = 50, /**< The time in milliseconds between clock updates */
    timeUp = 1000 * 60 * 10, /**< The maximum time in milliseconds before resetting the clock */
    timeOut = 700 /**< The time in milliseconds before a Modbus communication times out */
    };

signals:
    /**
    @brief This signal is emitted when the temperature value is updated.
    @param temperature The new temperature value.
    */
    void TemperatureUpdated(double temperature);

    /**
    @brief This signal is emitted when the output power value is updated.
    @param MV The new output power value.
    */
    void MVUpdated(double MV);

    /**
    @brief This signal is emitted when the set value is updated.
    @param SV The new set value.
    */
    void SVUpdated(double SV);

    /**
    @brief This signal is emitted when the output upper value is updated.
    @param MVupper The new output upper value.
    */
    void MVupperUpdated(double MVupper);

    /**
    @brief This signal is emitted when the output lower value is updated.
    @param MVlower The new output lower value.
    */
    void MVlowerUpdated(double MVlower);

    /**
     * @brief Emitted when the proportional (P) gain value of the PID controller is updated.
     * @param PID_P The updated PID P value.
     */
    void PID_PUpdated(double PID_P);

    /**
     * @brief Emitted when the integral (I) gain value of the PID controller is updated.
     * @param PID_I The updated PID I value.
     */
    void PID_IUpdated(double PID_I);

    /**
     * @brief Emitted when the derivative (D) gain value of the PID controller is updated.
     * @param PID_D The updated PID D value.
     */
    void PID_DUpdated(double PID_D);

    /**
     * @brief Emitted when an AT command is sent to the Omron device and the send is finished.
     * @param atFlag The flag indicating which AT command was sent.
     */
    void ATSendFinish(int atFlag);

    /**
     * @brief Emitted when a set value (SV) command is sent to the Omron device and the send is finished.
     * @param SV The updated SV value.
     */
    void SVSendFinish(double SV);

    /**
     * @brief Emitted when a connection attempt to the Omron device times out.
     */
    void connectTimeout();

    /**
     * @brief Emitted when a log message is generated.
     * @param string The log message.
     * @param newline True if a new line should be added to the log message, false otherwise.
     */
    void logMsg(QString string, bool newline = true);

    /**
     * @brief Emitted when the status of the controller is updated.
     */
    void statusUpdate();

    /**
     * @brief Emitted when a connection to the Omron device is established.
     */
    void deviceConnect();

    /**
     * @brief Emitted when a connection to the Omron device fails.
     */
    void failedConnect();

    /**
     * @brief Emitted when the Omron device ID is changed.
     */
    void OmronIDChanged();

    /**
     * @brief Emitted when a serial port is removed from the system.
     * @param str The name of the removed serial port.
     */
    void serialPortRemove(QString str);

    /**
     * @brief Emitted when the interval for updating values is changed.
     * @param interval The new interval in milliseconds.
     */
    void intervalUpdateChanged(int interval);

    /**
     * @brief Emitted when the interval for checking the connection is changed.
     * @param interval The new interval in milliseconds.
     */
    void intervalConectionCheckChanged(int interval);

private:
    QMainWindow *mainwindow_{nullptr};
    QStatusBar *statusBar_{nullptr};
    QModbusRtuSerialMaster* omron_{nullptr};
    QList<QSerialPortInfo> infos_;
    QModbusTcpClient* modbusDevice_{nullptr};
    QModbusReply *modbusReply_{nullptr};
    QTimer *timerUpdate_{nullptr};
    QTimer *connectTimer_{nullptr};
    QMutex mutex_;
    QString portName_{};
    QSerialPort *serialPort_{};
    int respondType_{};
    int omronID_{};
    int intervalUpdate_{3000};
    int intervalConectionCheck_{10000};
    bool modbusReady_{false};
    bool isSerialPortRemoved_{false};
    double temperature_{};
    double SV_{};
    double MV_{};
    double MVupper_{};
    double MVlower_{};
    double tempDecimal_{0.1};
    double pid_P_{};
    double pid_I_{};
    double pid_D_{};

    void Connection();
    void Run();
    void Stop();
    void sendRequestSV(double SV);
    void sendRequestAT(int afFlag);

private slots:
    void waitForMsec(int msec);
    //void read(QModbusDataUnit::RegisterType type, quint16 adress, int size);
    void readReady();
    void askStatus();
    void checkConnection();

};
#endif // COMMUNICATION_H



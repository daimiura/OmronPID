/**
 * @file safety.h
 * @brief This file contains the declaration of the Safety class, which is responsible for ensuring the safe operation of the system.
 */

#ifndef SAFETY_H
#define SAFETY_H

#include <QObject>
#include <QTimer>
#include <QMutex>
#include "datasummary.h"

/**
 * @class Safety
 * @brief The Safety class ensures the safe operation of the system.
 * This class monitors the temperature and other parameters of the system to ensure that they remain within safe limits.
 * If any parameter goes outside its safe range, the Safety class takes appropriate action to bring it back within range.
 * The class also provides a number of properties that can be used to configure its behavior.
 */
class Safety : public QObject
{
  Q_OBJECT
  Q_PROPERTY(double permitedMaxTemp READ getPermitedMaxTemp WRITE setPermitedMaxTemp NOTIFY permitedMaxTempChanged)
  Q_PROPERTY(double MVupper READ getMVUpper WRITE setMVUpper NOTIFY MVUpperChanged)
  Q_PROPERTY(int NumberOfCheck READ getNumberOfCheck WRITE setNumberOfCheck NOTIFY NumberOfCheckChanged)
  Q_PROPERTY(int checkNumber READ getCheckNumber WRITE setCheckNumber NOTIFY checkNumberChanged)
  Q_PROPERTY(double tempChangeThreshold READ getTempChangeThreshold WRITE setTempChangeThreshold NOTIFY tempChangeThresholdChanged)
  Q_PROPERTY(int intervalMVCheck READ getIntervalMVCheck WRITE setIntervalMVCheck NOTIFY intervalMVCheckChanged)
  Q_PROPERTY(int intervalTempChange READ getIntervalTempChange WRITE setIntervalTempChange NOTIFY intervalTempChangeChanged)

public:
  /**
   * @brief Constructs a new Safety object.
   * @param data A pointer to the DataSummary object that contains the system's data.
   *
   * This constructor initializes a new Safety object with the given DataSummary object pointer.
   */
  explicit Safety(DataSummary* data);

  /**
   * @brief Destroys the Safety object.
   */
  ~Safety();

  // Getter functions
  /**
  @brief Getter function for the current temperature.
  @return The current temperature.
  */
  double getTemperature() const;

  /**
  @brief Getter function for the permitted maximum temperature.
  @return The permitted maximum temperature.
  */
  double getPermitedMaxTemp() const;

  /**
  @brief Getter function for the lower bound of the manipulated variable.
  @return The upper bound of the manipulated variable.
  */
  double getMVLower() const;

  /**
  @brief Getter function for the upper bound of the manipulated variable.
  @return The upper bound of the manipulated variable.
  */
  double getMVUpper() const;

  /**
  @brief Getter function for the current value of the manipulated variable.
  @return The current value of the manipulated variable.
  */
  double getMV() const;

  /**
  @brief Getter function for the number of temperature checks.
  @return The number of temperature checks.
  */
  int getNumberOfCheck() const;

  /**
  @brief Getter function for the current check number.
  @return The current check number.
  */
  int getCheckNumber() const;

  /**
  @brief Getter function for the interval of the manipulated variable check.
  @return The interval of the manipulated variable check.
  */
  int getIntervalMVCheck() const;

  /**
  @brief Getter function for the interval of the temperature change check.
  @return The interval of the temperature change check.
  */
  int getIntervalTempChange() const;


  /**
  @brief Getter function for the threshold of temperature change.
  @return The threshold of temperature change.
  */
  double getTempChangeThreshold() const;

  /**
  @brief Getter function for the upper bound of the ignored temperature range.
  @return The upper bound of the ignored temperature range.
  */
  double getIgnoreUpper() const;

  /**
  @brief Getter function for the lower bound of the ignored temperature range.
  @return The lower bound of the ignored temperature range.
  */
  double getIgnoreLower() const;

  /**
  @brief Getter function for the ignored temperature range.
  @return The ignored temperature range.
  */
  QPair<double, double> getIgnoreTempRange() const;

  /**
  @brief Getter function for the timer object used for the manipulated variable check.
  @return The timer object used for the manipulated variable check.
  */
  QTimer* getTimerMVCheck() const;

  /**
  @brief Getter function for the timer object used for the temperature change check.
  @return The timer object used for the temperature change check.
  */
  QTimer* getTimerTempChangeCheck() const;

   // Setter functions

  /**
  @brief Sets the maximum permitted temperature.
  @param maxtemp Maximum permitted temperature.
  */
  void setPermitedMaxTemp(double maxtemp);

  /**
  @brief Sets the upper limit of the manipulated variable (MV).
  @param MVupper Upper limit of the manipulated variable (MV).
  */
  void setMVUpper(double MVupper);

  /**
  @brief Sets the value of the manipulated variable (MV).
  @param MV Value of the manipulated variable (MV).
  */
  void setMV(double MV);

  /**
  @brief Sets the number of check for the safety condition.
  @param number Number of check.
  */
  void setNumberOfCheck(int number);

  /**
  @brief Sets the check number.
  @param number Check number.
  */
  void setCheckNumber(int number);

  /**
  @brief Sets the threshold of temperature change.
  @param temp Threshold of temperature change.
  */
  void setTempChangeThreshold(double temp);

  /**
  @brief Sets the upper limit for ignoring temperature changes.
  @param upper Upper limit for ignoring temperature changes.
  */
  void setIgnoreUpper(double upper);

  /**
  @brief Sets the lower limit for ignoring temperature changes.
  @param lower Lower limit for ignoring temperature changes.
  */
  void setIgnoreLower(double lower);

  /**
  @brief Sets the temperature range for ignoring temperature changes.
  @param temp Temperature value for ignoring temperature changes.
  @param lower Lower limit of the temperature range for ignoring temperature changes.
  @param upper Upper limit of the temperature range for ignoring temperature changes.
  */
  void setIgnoreTempRange(double temp, double lower, double upper);

  /**
  @brief Checks whether the temperature has changed above the threshold value.
  */
  void checkTempChange();

  /**
  @brief Enables or disables the temperature change range.
  @param enable Boolean indicating whether the temperature change range is enabled.
  */
  void setEnableTempChangeRange(bool enable);

  /**
  @brief Sets the interval for checking the manipulated variable (MV).
  @param interval Interval for checking the manipulated variable (MV).
  */
  void setIntervalMVCheck(int interval);

  /**
  @brief Sets the interval for checking the temperature change.
  @param inteerval Interval for checking the temperature change.
  */
  void setIntervalTempChange(int inteerval);

  /**
  @brief Starts the safety monitoring.
  */
  void start();

  /**
  @brief Stops the safety monitoring.
  */
  void stop();

  /**
  @brief Checks whether the timer for checking the manipulated variable (MV) is running.
  @return Boolean indicating whether the timer for checking the manipulated variable (MV) is running.
  */
  bool isTimerMVCheckRunning() const;

  /**
  @brief Checks whether the timer for checking the temperature change is running.
  @return Boolean indicating whether the timer for checking the temperature change is running.
  */
  bool isTimerTempChangeCheckRunning() const;


signals:
  /**
   * @brief Signal emitted when the permitted maximum temperature has been changed.
   * @param maxtemp The new permitted maximum temperature.
   */
  void permitedMaxTempChanged(double maxtemp);

  /**
   * @brief Signal emitted when the MV upper limit has been changed.
   * @param MVupper The new MV upper limit.
   */
    void MVUpperChanged(double MVupper);

  /**
   * @brief Signal emitted when the MV upper limit has reached its upper limit.
   */
    void MVupperReachedUpperLimit();

  /**
   * @brief Signal emitted when the number of checks has been changed.
   * @param number The new number of checks.
   */
    void NumberOfCheckChanged(int number);

  /**
   * @brief Signal emitted when the check number has been changed.
   * @param number The new check number.
   */
    void checkNumberChanged(int number);

  /**
   * @brief Signal emitted when the temperature change threshold has been changed.
   * @param temp The new temperature change threshold.
   */
    void tempChangeThresholdChanged(double temp);

  /**
   * @brief Signal emitted when a danger is detected.
   * @param type The type of danger detected.
   */
    void dangerSignal(int type);

  /**
   * @brief Signal emitted when the interval for the MV check has been changed.
   * @param interval The new interval for the MV check.
   */
    void intervalMVCheckChanged(int interval);

  /**
   * @brief Signal emitted when the interval for the temperature change check has been changed.
   * @param interval The new interval for the temperature change check.
   */
    void intervalTempChangeChanged(int interval);

  /**
   * @brief Signal emitted when the temperature change check is cancelled.
   * @param sign The signal that cancelled the temperature change check.
   */
    void escapeTempCheckChange(int sign);

  /**
   * @brief Signal emitted when the temperature change check is started.
   * @param checknumber The check number for the temperature change check.
   */
    void startTempChangeCheck(int checknumber);

  /**
   * @brief Signal emitted when a log message is generated.
   * @param msg The log message.
   */
    void logMsg(QString msg);

  /**
   * @brief Signal emitted when a log message with a specific color is generated.
   * @param msg The log message.
   * @param color The color of the log message.
   */
    void logMsgWithColor(QString msg, QColor color);

  private slots:
  /**
   * @brief Slot that checks the temperature and handles any dangers.
   */
    void checkTemperature();

private:
    DataSummary *data_{nullptr}; /**< A pointer to the DataSummary object. */
    QMutex mutex_; /**< A mutex for thread-safety. */
    QTimer *timerMVCheck_{nullptr}; /**< A timer for checking the motor valve. */
    QTimer *timerTempChange_{nullptr}; /**< A timer for checking the temperature change. */
    int numberOfCheck_{10}; /**< The number of checks to be performed. */
    int checkNumber_{0}; /**< The current check number. */
    int intervalMVCheck_{10 * 1000}; /**< The interval for checking the motor valve, in milliseconds. */
    int intervalTempChange_{10 * 1000}; /**< The interval for checking the temperature change, in milliseconds. */
    double temperature_{}; /**< The current temperature. */
    double permitedMaxTemp_{280.0}; /**< The maximum permitted temperature. */
    double diffTemp_{}; /**< The difference between the current and previous temperature. */
    double MV_{}; /**< The current motor valve position. */
    double MVUpper_{}; /**< The upper limit for the motor valve position. */
    double ignoreLower_{-10.0}; /**< The lower limit for ignoring temperature changes. */
    double ignoreUpper_{10.0}; /**< The upper limit for ignoring temperature changes. */
    QPair<double, double> ignoreTempRange_{240.0, 260.0}; /**< The temperature range for ignoring temperature changes. */
    double tempChangeThreshold_{1.0}; /**< The temperature change threshold. */
    QVector<double> vTempHistory_{}; /**< The temperature history data. */
    QVector<double> vTempChangeData_{}; /**< The temperature change data. */
    bool isMVupper_{false}; /**< Whether the motor valve is at the upper limit. */
    bool isEnableTempChangeRange_{false}; /**< Whether to enable temperature change range checking. */

    /**
    @brief Check if the current temperature is different from the previous temperature
    @return true if the temperature has changed, false otherwise
    */
    bool isTemperatureChanged();

    /**
    @brief Check if the MV is currently at the upper limit
    @return true if MV is at the upper limit, false otherwise
    */
    bool isMVupper();

    /**
    @brief Calculate the difference between current temperature and the previous temperature
    @return the difference between the current and previous temperature
    */
    double diffTemp() const;

    /**
    @brief Calculate the difference between two temperatures
    @param temp1 the first temperature
    @param temp2 the second temperature
    @return the difference between the two temperatures
    */
    double diffTemp(double temp1, double temp2) const;

    /**
    @brief Add the current temperature to the temperature history vector
    @param temp the current temperature
    */
    void addTemperature(double temp);

    /**
    @brief Calculate the moving average of the data vector over a given window size
    @param data the data vector
    @param wsize the window size
    @return the moving average of the data vector over the window size
    */
    double movingAverage (QVector<double> data, int wsize);

};

#endif // SAFETY_H





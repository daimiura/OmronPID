#ifndef SAFETY_H
#define SAFETY_H

#include <QObject>
#include <QTimer>
#include <QMutex>
#include "datasummary.h"

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
  explicit Safety(DataSummary* data);
  ~Safety();
  double getTemperature() const;
  double getPermitedMaxTemp() const;
  double getMVUpper() const;
  double getMV() const;
  int getNumberOfCheck() const;
  int getCheckNumber() const;
  int getIntervalMVCheck() const;
  int getIntervalTempChange() const;
  double getTempChangeThreshold() const;
  double getIgnoreUpper() const;
  double getIgnoreLower() const;
  QPair<double, double> getIgnoreTempRange() const;
  QTimer* getTimerMVCheck() const;
  QTimer* getTimerTempChangeCheck() const;

  void setPermitedMaxTemp(double maxtemp);
  void setMVUpper(double MVupper);
  void setMV(double MV);
  void setNumberOfCheck(int number);
  void setCheckNumber(int number);
  void setTempChangeThreshold(double temp);
  void setIgnoreUpper(double upper);
  void setIgnoreLower(double lower);
  void setIgnoreTempRange(double temp, double lower, double upper);
  void checkTempChange();
  void setEnableTempChangeRange(bool enable);
  void setIntervalMVCheck(int interval);
  void setIntervalTempChange(int inteerval);
  void start();
  void stop();
  bool isTimerMVCheckRunning() const;
  bool isTimerTempChangeCheckRunning() const;


signals:
  void permitedMaxTempChanged(double maxtemp);
  void MVUpperChanged(double MVupper);
  void MVupperReachedUpperLimit();
  void NumberOfCheckChanged(int number);
  void checkNumberChanged(int number);
  void tempChangeThresholdChanged(double temp);
  void dangerSignal(int type);
  void intervalMVCheckChanged(int interval);
  void intervalTempChangeChanged(int interval);
  void escapeTempCheckChange(int sign);
  void startTempChangeCheck(int checknumber);
  void logMsg(QString msg);
  void logMsgWithColor(QString msg, QColor color);

private slots:
  void checkTemperature();


private:
  DataSummary *data_{nullptr};
  QMutex mutex_;
  QTimer *timerMVCheck_{nullptr};
  QTimer *timerTempChange_{nullptr};
  int numberOfCheck_{10};
  int checkNumber_{0};
  int intervalMVCheck_{10 * 1000};
  int intervalTempChange_{10 * 1000};

  double temperature_{};
  double permitedMaxTemp_{280.0};
  double diffTemp_{};
  double MV_{};
  double MVUpper_{};
  double ignoreLower_{-10.0};
  double ignoreUpper_{10.0};
  QPair<double, double> ignoreTempRange_{240.0, 260.0};
  double tempChangeThreshold_{1.0};
  QVector<double> vTempHistory_{};
  QVector<double> vTempChangeData_{};
  bool isMVupper_{false};
  bool isEnableTempChangeRange_{false};

  bool isTemperatureChanged();
  bool isMVupper();
  double diffTemp() const;
  double diffTemp(double temp1, double temp2) const;
  void addTemperature(double temp);
  double movingAverage (QVector<double> data, int wsize) const;

};

#endif // SAFETY_H





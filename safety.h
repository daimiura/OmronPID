#ifndef SAFETY_H
#define SAFETY_H

#include <QObject>
#include <QTimer>
#include <QMutex>
#include "communication.h"

class Safety : public QObject
{
  Q_OBJECT
  Q_PROPERTY(double permitedMaxTemp READ getPermitedMaxTemp WRITE setPermitedMaxTemp NOTIFY permitedMaxTempChanged);
  Q_PROPERTY(double MVupper READ getMVUpper WRITE setMVUpper NOTIFY MVUpperChanged);
  Q_PROPERTY(int NumberOfCheck READ getNumberOfCheck WRITE setNumberOfCheck NOTIFY NumberOfCheckChanged);
  Q_PROPERTY(int checkNumber READ getCheckNumber WRITE setCheckNumber NOTIFY checkNumberChanged);
  Q_PROPERTY(double tempChangeThreshold READ getTempChangeThreshold WRITE setTempChangeThreshold NOTIFY tempChangeThresholdChanged)
  Q_PROPERTY(int intervalMVCheck READ getIntervalMVCheck WRITE setIntervalMVCheck NOTIFY intervalMVCheckChanged);
  Q_PROPERTY(int intervalTempChange READ getIntervalTempChange WRITE setIntervalTempChange NOTIFY intervalTempChangeChanged);

public:
  explicit Safety(Communication* com);
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

  void setPermitedMaxTemp(double maxtemp);
  void setMVUpper(double MVupper);
  void setMV(double MV);
  void setNumberOfCheck(int number);
  void setCheckNumber(int number);
  void setTempChangeThreshold(double temp);
  void setIntervalMVCheck(int interval);
  void setIntervalTempChange(int inteerval);
  void setIgnoreUpper(double upper);
  void setIgnoreLower(double lower);
  void setIgnoreRange(double lower, double upper);
  void checkTempChange();
  void TempCheckStart(int interval = 5000);
  void setEnableTempChangeeRange(bool enable);

  void stopTimer();


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

private slots:
  void checkTemperature();

private:
  Communication *com_;
  QMutex mutex_;
  QTimer *timerMVCheck_;
  QTimer *timerTempChange_;
  int numberOfCheck_;
  int checkNumber_;
  int intervalMVCheck_;
  int intervalTempChange_;

  double temperature_;
  double permitedMaxTemp_;
  double diffTemp_;
  double MV_;
  double MVUpper_;
  double ignoreLower_;
  double ignoreUpper_;
  double tempChangeThreshold_;
  QVector<double> vTempHistory_;
  QVector<double> vTempChangeData_;
  bool isMVupper_ = false;
  bool isEnableTempChangeeRange_ = false;

  bool isTemperatureChanged();
  bool isMVupper();
  double diffTemp() const;
  double diffTemp(double temp1, double temp2) const;
  void addTemperature(double temp);
  double movingAverage (QVector<double> data, int wsize) const;

};

#endif // SAFETY_H





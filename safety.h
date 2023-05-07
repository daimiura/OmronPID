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
  Q_PROPERTY(double tempChangeThreshold READ getTempChangeThreshold WRITE setTempChangeThreshold NOTIFY tempChangeThresholdChanged)
  Q_PROPERTY(int intervalTempCheck READ getIntervalTempCheck WRITE setIntervalTempCheck NOTIFY intervalTempCheckChanged);
  Q_PROPERTY(int intervalTempChange READ getIntervalTempChange WRITE setIntervalTempChange NOTIFY intervalTempChangeChanged);

public:
  explicit Safety(Communication* com);
  ~Safety();
  QTimer *timer_; // 温度をチェックするためのタイマー
  double getTemperature() const;
  double getPermitedMaxTemp() const;
  double getMVUpper() const;
  double getMV() const;
  int getNumberOfCheck() const;
  int getIntervalTempCheck() const;
  int getIntervalTempChange() const;
  double getTempChangeThreshold() const;
  void setPermitedMaxTemp(double maxtemp);
  void setMVUpper(double MVupper);
  void setMV(double MV);
  void setNumberOfCheck(int number);
  void setTempChangeThreshold(double temp);
  void setIntervalTempCheck(int interval);
  void setIntervalTempChange(int inteerval);
  void checkTempChange();

  void TempCheckStart(int interval = 5000);


signals:
  void permitedMaxTempChanged(double maxtemp);
  void MVUpperChanged(double MVupper);
  void MVupperReachedUpperLimit();
  void NumberOfCheckChanged(int number);
  void tempChangeThresholdChanged(double temp);
  void dangerSignal(int type);
  void intervalTempCheckChanged();
  void intervalTempChangeChanged();


private slots:
  void checkTemperature();
  //void checkTempChange();

private:
  //QTimer *timer_; // 温度をチェックするためのタイマー
  Communication *com_;
  QMutex mutex_;
  QTimer *timerTempCheck_, *timerTempChange_;
  double temperature_, permitedMaxTemp_, diffTemp_, MV_, MVUpper_, tempChangeThreshold_;
  int numberOfCheck_, checkNumber_, intervalTempCheck_, intervalTempChange_;
  QVector<double> vTempHistory_, vTempChangeData_;
  bool isMVupper_ = false;
  bool isTemperatureChanged();
  bool isMVupper();
  double diffTemp() const;
  double diffTemp(double temp1, double temp2) const;
  void addTemperature(double temp);
  double movingAverage (QVector<double> data, int wsize) const;

};

#endif // SAFETY_H





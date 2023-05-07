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
public:
  explicit Safety(Communication* com);
  ~Safety();
  QTimer *timer_; // 温度をチェックするためのタイマー
  double getTemperature() const;
  double getPermitedMaxTemp() const;
  double getMVUpper() const;
  int getNumberOfCheck() const;
  double getTempChangeThreshold() const;
  void setPermitedMaxTemp(double maxtemp);
  void setMVUpper(double MVupper);
  void setNumberOfCheck(int number);
  void setTempChangeThreshold(double temp);

   void checkTempChange();


signals:
  void permitedMaxTempChanged(double maxtemp);
  void MVUpperChanged(double MVupper);
  void MVupperReachedUpperLimit();
  void NumberOfCheckChanged(int number);
  void tempChangeThresholdChanged(double temp);
  void dangerSignal(int type);

private slots:
  void checkTemperature();
  //void checkTempChange();

private:
  //QTimer *timer_; // 温度をチェックするためのタイマー
  Communication *com_;
  QMutex mutex_;
  QTimer *timer2_;
  double temperature_; // 最新の温度データ
  double permitedMaxTemp_;
  double diffTemp_;
  double MVUpper_;
  int numberOfCheck_;
  int checkNumber_;
  double tempChangeThreshold_;
  QVector<double> vTempHistory_;
  QVector<double> vTempChangeData_;
  bool isTemperatureChanged();
  double diffTemp() const;
  double diffTemp(double temp1, double temp2) const;
  void addTemperature(double temp);
  double movingAverage (QVector<double> data, int wsize) const;

};

#endif // SAFETY_H





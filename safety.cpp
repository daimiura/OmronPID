#include "safety.h"

Safety::Safety(Communication* com)
    : com_(com),
      numberOfCheck_(10),
      checkNumber_(0),
      timerTempCheck_(new QTimer(this)),
      timerTempChange_(new QTimer(this))
{
  vTempChangeData_.clear();
  timerTempCheck_->setInterval(4000);
  timerTempChange_->setInterval(5000);
  connect(timerTempCheck_, &QTimer::timeout, this, &Safety::checkTemperature);
  connect(timerTempCheck_, &QTimer::timeout, this, &Safety::isMVupper);
  connect(timerTempChange_, &QTimer::timeout, this, &Safety::checkTempChange);
  connect(this, &Safety::permitedMaxTempChanged, this, &Safety::setPermitedMaxTemp);
  connect(this, &Safety::MVUpperChanged, this, &Safety::setMVUpper);
  connect(this, &Safety::NumberOfCheckChanged, this, &Safety::setNumberOfCheck);
  connect(this, &Safety::tempChangeThresholdChanged, this, &Safety::setTempChangeThreshold);
  connect(this, &Safety::MVupperReachedUpperLimit, this, &Safety::checkTempChange);
}

Safety::~Safety(){
  delete timerTempCheck_;
  delete timerTempChange_;
  delete com_;
  mutex_.unlock();
  vTempChangeData_.clear();
}

void Safety::checkTemperature(){
  QMutexLocker locker(&mutex_);
  temperature_ = com_->getTemperature();
  addTemperature(temperature_);
  if (temperature_ >= permitedMaxTemp_) emit dangerSignal(0);
  diffTemp_ = diffTemp();
}

void Safety::checkTempChange() {
  timerTempChange_->start();
  qDebug() << "checkTempCange at " << checkNumber_;
  if (!isMVupper_) {
      checkNumber_ = 0;
      vTempChangeData_.clear();
      return;
  }

  QMutexLocker locker(&mutex_);
  if (checkNumber_ <= numberOfCheck_){
    vTempChangeData_.push_back(com_->getTemperature());
    checkNumber_ ++;
    return;
  }

  QVector<double> vdiff;
  for (auto i = 0; i <vTempChangeData_.size()-1; i++) vdiff.push_back(diffTemp(vTempChangeData_[i+1], vTempChangeData_[i]));
  double ave = movingAverage(vdiff, 3);
  if (ave <= tempChangeThreshold_) {
    emit dangerSignal(1);
    timerTempChange_->stop(); // 温度取得を停止
    timerTempCheck_->stop(); // 温度取得を停止
  } else {
    timerTempCheck_->stop();
  }
  vdiff.clear();
  vTempChangeData_.clear();
}

double Safety::movingAverage(QVector<double> data, int wsize) const {
  qDebug() << "Calculation start";
  double sum = 0.0;
  for (int i = 0; i < wsize; i++) sum += data[i];
  double avg = sum / wsize;
  for (int i = wsize; i < data.size(); i++) {
      sum += data[i] - data[i-wsize];
      avg = sum / wsize;
  }
  qDebug() << "Calculation end/ The average value is " << avg;
  return avg;
}


bool Safety::isMVupper(){
  setMV(com_->getMV());
  setMVUpper(com_->getMVupper());
  if (MV_ == MVUpper_) {
      qDebug() << "MV reached MVupper";
      isMVupper_ = true;
      emit MVupperReachedUpperLimit();
  } else {
      qDebug() << "MV < MVupper";
      isMVupper_ = false;
  }
  return isMVupper_;
}

void Safety::addTemperature(double temp){
  vTempHistory_.push_back(temp);
  if (vTempHistory_.size() > 100) vTempHistory_.remove(0);
}

double Safety::diffTemp() const {
  if (vTempHistory_.size() < 2) return .0;
  double currentTemp = vTempHistory_.last();
  double oldTemp = vTempHistory_[vTempHistory_.size()-2];
  return currentTemp - oldTemp;
}

void Safety::TempCheckStart(int interval){
  setIntervalTempCheck(interval);
  timerTempCheck_->start(intervalTempCheck_);
}

double Safety::diffTemp(double temp1, double temp2) const {return temp1 - temp2;}
double Safety::getTemperature() const {return temperature_;}
double Safety::getPermitedMaxTemp() const {return permitedMaxTemp_;}
double Safety::getMVUpper() const {return MVUpper_;}
double Safety::getMV() const {return MV_;}
double Safety::getTempChangeThreshold() const {return tempChangeThreshold_;}
int Safety::getNumberOfCheck() const {return numberOfCheck_;}
int Safety::getIntervalTempCheck() const {return intervalTempCheck_;}
int Safety::getIntervalTempChange() const {return intervalTempChange_;}

void Safety::setPermitedMaxTemp(double maxtemp) {permitedMaxTemp_ = maxtemp;}
void Safety::setMVUpper(double MVupper) {MVUpper_ = MVupper;}
void Safety::setMV(double MV) {MV_ = MV;}
void Safety::setNumberOfCheck(int number) {numberOfCheck_ = number;}
void Safety::setTempChangeThreshold(double temp){tempChangeThreshold_ = temp;}
void Safety::setIntervalTempCheck(int interval) {intervalTempCheck_ = interval;}
void Safety::setIntervalTempChange(int interval) {intervalTempChange_ = interval;}

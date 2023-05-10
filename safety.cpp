#include "safety.h"

Safety::Safety(Communication* com)
    : com_(com),
      timerMVCheck_(new QTimer(this)),
      timerTempChange_(new QTimer(this)),
      intervalTempChange_(10000),
      numberOfCheck_(10),
      checkNumber_(0),
      intervalMVCheck_(10000)
{
  vTempChangeData_.clear();
  timerMVCheck_->setInterval(intervalMVCheck_);
  timerTempChange_->setInterval(intervalTempChange_);
  connect(timerMVCheck_, &QTimer::timeout, this, &Safety::isMVupper);
  connect(timerTempChange_, &QTimer::timeout, this, &Safety::checkTempChange);
  connect(this, &Safety::permitedMaxTempChanged, this, &Safety::setPermitedMaxTemp);
  connect(this, &Safety::MVUpperChanged, this, &Safety::setMVUpper);
  connect(this, &Safety::NumberOfCheckChanged, this, &Safety::setNumberOfCheck);
  connect(this, &Safety::tempChangeThresholdChanged, this, &Safety::setTempChangeThreshold);
  connect(this, &Safety::MVupperReachedUpperLimit, this, &Safety::checkTempChange);
  connect(this, &Safety::intervalMVCheckChanged, this, &Safety::setIntervalTempChange);
}


Safety::~Safety(){
  delete timerMVCheck_;
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
  if (!isMVupper_) {
    checkNumber_ = 0;
    vTempChangeData_.clear();
    timerTempChange_->stop();
    emit escapeTempCheckChange(0);
    return;
  }
  double temp = com_->getTemperature();
  if (temp >= ignoreLower_ && temp <= ignoreUpper_){
    checkNumber_ = 0;
    vTempChangeData_.clear();
    timerTempChange_->stop();
    emit escapeTempCheckChange(1);
    return;
  }

  emit startTempChangeCheck(checkNumber_);
  QMutexLocker locker(&mutex_);
  timerTempChange_->start();
  qDebug() << "checkTempCange at " << checkNumber_;
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
    timerMVCheck_->stop(); // 温度取得を停止
  } else {
    timerMVCheck_->stop();
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
  setIntervalMVCheck(interval);
  timerMVCheck_->start(intervalMVCheck_);
}

void Safety::stopTimer(){
  timerMVCheck_->stop();
  timerTempChange_->stop();
}


double Safety::diffTemp(double temp1, double temp2) const {return temp1 - temp2;}
double Safety::getTemperature() const {return temperature_;}
double Safety::getPermitedMaxTemp() const {return permitedMaxTemp_;}
double Safety::getMVUpper() const {return MVUpper_;}
double Safety::getMV() const {return MV_;}
double Safety::getTempChangeThreshold() const {return tempChangeThreshold_;}
double Safety::getIgnoreLower() const {return ignoreLower_;}
double Safety::getIgnoreUpper() const {return ignoreUpper_;}
int Safety::getNumberOfCheck() const {return numberOfCheck_;}
int Safety::getCheckNumber() const {return checkNumber_;}
int Safety::getIntervalMVCheck() const {return intervalMVCheck_;}
int Safety::getIntervalTempChange() const {return intervalTempChange_;}


void Safety::setPermitedMaxTemp(double maxtemp) {permitedMaxTemp_ = maxtemp;}
void Safety::setMVUpper(double MVupper) {MVUpper_ = MVupper;}
void Safety::setMV(double MV) {MV_ = MV;}
void Safety::setNumberOfCheck(int number) {numberOfCheck_ = number;}
void Safety::setCheckNumber(int number) {checkNumber_ = number;}
void Safety::setTempChangeThreshold(double temp){tempChangeThreshold_ = temp;}
void Safety::setIntervalMVCheck(int interval) {intervalMVCheck_ = interval;}
void Safety::setIntervalTempChange(int interval) {intervalTempChange_ = interval;}
void Safety::setEnableTempChangeeRange (bool enable) {isEnableTempChangeeRange_ = enable;}
void Safety::setIgnoreLower(double lower) {ignoreLower_ = lower;}
void Safety::setIgnoreUpper(double upper) {ignoreUpper_ = upper;}
void Safety::setIgnoreRange(double lower, double upper){ignoreLower_ = lower; ignoreUpper_ = upper;}


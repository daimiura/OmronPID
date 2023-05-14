#include "safety.h"

Safety::Safety(DataSummary* data)
{
  data_ = data;
  timerMVCheck_ = new QTimer(this);
  timerTempChange_ = new QTimer(this);
  timerMVCheck_->setInterval(intervalMVCheck_);
  timerTempChange_->setInterval(intervalTempChange_);
  connect(timerMVCheck_, &QTimer::timeout, this, &Safety::isMVupper);
  connect(timerMVCheck_, &QTimer::timeout, this, &Safety::checkTemperature);
  connect(timerTempChange_, &QTimer::timeout, this, &Safety::checkTempChange);
  connect(this, &Safety::permitedMaxTempChanged, this, &Safety::setPermitedMaxTemp);
  connect(this, &Safety::MVUpperChanged, this, &Safety::setMVUpper);
  connect(this, &Safety::NumberOfCheckChanged, this, &Safety::setNumberOfCheck);
  connect(this, &Safety::tempChangeThresholdChanged, this, &Safety::setTempChangeThreshold);
  connect(this, &Safety::MVupperReachedUpperLimit, this, &Safety::checkTempChange);

}


Safety::~Safety(){
  delete timerMVCheck_;
  delete timerTempChange_;
  delete data_;
  mutex_.unlock();
  vTempChangeData_.clear();
}

void Safety::checkTemperature(){
  QMutexLocker locker(&mutex_);
  temperature_ = data_->getTemperature();
  addTemperature(temperature_);
  if (temperature_ >= permitedMaxTemp_) emit dangerSignal(0);
  diffTemp_ = diffTemp();
}


/**

@brief Check if the temperature has changed more than the threshold value.
This function checks if the temperature has changed more than the threshold value during the specified time interval.
If the temperature has changed less than the threshold value,
it emits a danger signal and stops the temperature change check timer and MV check timer.
@note This function must be called periodically by a QTimer object.
@details
This function periodically checks if the temperature has changed more than the threshold value during the specified time interval.
If the temperature has changed less than the threshold value, it emits a danger signal and stops the temperature change check timer and MV check timer.
The function starts by acquiring the current temperature and checking
if it's within the ignore range. If the temperature is within the ignore range, the function clears variables, stops the timer, and emits the escapeTempCheckChange signal with the argument 1, indicating that the temperature change check has been escaped.
If the temperature is outside the ignore range, the function checks if the conditions for stopping the temperature change check have been met. The conditions are: if the check number is equal to or greater than the number of checks minus one, or if the MV is not in the upper limit, or if the temperature is within the ignore range. If any of these conditions is true, the function clears variables, stops the timer, and emits the escapeTempCheckChange signal with the argument 0 if the MV is not in the upper limit, or 1 if the MV is in the upper limit.
If none of the stopping conditions is met, the function starts the check and pushes temperature data into the vTempChangeData_ vector. The function sets the timer and increments the check number. If the check number is less than the number of checks, the function returns.
When the check is completed, the function calculates the differences between adjacent temperature values, calculates the moving average, and checks if it's below the threshold. If it's below the threshold, the function emits the dangerSignal with the argument 1, indicating a dangerous situation. The function then clears variables, stops the timer, and resets the check number.
*/
void Safety::checkTempChange() {
  QMutexLocker locker(&mutex_);
  double sv = data_->getSV();
  double temp = data_->getTemperature();
  setIgnoreTempRange(sv, getIgnoreLower(), getIgnoreUpper());
  const double lower = ignoreTempRange_.first;
  const double upper = ignoreTempRange_.second;
  if (isEnableTempChangeRange_ && temp > lower && temp < upper){
    checkNumber_ = 0;
    vTempChangeData_.clear();
    timerTempChange_->stop();
    emit escapeTempCheckChange(1);
    return;
  }

  if (checkNumber_ >= numberOfCheck_ - 1 || (!isMVupper_)) {
    checkNumber_ = 0;
    vTempChangeData_.clear();
    timerTempChange_->stop();
    if (!isMVupper_) {
      emit escapeTempCheckChange(0);
    } else {
      emit escapeTempCheckChange(1);
    }
    return;
  }

  // Start the check and push temperature data into vTempChangeData_
  emit startTempChangeCheck(checkNumber_);
  timerTempChange_->start();
  qDebug() << "checkTempChange at " << checkNumber_;
  if (checkNumber_ < numberOfCheck_) {
    vTempChangeData_.push_back(temp);
    checkNumber_++;
    return;
  }

  // Calculate the differences between adjacent temperature values
  QVector<double> vdiff;
  for (int i = 0; i < vTempChangeData_.size() - 1; i++) {
    vdiff.push_back(diffTemp(vTempChangeData_[i + 1], vTempChangeData_[i]));
  }

  // Calculate the moving average and emit dangerSignal if it's below the threshold
  double ave = movingAverage(vdiff, 3);
  if (ave <= tempChangeThreshold_) {
    emit dangerSignal(1);
    timerTempChange_->stop();
    timerMVCheck_->stop();
  }
  // Clear variables and reset checkNumber_
  vdiff.clear();
  checkNumber_ = 0;
  vTempChangeData_.clear();
  timerTempChange_->stop();
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
  qDebug() << "Safey function works every " << intervalMVCheck_ ;
  setMV(data_->getMV());
  setMVUpper(data_->getMVUpper());
  if (MV_ >= MVUpper_) {
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

void Safety::start(){
  checkNumber_ = 0;
  timerMVCheck_->setInterval(intervalMVCheck_);
  timerMVCheck_->start();
}

void Safety::stop(){
  timerMVCheck_->stop();
  timerTempChange_->stop();
  checkNumber_ = 0;
}

bool Safety::isTimerMVCheckRunning() const {return timerMVCheck_->isActive();}
bool Safety::isTimerTempChangeCheckRunning() const {return timerTempChange_->isActive();}

double Safety::diffTemp(double temp1, double temp2) const {return temp1 - temp2;}
double Safety::getTemperature() const {return temperature_;}
double Safety::getPermitedMaxTemp() const {return permitedMaxTemp_;}
double Safety::getMVUpper() const {return MVUpper_;}
double Safety::getMV() const {return MV_;}
double Safety::getTempChangeThreshold() const {return tempChangeThreshold_;}
double Safety::getIgnoreLower() const {return ignoreLower_;}
double Safety::getIgnoreUpper() const {return ignoreUpper_;}
QPair<double, double> Safety::getIgnoreTempRange() const {return ignoreTempRange_;}
QTimer* Safety::getTimerMVCheck() const {return timerMVCheck_;}
QTimer* Safety::getTimerTempChangeCheck() const {return timerTempChange_;}
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
void Safety::setIntervalMVCheck(int interval) {
  qDebug() << "The arggument is " << interval;
  intervalMVCheck_ = interval*1000;
  qDebug() << "Current interval MV check is " << intervalMVCheck_;
  timerMVCheck_->setInterval(intervalMVCheck_);
}
void Safety::setIntervalTempChange(int interval) {
  intervalTempChange_ = interval*1000;
  timerTempChange_->setInterval(intervalTempChange_);
}
void Safety::setEnableTempChangeRange (bool enable) {isEnableTempChangeRange_ = enable;}
void Safety::setIgnoreLower(double lower) {ignoreLower_ = lower;}
void Safety::setIgnoreUpper(double upper) {ignoreUpper_ = upper;}
void Safety::setIgnoreTempRange(double temp, double lower, double upper){
  if (lower > upper){
      qWarning() << "Error: lower value greater than upper value in setIgnoreTempRange";
  }
  qDebug() << temp+lower << "  " << temp << "  " << temp +upper ;
  ignoreTempRange_ = qMakePair(temp + lower, temp + upper);
}


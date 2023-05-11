#include "datasummary.h"

DataSummary::DataSummary(Communication* com)
    : com_(com),
      logTimer_(new QTimer(this))
{
  //Check Temp Directory, is not exist, create
  QDir myDir;
  myDir.setPath(dataPath_);
  filePath_ = dataPath_;
  // if Z:/triplet/Temp_Record does not exist, make directory on desktop.
  if(!myDir.exists()){
    myDir.mkpath(dataPath2_);
    filePath_ = dataPath2_;
  }
  QDateTime startTime = QDateTime::currentDateTime();
  fileName_ = filePath_ + "/" + startTime.toString("yyyyMMdd_HHmmss") + ".dat";
  connect(com_, &Communication::TemperatureUpdated, this, &DataSummary::setTemperature);
  connect(com_, &Communication::MVUpdated, this, &DataSummary::setMV);
  connect(com_, &Communication::SVUpdated, this, &DataSummary::setSV);
  connect(logTimer_, &QTimer::timeout, this, &DataSummary::writeData);
}

double DataSummary::getTemperature() const {return temperature_;}
double DataSummary::getMV() const {return mv_;}
double DataSummary::getSV() const {return sv_;}
QString DataSummary::getFileName() const {return fileName_;}
QString DataSummary::getFilePath() const {return filePath_;}

void DataSummary::setTemperature(double temperature){temperature_ = temperature;}
void DataSummary::setMV(double mv){mv_ = mv;}
void DataSummary::setSV(double sv){sv_ = sv;}
void DataSummary::setFileName(QString name) {fileName_ = name;}
void DataSummary::setSave(bool save) {save_ = save;}
void DataSummary::setIntervalLog(int interval) {
  intervalLog_ = interval * 1000;
  if (logTimer_->isActive()){
    logTimer_->stop();
    logTimer_->setInterval(intervalLog_);
    logTimer_->start();
  } else {
    logTimer_->stop();
    logTimer_->setInterval(intervalLog_);
  }
}

bool DataSummary::generateSaveFile(){
  QFile output(fileName_);
  QTextStream stream(&output);
  if (output.exists()) {
    output.close();
    return false;
  }else {
    output.open(QIODevice::WriteOnly| QIODevice::Text);
    stream <<"Date\t"<<"Date_t\t"<<"temp [C]\t"<<"SV [C]\t"<<"Output [%]" <<Qt::endl;
    output.close();
    return true;
  }
}

void DataSummary::writeData(){
  if (!save_) return;
  QFile output(fileName_);
  qDebug () << fileName_;
  QTextStream stream(&output);
  if (!output.exists()){
    output.open(QIODevice::WriteOnly| QIODevice::Text);
    generateSaveFile();
  }
  QDateTime date = QDateTime::currentDateTime();
  output.open(QIODevice::Append| QIODevice::Text);
  stream << date.toString("MM-dd HH:mm:ss").toStdString().c_str()
         << "\t"
         << date.toSecsSinceEpoch()
         << "\t"
         << QString::number(getTemperature())
         << "\t"
         << QString::number(getSV())
         << "\t"
         << QString::number(getMV())
         << Qt::endl;
  output.close();
  emit FileSave(true);
}

void DataSummary::logingStart(){
  if(logTimer_->isActive()) logTimer_->stop();
  logTimer_->start();
}

void DataSummary::logingStop(){
  logTimer_->stop();
}



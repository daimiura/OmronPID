#include "datasummary.h"

/**
 * @brief Constructor for DataSummary class.
 *
 * Initializes the object and sets up the file path and name for saving data. Also connects the object to the Communication signals
 * to update the stored temperature, setpoint value, and manipulated variable. Starts a timer for logging data.
 *
 * @param com Pointer to Communication class.
 */
DataSummary::DataSummary(Communication* com)
    : com_(com),
      logTimer_(new QTimer(this))
{
  QDir myDir;
  myDir.setPath(dataPath_);
  filePath_ = dataPath_;
  qDebug() << "Default DATAPATH : " << filePath_;
  if(!myDir.exists()){
    myDir.mkpath(dataPath2_);
    filePath_ = dataPath2_;
    qDebug() << "Cannnot connect Z drive : " << filePath_;
  }
  QDateTime startTime = QDateTime::currentDateTime();
  fileName_ = filePath_ + "/" + startTime.toString("yyyyMMdd_HHmmss") + ".dat";
  connect(com_, &Communication::TemperatureUpdated, this, &DataSummary::setTemperature);
  connect(com_, &Communication::MVUpdated, this, &DataSummary::setMV);
  connect(com_, &Communication::MVupperUpdated, this, &DataSummary::setMVUpper);
  connect(com_, &Communication::MVlowerUpdated, this, &DataSummary::setMVLower);
  connect(com_, &Communication::SVUpdated, this, &DataSummary::setSV);
  connect(logTimer_, &QTimer::timeout, this, &DataSummary::writeData);
}

double DataSummary::getTemperature() const {return temperature_;}
double DataSummary::getMV() const {return mv_;}
double DataSummary::getMVUpper() const {return mvUpper_;}
double DataSummary::getMVLower() const {return mvLower_;}
double DataSummary::getSV() const {return sv_;}
QString DataSummary::getFileName() const {return fileName_;}
QString DataSummary::getFilePath() const {return filePath_;}
QTimer* DataSummary::getLogTimer() const {return logTimer_;}

void DataSummary::setTemperature(double temperature){temperature_ = temperature;}
void DataSummary::setMV(double mv){mv_ = mv;}
void DataSummary::setMVUpper(double mvUpper) {mvUpper_ = mvUpper;}
void DataSummary::setMVLower(double mvLower) {mvLower_ = mvLower;}
void DataSummary::setSV(double sv){sv_ = sv;}
void DataSummary::setFileName(QString name) {
  qDebug() << "File path to save is chanded....";
  fileName_ = name;
  qDebug() << "The path to save" << fileName_;
}
void DataSummary::setSave(bool save) {save_ = save;}
bool DataSummary::isTimerLogRunning() const {return logTimer_ -> isActive();}

/**
 * @brief Sets the logging interval for the DataSummary class.
 *
 * @param interval The logging interval in seconds.
 *
 * This method sets the logging interval for the DataSummary class. The logging interval
 * is specified in seconds, and is converted to milliseconds and stored in the intervalLog_
 * member variable. If the logging timer is already running, the method stops the timer,
 * updates the interval, and restarts the timer with the new interval. If the timer is not
 * running, the method only updates the interval.
 */
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


/**

@brief Generate a new save file if it does not already exist.
@return true if the file is successfully generated, false otherwise.
*/
bool DataSummary::generateSaveFile(){
  QFile output(fileName_);
  qDebug() << "SAVE to " << fileName_;
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

/**

@brief Writes temperature, MV and SV values to the data file.
The data is saved in the following format:
<date and time>\t<seconds since epoch>\t<temperature>\t<MV>\t<SV>
@details The method checks whether the saving of data has been enabled.
If not, the method exits. The data is written to a file specified by fileName_.
If the file does not exist, a new file is created by calling generateSaveFile().
The temperature, MV and SV values are retrieved by calling getTemperature(), getMV()
and getSV() respectively. The date and time of writing the data is obtained using
QDateTime::currentDateTime() method. The data is written in the text format.
@return void
*/
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
}

void DataSummary::logingStart(){
  if(logTimer_->isActive()) logTimer_->stop();
  logTimer_->start();
}

void DataSummary::logingStop(){
  logTimer_->stop();
}

/*
void DataSummary::writeHeader(QTextStream& stream, QDateTime startTime, int mode, double targetValue, double targetValue_2, int tempWaitTime, double tempTorr){
  QString lineout = "###" + startTime.toString("yyyy-MM-dd HH:mm:ss\n");
  stream << lineout;
  if (mode == 1) {
      lineout = "### Control mode          :  Stable Temperature.\n";
      stream << lineout;
      lineout = "### Target Temperature    : " + QString::number(targetValue) + " C.\n";
      stream << lineout;
      lineout = "### Temperature stable time: " + QString::number(tempWaitTime) + " min.\n";
      stream << lineout;
      lineout = "### Temperature tolerance : " + QString::number(tempTorr) + " C.\n";
      stream << lineout;
  } else if (mode == 2) {
      lineout = "### Control mode          :  Fixed Time.\n";
      stream << lineout;
      lineout = "### Target Temperature    : " + QString::number(targetValue) + " C.\n";
      stream << lineout;
      lineout = "### Set-temp change time  : " + QString::number(tempWaitTime) + " min.\n";
      stream << lineout;
  } else if (mode == 3) {
      lineout = "### Control mode          :  Set-temp Fixed Rate. \n";
      stream << lineout;
      lineout = "### Target Temperature    : " + QString::number(targetValue) + " C.\n";
      stream << lineout;
      lineout = "### Set-temp change rate  : " + QString::number(tempWaitTime/60./1000.) + " min/C.\n";
      stream << lineout;
  } else if (mode == 4) {
      lineout = "### Control mode          :  Normal + Set-temp Fixed Rate. \n";
      stream << lineout;
      lineout = "### Set-temp of normal    : " + QString::number(targetValue_2) + " C.\n";
      stream << lineout;
      lineout = "### Set-temp of fixed rate: " + QString::number(targetValue) + " C.\n";
      stream << lineout;
      lineout = "### Set-temp change rate  : " + QString::number(tempWaitTime/60./1000.) + " min/C.\n";
      stream << lineout;
  }

  lineout = "###" + QString("%1,\t%2,\t%3,\t%4,\t%5\n").arg("Date").arg("Date_t").arg("temp [C]").arg("SV [C]").arg("Output [%]");
  stream << lineout;
  }
}
*/

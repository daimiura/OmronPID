#include "datasummary.h"
DataSummary::DataSummary(Communication* com)
    : com_(com)
{
  //Check Temp Directory, is not exist, create
  QDir myDir;
  myDir.setPath(DATA_PATH);
  filePath_ = DATA_PATH;
  // if Z:/triplet/Temp_Record does not exist, make directory on desktop.
  if(!myDir.exists()){
    myDir.mkpath(DATA_PATH_2);
    filePath_ = DATA_PATH_2;
  }
  connect(com_, &Communication::TemperatureUpdated, this, &DataSummary::setTemperature);
  connect(com_, &Communication::MVUpdated, this, &DataSummary::setMV);
  connect(com_, &Communication::SVUpdated, this, &DataSummary::setSV);
}

double DataSummary::getTemperature() const {return temperature_;}
double DataSummary::getMV() const {return mv_;}
double DataSummary::getSV() const {return sv_;}

void DataSummary::setTemperature(double temperature){temperature_ = temperature;}
void DataSummary::setMV(double mv){mv_ = mv;}
void DataSummary::setSV(double sv){sv_ = sv;}
void DataSummary::setFileName(QString name) {fileName_ = name;}
void DataSummary::setSave(bool save) {save_ = save;}

bool DataSummary::generateSaveFile(){
  QFile output(fileName_);
  QTextStream stream(&output);
  if (output.exists()) {
    output.close();
    emit FileSave(false);
    return false;
  }else {
    output.open(QIODevice::WriteOnly| QIODevice::Text);
    stream <<"Date\t"<<"Date_t\t"<<"temp [C]\t"<<"SV [C]\t"<<"Output [%]" <<Qt::endl;
    output.close();
    emit FileSave(true);
    return true;
  }
}

int DataSummary::writeData(){
  if (!save_) return -1;
  QFile output(fileName_);
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
  return 1;
}



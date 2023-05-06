#include <QSerialPortInfo>
#include <QModbusRtuSerialMaster>
#include <QException>
#include <QDebug>
#include <QTimer>
#include <QEventLoop>
#include "communication.h"


Communication::Communication(QMainWindow *parent, QStatusBar *statusBar)
    : QObject(parent),
      omron_(nullptr),
      modbusDevice_(nullptr),
      modbusReply_(nullptr),
      connectTimer_(new QTimer(this)),
      statusBar_(statusBar)
{

  mutex_.lock();
  mainwindow_ = qobject_cast<MainWindow*>(parent);
  // シリアルポートの検索とコンボボックスへの追加
  const auto infos = QSerialPortInfo::availablePorts();
  infos_ = infos;
  omron_= new QModbusRtuSerialMaster(this);
  // タイマーの設定
  connectTimer_->setSingleShot(true);
  connect(connectTimer_, &QTimer::timeout, this, &Communication::connectTimeout);
}

Communication::~Communication(){
    mutex_.unlock();
    delete omron_;
}

void Communication::waitForMsec(int msec){
    QEventLoop eventLoop;
    QTimer::singleShot(msec, &eventLoop, SLOT(quit()));
    eventLoop.exec();
}
/*
void Communication::connectTimeout(){
    emit logMsg("Connection timeout.");
}

void Communication::errorOccurred(QModbusDevice::Error error){
    emit logMsg("Modbus error occurred: " + QString(error));
}

void Communication::stateChanged(QModbusDevice::State state){
    emit logMsg("Modbus state changed: " + QString(state));
}
*/

void Communication::request(QModbusPdu::FunctionCode code, QByteArray cmd){
  statusBar_->clearMessage();
  modbusReady_ = false;
  QModbusPdu pdu;
  pdu.setFunctionCode(code);
  pdu.setData(cmd);
  QModbusRequest ask(pdu);
  if (auto *reply = omron_->sendRawRequest(ask, omronID_)) {
      if (!reply->isFinished()) {
          connect(reply, &QModbusReply::finished, this, [this, reply]() {
              if (reply->error() == QModbusDevice::ProtocolError) {
                  statusBar_->showMessage(tr("Write response error: %1 (Mobus exception: 0x%2)")
                      .arg(reply->errorString()).arg(reply->rawResult().exceptionCode(), -1, 16), 0);
              } else if (reply->error() != QModbusDevice::NoError) {
                  statusBar_->showMessage(tr("Write response error: %1 (code: 0x%2)").
                      arg(reply->errorString()).arg(reply->error(), -1, 16), 0);
              }
              reply->deleteLater();
              modbusReady_ = true;
          });
      } else {
          // broadcast replies return immediately
          reply->deleteLater();
      }
  } else {
      statusBar_->showMessage(tr("Write error: ") + omron_->errorString(), 0);
  }
}

void Communication::read(QModbusDataUnit::RegisterType type, quint16 address, int size) {
  QModbusDataUnit ans(type, address, size);
  respondType_ = address;
  modbusReady_ = false;
  if (auto *reply = omron_->sendReadRequest(ans, omronID_)) {
      if (!reply->isFinished()){
          connect(reply, &QModbusReply::finished, this, &Communication::readReady);
      }else{
          delete reply; // broadcast replies return immediately
      }
  } else {
      statusBar_->showMessage(tr("Read error: ") + omron_->errorString(), 0);
  }
}

void Communication::readReady(){
  auto reply = qobject_cast<QModbusReply *>(sender());
  if (!reply) return;
  if (reply->error() == QModbusDevice::ProtocolError) {
  statusBar_->showMessage(tr("Read response error: %1 (Mobus exception: 0x%2)").
                              arg(reply->errorString()).
                              arg(reply->rawResult().exceptionCode(), -1, 16), 0);
  } else {
  statusBar_->showMessage(tr("Read response error: %1 (code: 0x%2)").
                              arg(reply->errorString()).
                              arg(reply->error(), -1, 16), 0);
  }
  switch (respondType_){
  case static_cast<int>(E5CC_Address::Type::PV): {
      const QModbusDataUnit unit = reply->result();
      temperature_ = QString::number(unit.value(1), 10).toDouble() * tempDecimal_;
      break;
  }
  case static_cast<int>(E5CC_Address::Type::SV):{
      const QModbusDataUnit unit = reply->result();
      SV_ = QString::number(unit.value(1), 10).toDouble() * tempDecimal_;
      break;
  }
  case static_cast<int>(E5CC_Address::Type::MV): {
      const QModbusDataUnit unit = reply->result();
      MV_ = QString::number(unit.value(1), 10).toDouble() * tempDecimal_;
      break;
  }
  case static_cast<int>(E5CC_Address::Type::MVupper): {
      const QModbusDataUnit unit = reply->result();
      MVupper_ = QString::number(unit.value(1), 10).toDouble() * tempDecimal_;
      break;
  }
  case static_cast<int>(E5CC_Address::Type::MVlower): {
      const QModbusDataUnit unit = reply->result();
      MVlower_ = QString::number(unit.value(1), 10).toDouble() * tempDecimal_;
      break;
  }
  case static_cast<int>(E5CC_Address::Type::PID_P): {
      const QModbusDataUnit unit = reply->result();
      pid_P_ = QString::number(unit.value(1), 10).toDouble() * 0.1;
      break;
  }
  case static_cast<int>(E5CC_Address::Type::PID_I): {
      const QModbusDataUnit unit = reply->result();
      pid_I_ = QString::number(unit.value(1), 10).toDouble();
      break;
  }
  case static_cast<int>(E5CC_Address::Type::PID_D): {
      const QModbusDataUnit unit = reply->result();
      pid_D_ = QString::number(unit.value(1), 10).toDouble();
      break;
  }
  default: {
      const QModbusDataUnit unit = reply->result();
      emit logMsg("respond count: " + QString::number(unit.valueCount()));
      for (uint i = 0; i < unit.valueCount(); i++) {
          const QString entry = tr("Address: %1, Value: %2").arg(unit.startAddress()).arg(QString::number(unit.value(i), 10));
          emit logMsg(entry);
      }
      break;
  }
  }
  reply->deleteLater();
  modbusReady_ = true;
}

QString Communication::formatHex(int value, int digit){
    QString valueStr = QString::number(value, 16).toUpper();
    while(valueStr.size() < digit){
        valueStr.insert(0, "0");
    }
    return valueStr;
}

QString Communication::formatE5CCAddress(E5CC_Address::Type address, int width){
    return formatHex(static_cast<int>(address), width);
}

void Communication::Connection(){
    omron_= new QModbusRtuSerialMaster(this);
    omron_->setConnectionParameter(QModbusDevice::SerialPortNameParameter, portName_);
    omron_->setConnectionParameter(QModbusDevice::SerialBaudRateParameter, QSerialPort::Baud9600);
    omron_->setConnectionParameter(QModbusDevice::SerialDataBitsParameter, QSerialPort::Data8);
    omron_->setConnectionParameter(QModbusDevice::SerialParityParameter, QSerialPort::NoParity);
    omron_->setConnectionParameter(QModbusDevice::SerialStopBitsParameter, QSerialPort::TwoStop);
    omron_->setTimeout(timing::timeOut);
    omron_->setNumberOfRetries(0);
    if(omron_->connectDevice()){
     emit deviceConnect();
     QString cmd = "00 00 01 01";
     QByteArray value = QByteArray::fromHex(cmd.toStdString().c_str());
     request(QModbusPdu::WriteSingleRegister, value);
    }else{
      emit failedConnect();
    }
}

void Communication::Run(){
    QString cmd = "00 00 01 00";
    QByteArray value = QByteArray::fromHex(cmd.toStdString().c_str());
    request(QModbusPdu::WriteSingleRegister, value);
}

void Communication::sendRequestAT(int atFlag){
  statusBar_ -> clearMessage();
  switch (atFlag){
    case 1:{
        QString cmd = "00 00 03 01";
        QByteArray value = QByteArray::fromHex(cmd.toStdString().c_str());
        request(QModbusPdu::WriteSingleRegister, value);
        break;
      }
    case 2:{
        QString cmd = "00 00 03 02";
        QByteArray value = QByteArray::fromHex(cmd.toStdString().c_str());
        request(QModbusPdu::WriteSingleRegister, value);
        break;
     }
    default:{
      QString cmd = "00 00 03 00";
      QByteArray value = QByteArray::fromHex(cmd.toStdString().c_str());
      request(QModbusPdu::WriteSingleRegister, value);
      break;
    }
  }
  emit ATSendFinish(atFlag);
}

void Communication::sendRequestSV(double SV){
  statusBar_->clearMessage();
  changeSVValue(SV);
  emit SVSendFinish(SV);
}

void Communication::Stop(){
    QString cmd = "00 00 01 01";
    QByteArray value = QByteArray::fromHex(cmd.toStdString().c_str());
    request(QModbusPdu::WriteSingleRegister, value);
}

void Communication::askTemperature(){
  read(QModbusDataUnit::HoldingRegisters, static_cast<int>(E5CC_Address::Type::PV), 2);
  waitForMsec(timing::modbus);
  emit TemperatureUpdated(temperature_);
}

void Communication::askSV(){
  read(QModbusDataUnit::HoldingRegisters, static_cast<int>(E5CC_Address::Type::SV), 2);
  waitForMsec(timing::modbus);
  emit SVUpdated(SV_);
}

void Communication::askMV(){
  read(QModbusDataUnit::HoldingRegisters, static_cast<int>(E5CC_Address::Type::MV), 2);
  waitForMsec(timing::modbus);
  emit MVUpdated(MV_);
}

void Communication::askMVupper(){
  read(QModbusDataUnit::HoldingRegisters, static_cast<int>(E5CC_Address::Type::MVupper), 2);
  waitForMsec(timing::modbus);
  emit MVupperUpdated(MVupper_);
}

void Communication::askMVlower(){
  read(QModbusDataUnit::HoldingRegisters, static_cast<int>(E5CC_Address::Type::MVlower), 2);
  waitForMsec(timing::modbus);
  emit MVlowerUpdated(MVlower_);
}

void Communication::askPID(QString PID){
  if (PID == "P"){
      read(QModbusDataUnit::HoldingRegisters, static_cast<int>(E5CC_Address::Type::PID_P), 2);
      waitForMsec(timing::modbus);
      emit PID_PUpdated(pid_P_);
  } else if (PID == "I"){
      read(QModbusDataUnit::HoldingRegisters, static_cast<int>(E5CC_Address::Type::PID_I), 2);
      waitForMsec(timing::modbus);
      emit PID_IUpdated(pid_I_);
  } else if (PID == "D"){
      read(QModbusDataUnit::HoldingRegisters, static_cast<int>(E5CC_Address::Type::PID_D), 2);
      waitForMsec(timing::modbus);
      emit PID_DUpdated(pid_D_);
  } else {
    read(QModbusDataUnit::HoldingRegisters, static_cast<int>(E5CC_Address::Type::PID_P), 2);
     waitForMsec(timing::modbus);
     emit PID_PUpdated(pid_P_);
     read(QModbusDataUnit::HoldingRegisters, static_cast<int>(E5CC_Address::Type::PID_I), 2);
     waitForMsec(timing::modbus);
     emit PID_IUpdated(pid_I_);
     read(QModbusDataUnit::HoldingRegisters, static_cast<int>(E5CC_Address::Type::PID_D), 2);
     waitForMsec(timing::modbus);
     emit PID_DUpdated(pid_D_);
  }
}

void Communication::changeMVlowerValue(double MVlower){
  if(!modbusReady_) return;
  setMVlower(MVlower);
  int sv = (qint16) (MVlower / tempDecimal_ + 0.5);
  qDebug() << sv;
  QString valueStr = formatHex(sv, 8);
  QString addressStr = formatHex(static_cast<int>(E5CC_Address::Type::MVlower), 4);
  QString cmd = addressStr + " 00 02 04" + valueStr;
  QByteArray value = QByteArray::fromHex(cmd.toStdString().c_str());
  request(QModbusPdu::WriteMultipleRegisters, value);
}

void Communication::changeMVupperValue(double MVupper){
  if(!modbusReady_) return;;
  setMVupper(MVupper);
  int sv = (qint16) (MVupper / tempDecimal_ + 0.5);
  qDebug() << sv;
  QString valueStr = formatHex(sv, 8);
  QString addressStr = formatHex(static_cast<int>(E5CC_Address::Type::MVupper), 4);
  QString cmd = addressStr + " 00 02 04" + valueStr;
  QByteArray value = QByteArray::fromHex(cmd.toStdString().c_str());
  request(QModbusPdu::WriteMultipleRegisters, value);
}

void Communication::changeSVValue(double SV){
  int sv_input = (qint16) (SV /tempDecimal_ + 0.5);
  QString valueStr = formatHex(sv_input, 8);
  QString addressStr = formatE5CCAddress(E5CC_Address::Type::SV);
  QString cmd = addressStr + " 00 02 04" + valueStr;
  QByteArray value = QByteArray::fromHex(cmd.toStdString().c_str());
  request(QModbusPdu::WriteMultipleRegisters, value);
}

// setter methods
void Communication::setSerialPortName(QString portName){portName_ = portName;}
void Communication::setTemperature(double temperature){temperature_ = temperature;}
void Communication::setSV(double SV){SV_ = SV;}
void Communication::setMV(double MV){MV_ = MV;}
void Communication::setMVupper(double MVupper){MVupper_ = MVupper;}
void Communication::setMVlower(double MVlower){MVlower_ = MVlower;}
void Communication::setOmronID(int OmronID){omronID_ = OmronID;}
void Communication::setPID_P(double PID_P){return;} // Nothing. Generated to define Q_PROPERTY
void Communication::setPID_I(double PID_I){return;} // Nothing. Generated to define Q_PROPERTY
void Communication::setPID_D(double PID_D){return;} // Nothing. Generated to define Q_PROPERTY

// getter methods
QModbusRtuSerialMaster* Communication::getOmron() const {return omron_;}
QList<QSerialPortInfo> Communication::getSerialPortDevices() const {return infos_;}
QString Communication::getPortName() const {return portName_;}
double Communication::getTemperature() const {return temperature_;}
double Communication::getMV() const {return MV_;}
double Communication::getSV() const {return SV_;}
double Communication::getMVupper() const {return MVupper_;}
double Communication::getMVlower() const {return MVlower_;}
double Communication::getPID_P() const {return pid_P_;}
double Communication::getPID_I() const {return pid_I_;}
double Communication::getPID_D() const {return pid_D_;}
int Communication::getOmronID() const {return omronID_;}

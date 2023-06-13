/**
 *@file mainwindow.cpp
 *@brief mainwindow for connection to the E5CC
 */
#include "communication.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "safety.h"
#include "datasummary.h"

/**
 * @brief Constructor for the MainWindow class.
 *
 * @param parent The parent QWidget.
 */
MainWindow::MainWindow(QWidget *parent) :
  QMainWindow(parent),
  ui(new Ui::MainWindow),
  plot(new QCustomPlot),
  clock(new QTimer),
  waitTimer(new QTimer),
  LINEToken_("9tYexDQw9KHKyJOAI5gIONbXLZgzolIxungdwos5Dyy") //for RIKEN
  //LINEToken_("bhWUyinEDdIJkDf3jznzeHwrf1NrRzqSzuzTHEfyINd") // for Kyushu
{
  //set up GUI. Reference gui.cpp
  ui->setupUi(this);
  setupPlot();
  setupCombBox();
  setupDialog();
  initializeVariables();

  //Generate instance to use Communication class.
  com_ = new Communication(this, ui->statusBar);
  com_->setOmronID(ui->spinBox_DeviceAddress->value());
  connect(com_, &Communication::TemperatureUpdated, this, &MainWindow::updateTemperature);
  connect(com_, &Communication::SVUpdated, this, &MainWindow::updateSV);
  connect(com_, &Communication::MVUpdated, this, &MainWindow::updateMV);
  connect(com_, &Communication::MVupperUpdated, this, &MainWindow::updateMVupper);
  connect(com_, &Communication::MVlowerUpdated, this, &MainWindow::updateMVlower);
  connect(com_, &Communication::PID_PUpdated, this, &MainWindow::updatePID_P);
  connect(com_, &Communication::PID_IUpdated, this, &MainWindow::updatePID_I);
  connect(com_, &Communication::PID_DUpdated, this, &MainWindow::updatePID_D);
  connect(com_, &Communication::deviceConnect, this, &MainWindow::connectDevice);
  connect(com_, &Communication::failedConnect, this, &MainWindow::connectFailed);
  connect(com_, &Communication::statusUpdate, this, &MainWindow::updateStatus);
  connect(com_, &Communication::logMsg, this, &MainWindow::catchLogMsg);
  connect(com_, &Communication::logMsg, this, &MainWindow::catchLogMsgWithColor);
  connect(com_, &Communication::ATSendFinish, this, &MainWindow::finishSendAT);
  connect(com_, &Communication::SVSendFinish, this, &MainWindow::finishSendSV);
  connect(com_, &Communication::serialPortRemove, this, &MainWindow::sendLINE);
  addPortName(com_->getSerialPortDevices());

  //Generate instance to use DataSummary class.
  data_ = new DataSummary(com_);
  ui->lineEdit_DirPath->setText(data_->getFilePath());
  connect(data_, &DataSummary::logMsgWithColor, this, &MainWindow::catchLogMsgWithColor);

  //Generate instance to use Safety class.
  safety_ = new Safety(data_);
  safety_->setPermitedMaxTemp(ui->spinBox_TempUpper->value());
  connect(safety_, &Safety::dangerSignal, this, &MainWindow::catchDanger);
  connect(safety_, &Safety::checkNumberChanged, this, &MainWindow::updateCheckNumber);
  connect(safety_, &Safety::escapeTempCheckChange, this, &MainWindow::cathcEscapeTempCheckChange);
  connect(safety_, &Safety::startTempChangeCheck, this, &MainWindow::catchStartTempChangeCheck);
  connect(safety_, &Safety::logMsg, this, &MainWindow::catchLogMsg);
  connect(safety_, &Safety::logMsgWithColor, this, &MainWindow::catchLogMsgWithColor);

  //Generate instance to use Notify class.
  notify_ = new Notify(this);

  //Set default value to parameters
  setIntervalAskMV();
  setIntervalAskTemp();
  setNumbers();
  setSafeLimit();
  setIgnoreRange();
  setEnabledFalse();

  ui->textEdit_Log->setTextColor(QColor(34,139,34,255));
  LogMsg("The AT and RUN/STOP do not get from the device. Please be careful.");
  ui->textEdit_Log->setTextColor(QColor(0,0,0,255));

  plotTimer_ = new QTimer(this);
  plotTimer_->setInterval(intervalPlot_);
  connect(plotTimer_, SIGNAL(timeout()), this, SLOT(makePlot()));
  connect(ui->spinBox_TempRecordTime, SIGNAL(valueChanged(int)), data_, SLOT(setIntervalLog(int)));
  connect(ui->spinBox_TempRecordTime, SIGNAL(valueChanged(int)), this, SLOT(setIntervalPlot(int)));
  connect(ui->checkBox_dataSave, SIGNAL(toggled(bool)), data_, SLOT(setSave(bool)));
  connect(ui->doubleSpinBox_MVupper, SIGNAL(valueChanged(double)), data_, SLOT(setMVUpper(double)));
  connect(ui->doubleSpinBox_MVlower, SIGNAL(valueChanged(double)), data_, SLOT(setMVLower(double)));
  connect(ui->lineEdit_DirPath, SIGNAL(textChanged(QString)), data_, SLOT(setFilePath(QString)));
  connect(ui->pushButton_Control, &QPushButton::clicked, safety_, &Safety::setIsSTC);

  connect(com_->getTimerUpdate(), &QTimer::timeout, this, &MainWindow::updateStatusBoxes);
  connect(safety_->getTimerMVCheck(), &QTimer::timeout, this, MainWindow::updateStatusBoxes);
  connect(safety_->getTimerTempChangeCheck(), &QTimer::timeout, this, MainWindow::updateStatusBoxes);
  connect(data_->getLogTimer(), &QTimer::timeout, this, &MainWindow::updateStatusBoxes);

  timing_ = com_->timing::clockUpdate;
  connect(clock, SIGNAL(timeout()), this, SLOT(showTime()));
}

/**
 * @brief MainWindow::~MainWindow
 */
MainWindow::~MainWindow()
{
    auto omron = com_->getOmron();
    if (omron) omron->disconnectDevice();
    clock->stop();
    waitTimer->stop();
    delete waitTimer;
    delete clock;
    delete plot;
    delete omron;
    delete ui;
}

/**
 * @brief Appends a log message to the log text area.
 *
 * @param str The log message to be added.
 * @param newLine Flag indicating whether to start a new line (default: true).
 */
void MainWindow::LogMsg(QString str, bool newLine)
{
    if( newLine == false ){
        ui->textEdit_Log->moveCursor(QTextCursor::End);
        ui->textEdit_Log->insertPlainText(str);
    }else{
        msgCount ++;
        QString dateStr = QDateTime::currentDateTime().toString("HH:mm:ss ");
        QString countStr;
        countStr.asprintf("[%05d]: ", msgCount);
        str.insert(0, countStr).insert(0, dateStr);
        ui->textEdit_Log->append(str);
    }
    int max = ui->textEdit_Log->verticalScrollBar()->maximum();
    ui->textEdit_Log->verticalScrollBar()->setValue(max);
}

/**
 * @brief Handles the key press event.
 *
 * This function is called when a key is pressed while the main window has focus.
 * It checks if the pressed key is the Shift key (Qt::Key_Shift) and performs
 * certain actions accordingly. In this case, when the Shift key is pressed,
 * the range drag and range zoom behaviors are enabled for the horizontal axis of the plot.
 *
 * @param key A pointer to the QKeyEvent object containing information about the key event.
 */
void MainWindow::keyPressEvent(QKeyEvent *key)
{
    if(key->key() == Qt::Key_Shift ){
        plot->axisRect()->setRangeDrag(Qt::Horizontal);
        plot->axisRect()->setRangeZoom(Qt::Horizontal);
    }
}

/**
 * @brief Handles the key release event.
 *
 * This function is called when a key is released while the main window has focus.
 * It checks if the released key is the Shift key (Qt::Key_Shift) and performs
 * certain actions accordingly. In this case, when the Shift key is released,
 * the range drag and range zoom behaviors are enabled for the vertical axis of the plot.
 *
 * @param key A pointer to the QKeyEvent object containing information about the key event.
 */
void MainWindow::keyReleaseEvent(QKeyEvent *key)
{
    if(key->key() == Qt::Key_Shift ){
        plot->axisRect()->setRangeDrag(Qt::Vertical);
        plot->axisRect()->setRangeZoom(Qt::Vertical);
    }
}


/**
 * @brief Pauses the program execution for the specified number of milliseconds.
 *
 * This function waits for the given number of milliseconds before allowing the program
 * to continue execution. It uses an event loop and a QTimer to achieve the desired delay.
 *
 * @param msec The number of milliseconds to wait.
 */
void MainWindow::waitForMSec(int msec)
{
    //wait for waitTime
    QEventLoop eventLoop;
    QTimer::singleShot(msec, &eventLoop, SLOT(quit()));
    eventLoop.exec();
}


/**
 * @brief Updates and displays the elapsed time.
 *
 * This function calculates the elapsed time since a specific starting point and updates
 * the display accordingly. It calculates the hours elapsed and checks if a new day has
 * started. It also constructs a `QTime` object to represent the elapsed time in hours,
 * minutes, seconds, and milliseconds.
 */
void MainWindow::showTime()
{
    double hour = totalElapse.elapsed()/1000./60./60.;
    if( checkDay && hour <= 22){
        dayCounter ++;
        checkDay = false;
    }
    if(hour > 22){
        checkDay = true;
    }
    QTime t(0,0,0,0);
    t = t.addMSecs(totalElapse.elapsed());
}

/**
 * @brief Slot function triggered when the "Ask Status" button is clicked.
 *
 * This function sends requests to the communication object `com_` to retrieve the temperature,
 * SV (Set Value), and MV (Manipulated Value) from the device. It calls the appropriate functions in
 * the communication object to send the requests.
 */
void MainWindow::on_pushButton_AskStatus_clicked(){
  com_->askTemperature();
  com_->askSV();
  com_->askMV();
}

/**
 * @brief Retrieves the settings from the device.
 *
 * This function sends requests to the communication object `com_` to retrieve the temperature,
 * SV (Set Value), MV (Manipulated Value), MV upper limit, MV lower limit, and PID values from the device.
 * It sets the `spinBoxEnable` flag to `false` before requesting the MV upper and lower limits to prevent
 * the associated spin boxes from being updated. After that, it sets the `spinBoxEnable` flag back to `true`
 * and requests the PID values from the device.
 */
void MainWindow::getSetting(){
  com_->askTemperature();
  com_->askSV();
  com_->askMV();
  spinBoxEnable = false;
  com_->askMVupper();
  com_->askMVlower();
  spinBoxEnable = true;
  com_->askPID("PID");
}

/**
 * @brief Slot function called when the "Set SV" button is clicked.
 *
 * This function sends a request to the communication object `com_` to set the SV (Set Value) to the value
 * specified in the `lineEdit_SV` widget. It also logs a message indicating the new set temperature.
 */
void MainWindow::on_pushButton_SetSV_clicked(){
  com_->executeSendRequestSV(ui->lineEdit_SV->text().toDouble());
  LogMsg("Set Temperature is changed to " + ui->lineEdit_SV->text() + " C.");
}


/**
 * @brief Slot function called when the "Control" button is clicked.
 *
 * This function toggles the temperature control on/off by flipping the value of the `tempControlOnOff` flag.
 * It updates the UI elements accordingly and performs temperature control based on the selected mode.
 * The control mode is determined by the value of the `comboBox_Mode` widget.
 * The function initializes the necessary variables from the GUI and calls the appropriate control mode function.
 */
void MainWindow::on_pushButton_Control_clicked(){
  tempControlOnOff = !tempControlOnOff;
  panalOnOff(!tempControlOnOff);
  ui->actionOpen_File->setEnabled(!tempControlOnOff);
  if(tempControlOnOff) {
      LogMsg("================ Temperature control =====");
      ui->pushButton_Control->setStyleSheet("background-color: rgb(50,137,48)");
      ui->lineEdit_msg->setText("Slow Temperature controle mode");
      ui->checkBoxStatusSTC->setChecked(true);
      ui->checkBoxStatusTempDrop->setChecked(false);
      ui->checkBoxStatusTempDrop->setEnabled(false);
  }else{
      LogMsg("================ Temperature control Off =====");
      ui->pushButton_Control->setStyleSheet("");
      on_comboBox_Mode_currentIndexChanged(ui->comboBox_Mode->currentIndex());
      qDebug()  << "temp control. = " << tempControlOnOff;
      clock->stop();
      totalElapse.start();
      ui->checkBoxStatusSTC->setChecked(false);
      ui->checkBoxStatusTempDrop->setEnabled(true);
      ui->lineEdit_msg->clear();
      return;
  }

  //* initialization variables from GUI.
  double targetValue = ui->lineEdit_SV->text().toDouble();
  double tempTorr = ui->doubleSpinBox_TempTorr->value();
  double tempStepSize = ui->doubleSpinBox_TempStepSize->value();
  int mode = ui->comboBox_Mode->currentData().toInt();
  switch (mode){
    case 1:
      controlStableMode(targetValue, tempTorr, tempStepSize);
      break;
    case 2:
      controlFixedTimeMode(targetValue, tempTorr, tempStepSize);
      break;
    case 3:
      controlFixedRateMode(targetValue, tempTorr, tempStepSize);
      break;
    case 4:
      controlNormalAndFixedRateMode(targetValue, tempTorr, tempStepSize);
      break;
    default:
      qDebug() << "Invalid control mode.";
      break;
  }
}

/**
 * @brief Performs temperature control in the Stable Mode.
 *
 * This function adjusts the set value (SV) based on the current temperature to gradually approach the target temperature.
 * It continuously checks the temperature and updates the SV until the temperature is within the specified tolerance (tempTorr).
 * The temperature adjustment is performed in steps (tempStepSize) to ensure a stable approach to the target temperature.
 * The function uses a while loop to monitor the temperature and adjust the SV accordingly.
 * It updates the UI with the current SV and sends the SV to the device using the Communication class.
 * It waits for a specified time (waitTime) before checking the temperature again.
 * The temperature control is stopped if the tempControlOnOff flag is false.
 * After the temperature reaches the target value within the tolerance, it updates the UI accordingly.
 *
 * @param targetValue The target temperature value to reach.
 * @param tempTorr The temperature tolerance allowed around the target temperature.
 * @param tempStepSize The step size used for adjusting the SV.
 */
void MainWindow::controlStableMode(double targetValue, double tempTorr, double tempStepSize){
  if (!tempControlOnOff) return;
  LogMsg("Stable Mode start");
  double temperature = data_->getTemperature();
  double nextSV = temperature;
  const int direction = (temperature > targetValue) ? -1 : 1;
  while (qAbs(temperature - targetValue) > tempTorr) {
    LogMsg("Current temperature is " + QString::number(temperature));
    if (direction * (targetValue - temperature) >= tempStepSize) {
      nextSV = temperature + direction * tempStepSize;
    } else {
      nextSV = targetValue;
    }
    ui->lineEdit_CurrentSV->setText(QString::number(nextSV) + " C");
    com_->executeSendRequestSV(nextSV);
    int waitTime = ui->spinBox_TempStableTime->value() * 60 * 1000; //msec to min
    QEventLoop loop;
    QTimer::singleShot(waitTime, &loop, &QEventLoop::quit);
    loop.exec();
    temperature = data_->getTemperature();
    if (!tempControlOnOff) break;
  }
  ui->checkBoxStatusSTC->setChecked(false);
}

/**
 * @brief Performs temperature control in the Fixed Time Mode.
 *
 * This function adjusts the set value (SV) based on the current temperature to gradually approach the target temperature
 * within a fixed time period. It continuously checks the temperature and updates the SV until the temperature is within
 * the specified tolerance (tempTorr) or the fixed time period is over.
 * The temperature adjustment is performed in steps (tempStepSize) to ensure a stable approach to the target temperature.
 * The function uses a while loop to monitor the temperature and adjust the SV accordingly.
 * It updates the UI with the current SV and sends the SV to the device using the Communication class.
 * It waits for a specified time (waitTime) before checking the temperature again.
 * The temperature control is stopped if the tempControlOnOff flag is false.
 * After the temperature reaches the target value within the tolerance or the fixed time period is over, it updates the UI accordingly.
 *
 * @param targetValue The target temperature value to reach.
 * @param tempTorr The temperature tolerance allowed around the target temperature.
 * @param tempStepSize The step size used for adjusting the SV.
 */
void MainWindow::controlFixedTimeMode(double targetValue, double tempTorr, double tempStepSize){
  if (!tempControlOnOff) return;
  LogMsg("Fixed Time mode start");
  double temperature = data_->getTemperature();
  double nextSV = temperature;
  const int direction = (temperature > targetValue) ? -1 : 1;
  while (qAbs(temperature - targetValue) > tempTorr) {
    if (qAbs(targetValue - temperature) >= tempStepSize) {
      nextSV += direction * tempStepSize; // ver.2023
      /**
       * the following nextSV is in case of original code.
      nextSV += temperature + direction * tempStepSize;
      **/
    } else {
      nextSV = targetValue;
    }
    ui->lineEdit_CurrentSV->setText(QString::number(nextSV) + " C");
    com_->executeSendRequestSV(nextSV);
    int waitTime = ui->spinBox_TempStableTime->value()*60 * 1000;
    QEventLoop loop;
    QTimer::singleShot(waitTime, &loop, &QEventLoop::quit);
    loop.exec();
    temperature = data_->getTemperature();
    if (!tempControlOnOff) break;
  }
  ui->checkBoxStatusSTC->setChecked(false);
}

/**
 * @brief Performs temperature control in the Fixed Rate Mode.
 *
 * This function adjusts the set value (SV) based on the current temperature to gradually approach the target temperature
 * at a fixed rate. It continuously checks the temperature and updates the SV until the temperature is within the specified
 * tolerance (tempTorr) or the maximum rate of change (tempStepSize) is reached.
 * The temperature adjustment is performed in steps equal to the time interval specified in the TempStableTime spin box.
 * The function uses a while loop to monitor the temperature and adjust the SV accordingly.
 * It updates the UI with the current SV and sends the SV to the device using the Communication class.
 * It waits for a fixed time interval (waitTime) before checking the temperature again.
 * The temperature control is stopped if the tempControlOnOff flag is false.
 * After the temperature reaches the target value within the tolerance or the maximum rate of change is reached, it updates the UI accordingly.
 *
 * @param targetValue The target temperature value to reach.
 * @param tempTorr The temperature tolerance allowed around the target temperature.
 * @param tempStepSize The maximum rate of change for adjusting the SV.
 */
void MainWindow::controlFixedRateMode(double targetValue, double tempTorr, double tempStepSize){
  if (!tempControlOnOff) return;
  LogMsg("Fixed Rate mode start");
  double temperature = data_->getTemperature();
  double nextSV = temperature - tempStepSize;
  const int direction = (temperature > targetValue) ? -1 : 1;
  tempStepSize = ui->spinBox_TempStableTime->value(); //
  int waitTime = 60 * 1000;//msec
  while (qAbs(temperature - targetValue) > tempTorr) {
    if (qAbs(targetValue - temperature) >= tempStepSize) {
      nextSV += direction * tempStepSize;
    } else {
      nextSV = targetValue;
    }
    ui->lineEdit_CurrentSV->setText(QString::number(nextSV) + " C");
    com_->executeSendRequestSV(nextSV);
    QEventLoop loop;
    QTimer::singleShot(waitTime, &loop, &QEventLoop::quit);
    loop.exec();
    temperature = data_->getTemperature();
    if (!tempControlOnOff) break;
  }
  ui->checkBoxStatusSTC->setChecked(false);
}



void MainWindow::controlNormalAndFixedRateMode(double targetValue, double tempTorr, double tempStepSize){
  double temperature = data_->getTemperature();
  const double targetValue_2 = ui->lineEdit_SV2->text().toDouble();
  int targetValue_2_waitTime = ui->doubleSpinBox_SV2WaitTime->value() * 60.*1000.;
  ui->lineEdit_CurrentSV->setText(QString::number(targetValue_2) + " C");
  com_->executeSendRequestSV(targetValue_2);
  while(temperature > targetValue_2){
    QEventLoop loop;
    QTimer::singleShot(targetValue_2_waitTime, &loop, &QEventLoop::quit);
    loop.exec();
    temperature = data_->getTemperature();
  }
  controlFixedRateMode(targetValue, tempTorr, tempStepSize);
}

/**
 * @brief Performs temperature control in the Normal and Fixed Rate Mode.
 *
 * This function combines the functionality of the Normal Mode and Fixed Rate Mode for temperature control.
 * It first adjusts the set value (SV) to a secondary target value (targetValue_2) and waits until the temperature reaches
 * or falls below that value. Once the temperature reaches the secondary target value, it calls the controlFixedRateMode
 * function to continue temperature control using the fixed rate approach.
 *
 * The function starts by getting the current temperature and the secondary target value from the UI.
 * It also retrieves the wait time for the secondary target value from the UI.
 * It sets the current SV to the secondary target value and sends it to the device using the Communication class.
 *
 * The function enters a while loop that continues until the temperature is not greater than the secondary target value.
 * Within the loop, it waits for the specified wait time and updates the temperature.
 *
 * Once the temperature reaches or falls below the secondary target value, it calls the controlFixedRateMode function
 * to continue temperature control using the fixed rate approach with the primary target value, temperature tolerance,
 * and maximum rate of change specified.
 *
 * @param targetValue The primary target temperature value to reach after reaching the secondary target value.
 * @param tempTorr The temperature tolerance allowed around the primary target temperature.
 * @param tempStepSize The maximum rate of change for adjusting the SV during the fixed rate mode.
 */
double MainWindow::calcRate(double temp, double aftertemp, int msec){
  double diff = qAbs(aftertemp - temp);
  double rate = 0.0;
  LogMsg("Cuurent temperature is " + QString::number(temp));
  LogMsg("After temperature is " + QString::number(aftertemp));
  LogMsg("Difference is " + QString::number(diff));
  if (diff < 1e-6) {
      LogMsg("temperature difference is too small to cacurate for the rate. (rate is inf).");
      LogMsg("So, current rate is set to be 0.0");
      rate = 0.0;
  } else{
      rate = diff/msec/60.0/1000.0; //C /min
      LogMsg("Rate is " + QString::number(rate));
  }
  return rate;
}

/**
 * @brief Slot triggered when the index of the comboBox_AT changes.
 *
 * This slot is called when the index of the comboBox_AT changes.
 * It checks if the comboxEnable flag is set, and if not, it returns immediately.
 * Otherwise, it calls the executeSendRequestAT function of the com_ object to send the selected index value to the device.
 *
 * @param index The new index selected in the comboBox_AT.
 */
void MainWindow::on_comboBox_AT_currentIndexChanged(int index)
{
    if(!comboxEnable) return;
    com_->executeSendRequestAT(index);
}

/**
 * @brief Slot triggered when the "Connect" button is clicked.
 *
 * This slot is called when the "Connect" button is clicked.
 */
void MainWindow::on_pushButton_Connect_clicked(){
  LogMsg("Start connecing...");
  LogMsg("Please do nothing and wait for a moment.");
  com_->setSerialPortName(ui->comboBox_SeriesNumber->currentData().toString());
  com_->executeConnection();
  LogMsg("Finish connecing.");
}

/**
 * @brief Slot triggered when the value of the MV lower limit spin box is changed.
 *
 * This slot is called when the user changes the value of the MV lower limit spin box (doubleSpinBox_MVlower).
 * It updates the lower limit value of the output based on the new value entered by the user and logs a message
 * to indicate the change.
 *
 * @param arg1 The new value of the MV lower limit entered by the user.
 */
void MainWindow::on_doubleSpinBox_MVlower_valueChanged(double arg1){
    if(!spinBoxEnable) return;
    com_->changeMVlowerValue(arg1);
    LogMsg("Output lower limit is set to be " + QString::number(arg1));
}

/**
 * @brief Slot triggered when the value of the MV upper limit spin box is changed.
 *
 * This slot is called when the user changes the value of the MV upper limit spin box (doubleSpinBox_MVupper).
 * It updates the upper limit value of the output, sets the upper limit value for safety checks, and updates the plot.
 * Additionally, it logs a message to indicate the change.
 *
 * @param arg1 The new value of the MV upper limit entered by the user.
 */
void MainWindow::on_doubleSpinBox_MVupper_valueChanged(double arg1){
    if(!spinBoxEnable) return;
    com_->changeMVupperValue(arg1);
    safety_->setMVUpper(arg1);
    LogMsg("Output upper limit is set to be " + QString::number(arg1));
    plot->yAxis2->setRangeLower(com_->getMVupper() + 2);
    plot->replot();
}

/**
 * @brief Slot triggered when the Get PID button is clicked.
 *
 * This slot is called when the user clicks the Get PID button (pushButton_GetPID).
 * It sends a request to the communication object (com_) to ask for the PID value.
 */
void MainWindow::on_pushButton_GetPID_clicked() {com_->askPID("PID");}

/**
 * @brief Slot triggered when the current text of the memory address combo box is changed.
 *
 * This slot is called when the user selects a different memory address from the combo box (comboBox_MemAddress).
 * It reads the selected memory address using the communication object (com_) and logs a message indicating the read operation.
 *
 * @param arg1 The current text of the memory address combo box.
 */
void MainWindow::on_comboBox_MemAddress_currentTextChanged(const QString &arg1)
{
    if(!comboxEnable) return;
    quint16 address = ui->comboBox_MemAddress->currentData().toUInt();
    LogMsg("--------- read " + arg1);
    com_->read(QModbusDataUnit::HoldingRegisters, address, 2);
}

/**
 * @brief Slot triggered when the Open File action is triggered.
 *
 * This slot is called when the user triggers the Open File action from the menu.
 * It opens a file dialog to allow the user to select a file to open. If the file is successfully opened,
 * it reads the contents of the file and populates the plot data with the read values.
 */
void MainWindow::on_actionOpen_File_triggered()
{
    QString filePath = QFileDialog::getOpenFileName(this, "Open File", filePath_ );
    QFile infile(filePath);
    if(infile.open(QIODevice::ReadOnly | QIODevice::Text)) LogMsg("Open File : %s" + filePath);
    else{
        LogMsg("Open file failed ");
        return;
    }
    QTextStream stream(&infile);
    QString line;
    pvData.clear();
    svData.clear();
    mvData.clear();

    bool haveSVMVData = false;
    while(stream.readLineInto(&line)){
        if( line.left(3) == "###") {
            LogMsg(line);
            continue;
        }
        QStringList list = line.split(",");
        if(list.size() < 3) continue;
        QString time = list[1];
        QString pv = list[2];
        QCPGraphData plotdata;
        plotdata.key = time.toInt();
        plotdata.value = pv.toDouble();
        pvData.push_back(plotdata);

        if( list.size() <= 5){
            haveSVMVData = true;
            QString sv = list[3];
            QString mv = list[4];
            plotdata.value = sv.toDouble();
            svData.push_back(plotdata);
            plotdata.value = mv.toDouble();
            mvData.push_back(plotdata);
        }
    }

    infile.close();

    plot->graph(0)->data()->clear();
    plot->graph(1)->data()->clear();
    plot->graph(2)->data()->clear();

    plot->graph(1)->data()->add(pvData);
    if( haveSVMVData ){
        plot->graph(2)->data()->add(svData);
        plot->graph(0)->data()->add(mvData);
    }

    plot->yAxis->rescale();
    plot->yAxis2->rescale();
    plot->xAxis->rescale();

    double ymin = plot->yAxis->range().lower - 2;
    double ymax = plot->yAxis->range().upper + 2;
    plot->yAxis->setRangeLower(ymin);
    plot->yAxis->setRangeUpper(ymax);

    ymax = plot->yAxis2->range().upper + 2;
    plot->yAxis2->setRangeLower(0);
    plot->yAxis2->setRangeUpper(ymax);

    plot->replot();

}

/**
 * @brief Adds data points to the plot and updates the plot's range based on the new data.
 *
 * @param date The date and time of the data point.
 * @param PV The process value.
 * @param SV The set value.
 * @param MV The manipulated value.
 */
void MainWindow::fillDataAndPlot(const QDateTime date, const double PV, const double SV, const double MV)
{
    QCPGraphData plotdata;
    plotdata.key = date.toSecsSinceEpoch();
    plotdata.value = PV;
    pvData.push_back(plotdata);
    plotdata.value = SV;
    svData.push_back(plotdata);
    plotdata.value = MV;
    mvData.push_back(plotdata);

    plot->graph(0)->data()->clear();
    plot->graph(0)->data()->add(mvData);
    plot->graph(1)->data()->clear();
    plot->graph(1)->data()->add(pvData);
    plot->graph(2)->data()->clear();
    plot->graph(2)->data()->add(svData);

    int intDate = date.toSecsSinceEpoch();
    int setrange = plotDialog_->displayRange_ * 60;
    plot->xAxis->setRange(intDate - setrange, intDate);
    plot->yAxis->rescale();

    double ymin = plot->yAxis->range().lower - 2;
    double ymax = plot->yAxis->range().upper + 2;

    plot->yAxis->setRangeLower(ymin);
    plot->yAxis->setRangeUpper(ymax);
    plot->replot();
}


/**
 * @brief MainWindow::fillDifference
 * @param mute
 * @return double temperature difference
 */
double MainWindow::fillDifference(bool mute){
  int size = valltemp_.size();
  if(size < 2) return .0;
  double diff = valltemp_.at(size-1) - valltemp_.at(size-2);
  vdifftemp_.push_back(diff);
  ui->lineEdit_CurrentTempDiff->setText(QString::number(diff) + " C");
  if (!mute) LogMsg("Difference is " + QString::number(diff));
  return diff;
}

/**
 * @brief Shows the help dialog if it is hidden.
 */
void MainWindow::on_actionHelp_Page_triggered(){
    if( helpDialog_->isHidden() ) helpDialog_->show();
}

/**
 * @brief Shows the plot dialog if it is hidden.
 */
void MainWindow::on_action_Setting_plot_triggered(){
  if( plotDialog_->isHidden() ) plotDialog_->show();
}

/**
 * @brief Shows the configure dialog for setting parameters for TempCheck if the E5CC is not running.
 */
void MainWindow::on_action_Setting_parameters_for_TempCheck_triggered(){
  if(ui->pushButton_RunStop->isChecked()){
    QMessageBox msgbox;
    msgbox.setIcon(QMessageBox::Warning);
    msgbox.setText(tr("Cannot be set while E5CC is running.\nPlease set after Stop."));
    msgbox.setWindowTitle(tr("Warning"));
    msgbox.setStandardButtons(QMessageBox::Ok);
    msgbox.exec();
    return;
  }
  if(configureDialog_->isHidden()) configureDialog_->show();
}

/**
 * @brief Shows the join dialog for joining LINE if it is hidden.
 */
void MainWindow::on_action_JoinLINE_RIKEN_triggered(){
    if( joinDialog_->isHidden() ) joinDialog_->show();
}

/**
 * @brief Shows the join dialog for joining LINE if it is hidden.
 */
void MainWindow::on_action_JoinLINE_Kyushu_triggered(){
    if( joinDialogK_->isHidden() ) joinDialogK_->show();
}

/*
void MainWindow::on_action_Setting_Temperature_Drop_triggered(){
  if (tempDropDialog_->isHidden()) tempDropDialog_->show();
}
*/

/**
 * @brief Sets the interval for asking the MV (Manipulated Variable).
 *
 * This function enables the line edit widget for interval input, sets its text
 * to the configured interval value, stops the safety monitoring, updates the
 * interval for MV check in the safety module, and disables the line edit widget
 * again.
 */
void MainWindow::setIntervalAskMV(){
  ui->lineEdit_IntervalAskMV->setEnabled(true);
  ui->lineEdit_IntervalAskMV->setText(QString::number(configureDialog_->intervalAskMV_));
  safety_->stop();
  safety_->setIntervalMVCheck(configureDialog_->intervalAskMV_);
  ui->lineEdit_IntervalAskMV->setEnabled(false);
}

/**
 * @brief Sets the interval for asking the temperature change.
 *
 * This function enables the line edit widget for interval input, sets its text
 * to the configured interval value, stops the safety monitoring, updates the
 * interval for temperature change in the safety module, and disables the line
 * edit widget again.
 */
void MainWindow::setIntervalAskTemp(){
  ui->lineEdit_IntervalAskTemp->setEnabled(true);
  ui->lineEdit_IntervalAskTemp->setText(QString::number(configureDialog_->intervalAskTemp_));
  safety_->stop();
  safety_->setIntervalTempChange(configureDialog_->intervalAskTemp_);
  ui->lineEdit_IntervalAskTemp->setEnabled(false);
}

/**
 * @brief Sets the number of checks for safety monitoring.
 *
 * This function enables the line edit widget for number input, sets its text
 * to the configured number value, stops the safety monitoring, updates the
 * number of checks in the safety module, and disables the line edit widget again.
 */
void MainWindow::setNumbers(){
  ui->lineEdit_Numbers->setEnabled(true);
  ui->lineEdit_Numbers->setText(QString::number(configureDialog_->numbers_));
  safety_->stop();
  safety_->setNumberOfCheck(configureDialog_->numbers_);
  ui->lineEdit_Numbers->setEnabled(false);
}

/**
 * @brief Sets the safe limit for temperature change.
 *
 * This function enables the line edit widget for safe limit input, sets its text
 * to the configured safe limit value, stops the safety monitoring, updates the
 * safe limit for temperature change in the safety module, and disables the line
 * edit widget again.
 */
void MainWindow::setSafeLimit(){
  ui->lineEdit_SafeLimit->setEnabled(true);
  ui->lineEdit_SafeLimit->setText(QString::number(configureDialog_->safeLimit_));
  safety_->stop();
  safety_->setTempChangeThreshold(configureDialog_->safeLimit_);
  ui->lineEdit_SafeLimit->setEnabled(false);
}

/**
 * @brief Sets the ignore range for temperature monitoring.
 *
 * This function enables the line edit widgets for lower and upper ignore range input,
 * stops the safety monitoring, updates the ignore range values in the safety module
 * if the ignore range is enabled, and disables the line edit widgets again.
 * If the ignore range is not enabled, the line edit widgets are set to display "None".
 */
void MainWindow::setIgnoreRange(){
  ui->lineEdit_IgnoreLower->setEnabled(true);
  ui->lineEdit_IgnoreUpper->setEnabled(true);
  safety_->stop();
  if(configureDialog_->ignoreEnable_){
    ui->lineEdit_IgnoreLower->setText(QString::number(configureDialog_->ignoreLower_));
    ui->lineEdit_IgnoreUpper->setText(QString::number(configureDialog_->ignoreUpper_));
    safety_->setIgnoreLower(configureDialog_->ignoreLower_);
    safety_->setIgnoreUpper(configureDialog_->ignoreUpper_);
  } else{
    ui->lineEdit_IgnoreLower->setText("None.");
    ui->lineEdit_IgnoreUpper->setText("None.");
  }
  ui->lineEdit_IgnoreLower->setEnabled(false);
  ui->lineEdit_IgnoreUpper->setEnabled(false);
}

/**
 * @brief Sets the enable state for ignore range.
 *
 * This function enables the check box widget for ignore range, sets its checked state
 * based on the configured value, stops the safety monitoring, updates the enable state
 * for temperature change range in the safety module, and disables the check box widget again.
 */
void MainWindow::setIgnoreEnable(){
  ui->checkBox_Ignore->setEnabled(true);
  ui->checkBox_Ignore->setChecked(configureDialog_->ignoreEnable_);
  safety_->stop();
  safety_->setEnableTempChangeRange(configureDialog_->ignoreEnable_);
  ui->checkBox_Ignore->setEnabled(false);
}

/**
 * @brief Sets the parameters for TempCheck change.
 *
 * This function sets the interval for asking MV, the interval for asking temperature,
 * the number of checks, the safe limit for temperature change, the ignore range for
 * temperature monitoring, and the enable state for ignore range based on the configured values.
 * If the `mute` parameter is `false`, it logs a message indicating that the parameters have been set.
 *
 * @param mute A boolean indicating whether to mute the log message (default: `false`).
 */
void MainWindow::setParametersTempCheckChange(bool mute){
  if (!configureDialog_->warnigcheck_) return;
  setIntervalAskMV();
  setIntervalAskTemp();
  setNumbers();
  setSafeLimit();
  setIgnoreRange();
  setIgnoreEnable();
  if (!mute){
    LogMsg("set to be parameters for TempCheck.");
    LogMsg(configureDialog_->msg_);
  }
}

/**
 * @brief Handles the toggle action of the "Run/Stop" button.
 *
 * This function is triggered when the "Run/Stop" button is toggled. It checks if the PID is connected and
 * the button is checked. If both conditions are met, it sets the button text to "Stop" and calls the Run() function.
 * If the PID is connected but the button is not checked, it sets the button text to "Run" and calls the Stop() function.
 * If the PID is not connected, it logs a message and unchecks the "Run" checkbox.
 *
 * @param checked A boolean indicating whether the button is checked or not.
 */
void MainWindow::on_pushButton_RunStop_toggled(bool checked)
{
  bool connectPID = ui->pushButton_Connect->isChecked();
  if(checked && connectPID){
    ui->pushButton_RunStop->setText("Stop");
    Run();
  }else if (connectPID) {
    ui->pushButton_RunStop->setText("Run");
    Stop();
  } else {
    LogMsg("Not connected. Please check COM PORT etc.");
    ui->checkBoxStatusRun->setChecked(false);
  }
}

/**
 * @brief Handles the toggle action of the "Log" button.
 *
 * This function is triggered when the "Log" button is toggled. It checks if the PID is connected and
 * the button is checked. If both conditions are met, it sets the button text to "Logging Stop" and
 * calls the logingStart() function in the data module. If the PID is connected but the button is not checked,
 * it sets the button text to "Logging Start" and calls the logingStop() function in the data module.
 * If the PID is not connected, it logs a message and calls the logingStop() function in the data module.
 *
 * @param checked A boolean indicating whether the button is checked or not.
 */
void MainWindow::on_pushButton_Log_toggled(bool checked){
  bool connectPID = ui->pushButton_Connect->isChecked();
  if(checked && connectPID){
    ui->pushButton_Log->setText("Logging Stop");
    data_->logingStart();
  }else if (connectPID) {
    ui->pushButton_Log->setText("Logging Start");
    data_->logingStop();
  } else {
    LogMsg("Not connected. Please check COM PORT etc.");
    data_->logingStop();
  }
}

/**
 * @brief Handles the "Run" operation.
 *
 * This function is called when the "Run" button is pressed. It performs the necessary operations to start the system.
 * It clears the status bar message, logs a "Set Run" message, executes the run command in the communication module,
 * sets the color, stylesheets, and checked state of UI elements, starts the plot timer, generates a save file,
 * sets the interval for logging, starts logging, sets the interval for checking MV, sets the interval for temperature change,
 * starts the safety module, and sets the isQuit_ flag to false.
 */
void MainWindow::Run(){
  statusBar()->clearMessage();
  LogMsg("Set Run.");
  com_->executeRun();
  setColor(1);
  ui->lineEdit_msg->setStyleSheet("");
  ui->pushButton_Log->setChecked(true);
  ui->checkBoxStatusRun->setChecked(true);
  sendLINE("Running start.");
  plotTimer_->start();
  data_->generateSaveFile();
  data_->SetIntervalLog(ui->spinBox_TempRecordTime->value());
  data_->logingStart();
  safety_->setIntervalMVCheck(ui->lineEdit_IntervalAskMV->text().toInt());
  safety_->setIntervalTempChange(ui->lineEdit_IntervalAskTemp->text().toInt());
  safety_->start();
  isQuit_ = false;
}

/**
 * @brief Handles the "Stop" operation.
 *
 * This function is called when the "Stop" button is pressed. It performs the necessary operations to stop the system.
 * It clears the status bar message, executes the stop command in the communication module, logs a "Set Stop" message,
 * sets the color, and updates the checked state of various UI elements. It stops the safety module, stops logging,
 * stops the plot timer, sets the isQuit_ flag to false, and sends a message via LINE.
 */
void MainWindow::Stop(){
  statusBar()->clearMessage();
  com_->executeStop();
  LogMsg("Set Stop.");
  setColor(0);
  ui->checkBoxStatusRun->setChecked(false);
  ui->checkBoxStatusSTC->setChecked(false);
  ui->checkBoxStatusPeriodic->setChecked(false);
  ui->checkBoxUpdate->setChecked(false);
  ui->checkBoxStautsTempCheck->setChecked(false);
  ui->action_Setting_parameters_for_TempCheck->setEnabled(true);
  ui->lineEdit_msg->clear();
  ui->lineEdit_msg->setStyleSheet("");
  safety_->stop();
  data_->logingStop();
  plotTimer_->stop();
  isQuit_ = false;
  sendLINE("Running stop");
}

/**
 * @brief Handles the "Quit" operation.
 *
 * This function is called when the "Quit" operation is triggered. It performs the necessary operations to stop the system
 * and handle an emergency stop situation. It executes the stop command in the communication module, logs an "Emergency Stop"
 * message, clears the temperature vector, updates the checked state of various UI elements, stops the safety module,
 * sends a message via LINE, sets the color, sets the isQuit_ flag to true, starts logging, and starts the plot timer.
 */
void MainWindow::Quit(){
  com_->executeStop();
  ui->textEdit_Log->setTextColor(QColor(255,0,0,255));
  LogMsg("Emergency Stop. Check the experimental condition.");
  vtemp_.clear();
  ui->textEdit_Log->setTextColor(QColor(0,0,0,255));
  ui->checkBoxStatusRun->setChecked(false);
  ui->checkBoxStatusSTC->setChecked(false);
  ui->pushButton_RunStop->setChecked(false);
  safety_->stop();
  sendLINE("Emergency Stop!");
  setColor(3);
  isQuit_ = true;
  data_->logingStart();
  plotTimer_->start();
}

/**
 * @brief Sets the text for the "Temp Drop Enable" checkbox.
 *
 * This function is called to update the text displayed in the "Temp Drop Enable" checkbox based on the specified enable state.
 *
 * @param enable A boolean indicating whether the "Temp Drop" feature is enabled or not.
 */
void MainWindow::setTextTempDrop(bool enable){
  if(enable) ui->checkBox_TempDropEnable->setText(tr("Yes"));
  else ui->checkBox_TempDropEnable->setText(tr("No"));
}

/**
 * @brief Generates a plot.
 *
 * This function is called to generate a plot based on the current temperature and set temperature. It retrieves the current
 * date and time, retrieves the temperature values, and calls the "fillDataAndPlot" function to update the plot data.
 */
void MainWindow::makePlot(){
  const double setTemperature = ui->lineEdit_SV->text().toDouble();
  QDateTime date = QDateTime::currentDateTime();
  valltemp_.push_back(data_->getTemperature());
  fillDataAndPlot(date, data_->getTemperature(), setTemperature, data_->getMV());
}

/**
 * @brief Handles the toggling of the "Data Save" checkbox.
 *
 * This function is called when the "Data Save" checkbox is toggled. If the checkbox is checked, it generates a save file.
 * Otherwise, it returns without performing any further action.
 *
 * @param checked A boolean indicating whether the "Data Save" checkbox is checked or not.
 */
void MainWindow::on_checkBox_dataSave_toggled(bool checked)
{
  if(!checked) return;
  data_->generateSaveFile();
}

/**
 * @brief Sets the color of the application based on the specified color index.
 *
 * This function sets the background color of the application and updates the palette based on the specified color index.
 * It also enables or disables the color change based on the specified enable state.
 *
 * @param colorindex An integer representing the color index:
 *                   - 1: Green
 *                   - 2: Yellow
 *                   - 3: Red
 *                   - 4: Blue
 *                   - Default: Gray
 * @param enable     A boolean indicating whether the color change is enabled or not.
 */
void MainWindow::setColor(int colorindex){
  QPalette pal = palette();
  int index = colorindex;
  QColor color = QColor(215, 214, 213, 255);
  switch (index) {
    case 1: //green
      ui->tabWidget->setStyleSheet("background-color: rgb(143, 188, 143)");
      color = QColor(143, 188, 143, 255);
      pal.setColor(QPalette::Window, color);
      this->setAutoFillBackground(true);
      this->setPalette(pal);
      this->setAutoFillBackground(false);
      break;
    case 2: //yellow
      ui->tabWidget->setStyleSheet("background-color: rgb(224, 195, 30)");
      color = QColor(224, 195, 30, 255);
      pal.setColor(QPalette::Window, color);
      this->setAutoFillBackground(true);
      this->setPalette(pal);
      this->setAutoFillBackground(false);
      break;
    case 3: //red
      ui->tabWidget->setStyleSheet("background-color: rgb(205, 92, 92)");
      color = QColor(205, 92, 92, 255);
      pal.setColor(QPalette::Window, color);
      this->setAutoFillBackground(true);
      this->setPalette(pal);
      this->setAutoFillBackground(false);
      break;
    case 4: //blue
      ui->tabWidget->setStyleSheet("background-color: rgb(153, 153, 255)");
      color = QColor(153, 153, 255, 255);
      pal.setColor(QPalette::Window, color);
      this->setAutoFillBackground(true);
      this->setPalette(pal);
      this->setAutoFillBackground(false);
      break;
    default: //gray
      ui->tabWidget->setStyleSheet("background-color: rgb(215, 214, 213)");
      color = QColor(215, 214, 213, 255);
      pal.setColor(QPalette::Window, color);
      this->setAutoFillBackground(true);
      this->setPalette(pal);
      this->setAutoFillBackground(false);
      break;
    }
}

/**
 * @brief Sends a message to LINE.
 *
 * This function sends the specified message to LINE using the LINE token stored in LINEToken_.
 *
 * @param message The message to be sent to LINE.
 */
void MainWindow::sendLINE(const QString& message){notify_->setLINE(message, LINEToken_);}

/**
 * @brief Adds the port names to the COM port list.
 *
 * This function adds the port names, serial numbers, and manufacturers of the COM ports
 * to the COM port list in the user interface. It also logs the information in the application's log.
 *
 * @param info A QList of QSerialPortInfo objects representing the COM ports found.
 */
void MainWindow::addPortName(QList<QSerialPortInfo> info){
    LogMsg("-------------- COM Ports found :");
    for (const QSerialPortInfo &portinfo : info) {
        LogMsg(portinfo.portName() + ", " + portinfo.serialNumber() + ", " + portinfo.manufacturer());
        ui->comboBox_SeriesNumber->addItem( portinfo.serialNumber(), (QString) portinfo.portName());
    }
    LogMsg ("--------------");
}


/**
 * @brief Updates the displayed temperature value.
 *
 * This function is a slot that is connected to the temperature update signal. It updates the displayed
 * temperature value in the user interface with the provided temperature value.
 *
 * @param temperature The new temperature value to be displayed.
 */
void MainWindow::updateTemperature(double temperature){
  ui->lineEdit_Temp->setText(QString::number(temperature) + " C");
}

/**
 * @brief Updates the displayed manipulated variable (MV) value.
 *
 * This function is a slot that is connected to the MV update signal. It updates the displayed MV value
 * in the user interface with the provided MV value.
 *
 * @param MV The new MV value to be displayed.
 */
void MainWindow::updateMV(double MV){
  ui->lineEdit_CurrentMV->setText(QString::number(MV) + " %");
}

/**
 * @brief Updates the displayed set value (SV).
 *
 * This function is a slot that is connected to the SV update signal. It updates the displayed SV value
 * in the user interface with the provided SV value.
 *
 * @param SV The new SV value to be displayed.
 */
void MainWindow::updateSV(double SV){
  ui->lineEdit_CurrentSV->setText(QString::number(SV) + " C");
}

/**
 * @brief Updates the upper range of the manipulated variable (MV) and the corresponding plot range.
 *
 * This function is a slot that is connected to the MV upper range update signal. It updates the upper range
 * of the MV in the user interface and adjusts the plot range accordingly.
 *
 * @param MVupper The new upper range of the MV.
 */
void MainWindow::updateMVupper(double MVupper){
  ui->doubleSpinBox_MVupper->setValue(MVupper);
  plot->yAxis2->setRangeUpper(MVupper + 2);
  plot->replot();
}

/**
 * @brief Updates the lower range of the manipulated variable (MV).
 *
 * This function is a slot that is connected to the MV lower range update signal. It updates the lower range
 * of the MV in the user interface with the provided MV lower value.
 *
 * @param MVlower The new lower range of the MV.
 */
void MainWindow::updateMVlower(double MVlower){
  ui->doubleSpinBox_MVlower->setValue(MVlower);
}

/**
 * @brief Updates the displayed proportional gain (P) value of the PID controller.
 *
 * This function is a slot that is connected to the PID P value update signal. It updates the displayed P value
 * in the user interface with the provided PID P value.
 *
 * @param PID_P The new PID P value to be displayed.
 */
void MainWindow::updatePID_P(double PID_P){
  ui->lineEdit_P->setText(QString::number(PID_P));
}

/**
 * @brief Updates the displayed integral gain (I) value of the PID controller.
 *
 * This function is a slot that is connected to the PID I value update signal. It updates the displayed I value
 * in the user interface with the provided PID I value.
 *
 * @param PID_I The new PID I value to be displayed.
 */
void MainWindow::updatePID_I(double PID_I){
  ui->lineEdit_I->setText(QString::number(PID_I));
}

/**
 * @brief Updates the displayed derivative gain (D) value of the PID controller.
 *
 * This function is a slot that is connected to the PID D value update signal. It updates the displayed D value
 * in the user interface with the provided PID D value.
 *
 * @param PID_D The new PID D value to be displayed.
 */
void MainWindow::updatePID_D(double PID_D){
  ui->lineEdit_D->setText(QString::number(PID_D));
}

/**
 * @brief Updates the check number for temperature change.
 *
 * This function is a slot that is connected to the check number update signal. It displays a log message indicating
 * the current number of temperature checks being performed.
 *
 * @param checkNumber The current number of temperature checks.
 */
void MainWindow::updateCheckNumber(int checkNumber){
  if (checkNumber == 0) return;
  ui->textEdit_Log->setTextColor(QColor(255, 0, 0, 255));
  LogMsg("Checking temperature change in " + QString::number(checkNumber));
  ui->textEdit_Log->setTextColor(QColor(0, 0, 0, 255));
  ui->lineEdit_msg->setText("Checking temperature change in " + QString::number(checkNumber));
  setColor(2);
}

/**
 * @brief Updates the status display with the current data update timestamp.
 *
 * This function is a slot that is connected to the status update signal. It updates the status display with the current
 * data update timestamp.
 */
void MainWindow::updateStatus(){
  QDateTime date = QDateTime::currentDateTime();
  QString datestr = date.toString("HH:mm:ss").toStdString().c_str();
  ui->lineEdit_msg->setEnabled(true);
  ui->lineEdit_msg->setText("Data is updated @ " + datestr);
  ui->lineEdit_msg->setEnabled(false);
}

/**
 * @brief Receives a log message and passes it to the LogMsg function.
 *
 * This function is a slot that is connected to the log message signal. It receives a log message and passes it
 * to the LogMsg function to be displayed in the log text area.
 *
 * @param msg The log message.
 */
void MainWindow::catchLogMsg(const QString& msg) {LogMsg(msg);}

/**
 * @brief Receives a log message with a specified color and passes it to the LogMsg function.
 *
 * This function is a slot that is connected to the log message with color signal. It receives a log message
 * along with a color and passes it to the LogMsg function to be displayed in the log text area with the specified color.
 *
 * @param msg The log message.
 * @param color The color of the log message.
 */
void MainWindow::catchLogMsgWithColor(const QString &msg, QColor color){
  ui->textEdit_Log->setTextColor(color);
  LogMsg(msg);
  ui->textEdit_Log->setTextColor(QColor(0, 0, 0, 255));
}

/**
 * @brief Connects to the Omron temperature control device.
 *
 * This function is called when the device is successfully connected. It updates the UI to reflect the connection status,
 * displays a log message indicating the connection, and performs necessary UI updates and settings.
 */
void MainWindow::connectDevice(){
  ui->textEdit_Log->setTextColor(QColor(0,0,255,255));
  LogMsg("The Omron temperature control is connected in " + com_->getPortName() + ".");
  ui->textEdit_Log->setTextColor(QColor(0,0,0,255));
  ui->comboBox_SeriesNumber->setEnabled(false);
  ui->pushButton_Connect->setStyleSheet("background-color: rgb(255,127,80)");
  ui->pushButton_Connect->setEnabled(false);
  panalOnOff(true);
  ui->pushButton_Control->setEnabled(true);
  ui->lineEdit_SV2->setEnabled(false);
  QString title = this->windowTitle();
  this->setWindowTitle(title + " | " + ui->comboBox_SeriesNumber->currentText());
  getSetting();
  ui->lineEdit_SV->setText(QString::number(data_->getSV()));
  LogMsg("Set Stop.");
  QColor color = QColor("palegray");
  QPalette pal = palette();
  pal.setColor(QPalette::Window, color);
}

/**
 * @brief Handles the failure to connect to the Omron temperature control device.
 *
 * This function is called when the connection to the device fails. It updates the UI to reflect the connection status,
 * displays a log message indicating the failure, and re-enables the COM port selection.
 */
void MainWindow::connectFailed(){
    ui->textEdit_Log->setTextColor(QColor(255,0,0,255));
    LogMsg("The Omron temperature control cannot be connected on any COM port.");
    ui->textEdit_Log->setTextColor(QColor(0,0,0,255));
    ui->comboBox_SeriesNumber->setEnabled(true);
    ui->pushButton_Connect->setStyleSheet("");
}

/**
 * @brief Handles the completion of the AT (Automatic Tuning) command.
 *
 * This function is called when the AT command is sent to the temperature control device and the command execution is completed.
 * The @p atFlag parameter indicates the result of the AT command:
 *   - If @p atFlag is 1, the AT command was set to 100% and the Set Point is disabled.
 *   - If @p atFlag is 2, the AT command was set to 40% and the Set Point is disabled.
 *   - If @p atFlag is any other value, the AT command is set to none and the Set Point is enabled.
 * This function updates the UI elements accordingly and displays a log message indicating the result of the AT command.
 *
 * @param atFlag The result of the AT command.
 */
void MainWindow::finishSendAT(int atFlag){
  if (atFlag == 1){
    ui->lineEdit_SV->setEnabled(false);
    ui->pushButton_SetSV->setEnabled(false);
    LogMsg("Set AT to 100%., disable Set Point.");
  } else if (atFlag == 2){
    ui->lineEdit_SV->setEnabled(false);
    ui->pushButton_SetSV->setEnabled(false);
    LogMsg("Set AT to 40%. disable Set Point.");
  } else {
    ui->lineEdit_SV->setEnabled(true);
    ui->pushButton_SetSV->setEnabled(true);
    LogMsg("Set AT to none.");
  }
}

/**
 * @brief Handles the completion of setting the target temperature (SV).
 *
 * This function is called when the target temperature (SV) is set and the command execution is completed.
 * The @p SV parameter represents the value of the target temperature.
 * This function displays a log message indicating the target temperature that was set.
 *
 * @param SV The value of the target temperature that was set.
 */
void MainWindow::finishSendSV(double SV){
   LogMsg("Target temperature is set to be " + QString::number(SV));
}

/**
 * @brief Handles the detection of a danger signal.
 *
 * This function is called when a danger signal is detected based on the @p type parameter:
 *   - If @p type is 0, it indicates that the maximum allowed temperature has been exceeded.
 *   - If @p type is 1, it indicates that even though the MV output is at the maximum, the temperature change is less than the threshold.
 *   - If @p type is 2, it indicates that the temperature has dropped below the threshold.
 *   - If @p type is 3, it indicates that the temperature continued to drop for some intervals.
 *   - If @p type is any other value, it indicates a general danger signal.
 *
 * This function displays a log message describing the detected danger signal.
 * It also updates the UI to indicate the emergency stop, displaying the current date and time in the message field and applying styling to highlight it.
 * Finally, it calls the Quit() function to perform the necessary actions for an emergency stop.
 *
 * @param type The type of danger signal that was detected.
 */
void MainWindow::catchDanger(int type){
  ui->textEdit_Log->setTextColor(QColor(255,0,0,255));
  switch (type){
    case 0 :
      LogMsg("The maximum allowed temperature has been exceeded.");
      break;
    case 1 :
      LogMsg("Even though the MV output is maximum, the temperature change is less than the threshold");
      break;
    case 2 :
      LogMsg("Temperature is Droped over the threshold");
      break;
    case 3 :
      LogMsg("Temperature continued to drop for some intervals.");
      break;
    default :
      LogMsg("Danger Signal is detectived.");
      break;
  }
  ui->textEdit_Log->setTextColor(QColor(0,0,0,255));
  QDateTime date = QDateTime::currentDateTime();
  QString datestr = date.toString("yyyyMMdd_HHmmss");
  ui->lineEdit_msg->setStyleSheet("background-color:yellow; color:red;selection-background-color:red;");
  ui->lineEdit_msg->setText("Emergency Stop at " + datestr);
  Quit();
}

/**
 * @brief Handles the escape from TempChangeCheck mode.
 *
 * This function is called when the system escapes from the TempChangeCheck mode based on the @p sign parameter:
 *   - If @p sign is 0, it indicates that the current MV has decreased below the MVupper threshold.
 *   - If @p sign is 1, it indicates that the current temperature is within the ignore range.
 *   - If @p sign is any other value, it indicates a general escape from TempChangeCheck mode.
 *
 * This function updates the UI to display the appropriate color, and it displays a log message indicating the reason for the escape from TempChangeCheck mode.
 *
 * @param sign The sign indicating the reason for the escape from TempChangeCheck mode.
 */
void MainWindow::cathcEscapeTempCheckChange(int sign){
  setColor(1);
  ui->textEdit_Log->setTextColor(QColor(0, 0, 255, 255));
  if (sign == 0){
      LogMsg("Escape TempChangeCheck mode because current MV decrease belows MVupper.");
  } else if (sign ==1) {
      LogMsg("Escape TempChangeCheck mode because current temperature is in ignore range.");
  } else {
      LogMsg("Escape TempChangeCheck mode");
  }
  ui->textEdit_Log->setTextColor(QColor(0, 0, 0, 255));
}

/**
 * @brief Handles the start of TempChangeCheck mode.
 *
 * This function is called when the system starts the TempChangeCheck mode based on the @p checknumber parameter.
 * If @p checknumber is 0, it indicates that the current MV has reached the MV upper threshold, and the TempChangeCheck mode is initiated.
 * Otherwise, it indicates that the system is in TempChangeCheck mode with the specified number of checks.
 *
 * This function updates the UI to display the appropriate color, and it displays a log message indicating the start of TempChangeCheck mode.
 *
 * @param checknumber The number of checks in the TempChangeCheck mode.
 */
void MainWindow::catchStartTempChangeCheck(int checknumber){
  setColor(2);
  ui->textEdit_Log->setTextColor(QColor(255, 0, 0, 255));
  if (checknumber == 0){
      LogMsg("current MV reached MV upper.");
      LogMsg("Start Temp Change Check mode.");
  } else {
      LogMsg("Temp Change Check mode in " + QString::number(checknumber));
  }
  ui->textEdit_Log->setTextColor(QColor(0, 0, 0, 255));
}

/**
 * @brief Sets the interval for the plot timer.
 *
 * This function sets the interval for the plot timer based on the specified @p interval.
 * The @p interval is given in seconds and is converted to milliseconds before setting the timer interval.
 * If the plot timer is already active, it is stopped and restarted with the new interval.
 * If the plot timer is not active, the interval is set but the timer is not started.
 *
 * @param interval The interval for the plot timer in seconds.
 */
void MainWindow::setIntervalPlot(int interval){
  intervalPlot_ = interval * 1000;
  if (plotTimer_->isActive()){
    plotTimer_->stop();
    plotTimer_->setInterval(intervalPlot_);
    plotTimer_->start();
  } else {
    plotTimer_->stop();
    plotTimer_->setInterval(intervalPlot_);
  }
}

/**
 * @brief Updates the status checkboxes and sets the color based on the system status.
 *
 * This function updates the state of the QCheckBox widgets based on the activity status of various QTimer objects.
 * It retrieves the activity status of the timers from the corresponding safety, data, and com objects.
 * The function also updates the color of the UI based on the system status.
 * If the system is in a quit state, the color is set to indicate a stopped state.
 * If the system is running and the TempChangeCheck timer is active, the color is set to indicate TempChangeCheck mode.
 * If the system is running, but the TempChangeCheck timer is not active, the color is set to indicate normal running mode.
 * If the system is not running, the color is set to indicate a stopped state.
 *
 * Note: The behavior of this function depends on the implementation of the various objects and their timers.
 * Please ensure that the objects are correctly initialized and the timers are properly managed for accurate functionality.
 */
void MainWindow::updateStatusBoxes(){
  // Update the state of the QCheckBox widgets based on the activity status of the QTimer objects
  bool is_mvcheck_running = safety_->isTimerMVCheckRunning();
  ui->checkBoxStatusPeriodic->setChecked(is_mvcheck_running);
  bool is_tempchange_running = safety_->isTimerTempChangeCheckRunning();
  ui->checkBoxStautsTempCheck->setChecked(is_tempchange_running);
  bool is_log_running = data_->isTimerLogRunning();
  ui->checkBoxStatusRecord->setChecked(is_log_running);
  bool is_update_running = com_->isTimerUpdateRunning();
  ui->checkBoxUpdate->setChecked(is_update_running);
  bool is_run = ui->pushButton_RunStop->isChecked();
  LogMsg(QString::number(isQuit_));

  if (isQuit_) {
      setColor(3);
      return;
  }

  if (is_run && is_tempchange_running) {
      setColor(2);
  } else if (is_run) {
      setColor(1);
  } else {
      setColor(0);
  }
}

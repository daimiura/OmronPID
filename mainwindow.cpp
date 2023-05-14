//!
//! \file mainwindow.cpp
//! \brief mainwindow for connection to the E5CC
//! \author Daisuke Miura (Original source code xxxxx)
//! \date 2023/3/15
//! \details The original source code did not include safety measures.
//! To improve safety, functions were added to check if the temperature was rising and to detect unexpected sudden temperature drops.
//! The GUI was also modified to set the necessary parameters for these functions.
//!
//!
#include "communication.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "safety.h"
#include "datasummary.h"


MainWindow::MainWindow(QWidget *parent) :
  QMainWindow(parent),
  ui(new Ui::MainWindow),
  plot(new QCustomPlot),
  clock(new QTimer),
  waitTimer(new QTimer),
  LINEToken_("9tYexDQw9KHKyJOAI5gIONbXLZgzolIxungdwos5Dyy")
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
  connect(com_, &Communication::ATSendFinish, this, &MainWindow::finishSendAT);
  connect(com_, &Communication::SVSendFinish, this, &MainWindow::finishSendSV);
  connect(com_, &Communication::serialPortRemove, this, &MainWindow::sendLINE);
  addPortName(com_->getSerialPortDevices());

  //Generate instance to use DataSummary class.
  data_ = new DataSummary(com_);
  connect(data_, &DataSummary::FileSave, this, &MainWindow::saveFile);
  ui->lineEdit_DirPath->setText(data_->getFilePath());

  //Generate instance to use Safety class.
  safety_ = new Safety(data_);
  safety_->setPermitedMaxTemp(ui->spinBox_TempUpper->value());
  connect(safety_, &Safety::dangerSignal, this, &MainWindow::catchDanger);
  connect(safety_, &Safety::checkNumberChanged, this, &MainWindow::updateCheckNumber);
  connect(safety_, &Safety::escapeTempCheckChange, this, &MainWindow::cathcEscapeTempCheckChange);
  connect(safety_, &Safety::startTempChangeCheck, this, &MainWindow::catchStartTempChangeCheck);

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
  connect(com_->getTimerUpdate(), &QTimer::timeout, this, &MainWindow::updateStatusBoxes);
  connect(safety_->getTimerMVCheck(), &QTimer::timeout, this, MainWindow::updateStatusBoxes);
  connect(safety_->getTimerTempChangeCheck(), &QTimer::timeout, this, MainWindow::updateStatusBoxes);
  connect(data_->getLogTimer(), &QTimer::timeout, this, &MainWindow::updateStatusBoxes);

  timing_ = com_->timing::clockUpdate;
  connect(clock, SIGNAL(timeout()), this, SLOT(showTime()));
  connect(waitTimer, SIGNAL(timeout()), this, SLOT(allowSetNextSV()));
}


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


void MainWindow::keyPressEvent(QKeyEvent *key)
{
    if(key->key() == Qt::Key_Shift ){
        plot->axisRect()->setRangeDrag(Qt::Horizontal);
        plot->axisRect()->setRangeZoom(Qt::Horizontal);
    }
}

void MainWindow::keyReleaseEvent(QKeyEvent *key)
{
    if(key->key() == Qt::Key_Shift ){
        plot->axisRect()->setRangeDrag(Qt::Vertical);
        plot->axisRect()->setRangeZoom(Qt::Vertical);
    }
}

void MainWindow::waitForMSec(int msec)
{
    //wait for waitTime
    QEventLoop eventLoop;
    QTimer::singleShot(msec, &eventLoop, SLOT(quit()));
    eventLoop.exec();
}

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

void MainWindow::allowSetNextSV(){
    nextSV = true;
}

void MainWindow::on_pushButton_AskStatus_clicked(){
  com_->askTemperature();
  com_->askSV();
  com_->askMV();
}


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

void MainWindow::on_pushButton_SetSV_clicked(){
  com_->executeSendRequestSV(ui->lineEdit_SV->text().toDouble());
  LogMsg("Set Temperature is changed to " + ui->lineEdit_SV->text() + " C.");
}


//Want to rewrite.....
void MainWindow::on_pushButton_Control_clicked()
{
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

    double iniTemp = 0;
    if(tempControlOnOff){
        on_pushButton_AskStatus_clicked();
        iniTemp = com_->getTemperature();
        LogMsg("Current Temperature         : " + QString::number(com_->getTemperature()) + " C.");

        const double targetValue = ui->lineEdit_SV->text().toDouble();
        const int tempGetTime = ui->spinBox_TempRecordTime->value() * 1000; // msec
        int tempWaitTime = ui->spinBox_TempStableTime->value() * 60 * 1000; // msec
        const double tempTorr = ui->doubleSpinBox_TempTorr->value();
        const double tempStepSize = ui->doubleSpinBox_TempStepSize->value();
        const int mode = ui->comboBox_Mode->currentData().toInt();
        const double targetValue_2 = ui->lineEdit_SV2->text().toDouble();
        const double targetValue_2_waitTime = ui->doubleSpinBox_SV2WaitTime->value() * 60.*1000.;
        if( mode == 4){
            tempWaitTime = tempWaitTime * tempStepSize;
        }

        if( mode == 4){
            LogMsg("First go to : " + QString::number(targetValue_2) + " C. normaly.");
            LogMsg("Then go to  : " + QString::number(targetValue) + " C. at fixed rate.");
        }else{
            LogMsg("Target Temperature          : " + QString::number(targetValue) + " C.");
        }

        //============= estimate total time
        double estTransitionTime = 5; //min
        if( mode == 2 || mode == 3 || mode == 4) estTransitionTime = 0;
        double estSlope = (estTransitionTime + tempWaitTime/60/1000) / tempStepSize ;
        double estTotalTime = estSlope * qAbs(com_->getTemperature()-targetValue);
        if(mode == 4){
            estSlope = ui->spinBox_TempStableTime->value(); // min/C
            estTotalTime = estSlope * qAbs(targetValue_2-targetValue);
        }

        QMessageBox box;
        QString boxMsg;
        if( mode == 1){
            LogMsg("======== Stable Mode ==========");
            boxMsg.asprintf("======== Stable Mode ========== \n"
                           "Estimated transition time : %6.1f min. \n"
                           "Estimated gradience       : %6.1f min/C \n"
                           "Estimated total time      : %6.1f min = %6.1f hr",
                           estTransitionTime,
                           estSlope,
                           estTotalTime, estTotalTime/60.);
        }else if(mode == 2){
            LogMsg("======== Fixed Time Mode ==========");
            boxMsg.asprintf("======== Fixed Time Mode ========== \n"
                           "Estimated gradience  : %6.1f min/C \n"
                           "Estimated total time : %6.1f min = %6.1f hr",
                           estSlope,
                           estTotalTime, estTotalTime/60.);
        }else if(mode == 3){
            LogMsg("======== Fixed Rate Mode ==========");
            boxMsg.asprintf("======== Fixed Rate Mode ========== \n"
                           "Set-temp Gradience   : %6.1f min/C \n"
                           "Estimated total time : %6.1f min = %6.1f hr",
                           estSlope,
                           estTotalTime, estTotalTime/60.);
        }else if(mode == 4){
            LogMsg("======== Normal + Fixed Rate Mode ==========");
            boxMsg.asprintf("======== Normal + Fixed Rate Mode ========== \n"
                           "1) Go to %5.1f C using normal mode.\n"
                           "   Time unknown. \n"
                           "2) Fixed rate to go to %5.1f C\n"
                           "   Fixed-rate Set-temp Gradience   : %6.1f min/C \n"
                           "   Estimated fixed-rate time       : %6.1f min = %6.1f hr",
                           targetValue_2, targetValue,
                           estSlope,
                           estTotalTime, estTotalTime/60.);
        }
        LogMsg("Estimated transiton Time : " + QString::number(estTransitionTime) + " min.");
        LogMsg("Estimated gradience      : " + QString::number(estSlope) + " min/C.");
        LogMsg("Estimated total Time     : " + QString::number(estTotalTime) + " min. = " + QString::number(estTotalTime/60.) + " hr.");
        box.setText(boxMsg);
        box.setInformativeText("Do you want to proceed ?");
        box.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
        box.setDefaultButton(QMessageBox::Cancel);
        if(box.exec() == QMessageBox::Cancel) {
            tempControlOnOff = false;
            ui->pushButton_Control->setStyleSheet("");
            panalOnOff(true);
            ui->actionOpen_File->setEnabled(true);
            on_comboBox_Mode_currentIndexChanged(ui->comboBox_Mode->currentIndex());
            LogMsg("=============== Slow Temperature control cancelled.======");
            return;
        }

        // set output file =================
        QDateTime startTime = QDateTime::currentDateTime();
        QString fileName = startTime.toString("yyyyMMdd_HHmmss")
                + "_tempControl_mode" + QString::number(mode)
                + "_"  + ui->comboBox_SeriesNumber->currentText() +".dat";
        QString filePath = filePath_ + "/" + fileName;
        LogMsg("data save to : " + filePath);
        QFile outfile(filePath);

        outfile.open(QIODevice::WriteOnly| QIODevice::Text);
        QTextStream stream(&outfile);
        QString lineout;

        lineout.asprintf("###%s", startTime.toString("yyyy-MM-dd HH:mm:ss\n").toStdString().c_str());
        stream << lineout;
        if( mode == 1){
            lineout = "### Control mode          :  Stable Temperature.\n";
        }else if(mode == 2){
            lineout = "### Control mode          :  Fixed Time.\n";
        }else if(mode == 3){
            lineout = "### Control mode          :  Set-temp Fixed Rate. \n";
        }else if(mode == 4){
            lineout = "### Control mode          :  Normal + Set-temp Fixed Rate. \n";
        }
        stream << lineout;
        if( mode == 1){
            lineout = "### Target Temperature          : " + QString::number(targetValue) + " C.\n";
            stream << lineout;
            lineout = "### Temperature  stable time    : " + QString::number(tempWaitTime) + " min.\n";
            stream << lineout;
            lineout = "### Temperature tolerance       : " + QString::number(tempTorr) + " C.\n";
            stream << lineout;
        }else if(mode == 2){
            lineout = "### Target Temperature      : " + QString::number(targetValue) + " C.\n";
            stream << lineout;
            lineout = "### Set-temp change time    : " + QString::number(tempWaitTime) + " min.\n";
            stream << lineout;
        }else if(mode == 3){
            lineout = "### Target Temperature      : " + QString::number(targetValue) + " C.\n";
            stream << lineout;
            lineout = "### Set-temp change rate    : " + QString::number(tempWaitTime/60./1000.) + " min/C.\n";
            stream << lineout;
        }else if(mode == 4){
            lineout = "### Set-temp of normal      : " + QString::number(targetValue_2) + " C.\n";
            stream << lineout;
            lineout = "### Set-temp of fixed rate  : " + QString::number(targetValue) + " C.\n";
            stream << lineout;
            lineout = "### Set-temp change rate    : " + QString::number(tempWaitTime/60./1000.) + " min/C.\n";
            stream << lineout;
        }
        lineout.asprintf("###%11s,\t%12s,\t%10s,\t%10s,\t%10s\n", "Date", "Date_t", "temp [C]", "SV [C]", "Output [%]");
        stream << lineout;
        stream.flush();

        clock->setSingleShot(false);
        clock->start(timing_);
        totalElapse.start(); // used for clock
        QTimer getTempTimer;
        getTempTimer.setSingleShot(true);

        //########################### mode 4 extra code, go to targetValue_2
        //----- set SV
        com_->executeSendRequestSV(targetValue_2);

        //----- wait for temp reach targetValue_2, while recording temperature.
        pvData.clear();
        svData.clear();
        mvData.clear();
        //muteLog = ui->checkBox_MuteLogMsg->isChecked();
        bool targetValue_2_Reached = false;
        bool waitTimerStarted = false;
        waitTimer->setSingleShot(true);
        while(tempControlOnOff && mode == 4 && !targetValue_2_Reached){
            getTempTimer.start(tempGetTime);
            double temperature = data_->getTemperature();
            double MV = data_->getMV();
            QDateTime date = QDateTime::currentDateTime();
            fillDataAndPlot(date, temperature, targetValue_2, MV);
            data_->writeData();
            while(getTempTimer.remainingTime() > 0 ){
                waitForMSec(com_->timing::getTempTimer);
                if(waitTimer->remainingTime() <= 0 && waitTimerStarted == true){
                    targetValue_2_Reached = true; // break the getTemp loop
                    muteLog = false;
                    LogMsg("Target Set-temp stable. Start fixed rate. Elapse time : " + QString::number(totalElapse.elapsed()/1000./60) + " mins.");
                    lineout.asprintf("### fixed-rate start.\n");
                    stream << lineout;
                    stream.flush();

                    break; // break this loop
                }
            }

            if(temperature == targetValue_2 && waitTimerStarted == false){
                waitTimer->start(targetValue_2_waitTime);
                waitTimerStarted = true;
                muteLog = false;
                LogMsg("Target Set-temp reached : " + QString::number(targetValue_2) + " C. Elapse time : " + QString::number(totalElapse.elapsed()/1000./60) + " mins.");
                LogMsg("wait for 10 mins.");
                lineout.asprintf("### Target Set-temp reached : %5.1f C\n", targetValue_2);
                stream << lineout;
                stream.flush();
            }

        }
        muteLog = false;

        //Looping ========================
        double temperature = data_->getTemperature();
        double smallShift = temperature;
        LogMsg("Present Temperature : " + QString::number(temperature) + " C.");
        const int direction = (temperature > targetValue ) ? (-1) : 1;
        LogMsg("Temperature step            : " + QString::number(direction * tempStepSize) + " C.");

        if( mode == 1){
            waitTimer->setSingleShot(true);
        }else{
            waitTimer->setSingleShot(false);
            waitTimer->start(tempWaitTime);
            waitTimerStarted = true;
        }
        while(tempControlOnOff){
            nextSV = false;
            if( mode == 1) {
                waitTimer->stop();
                waitTimerStarted = false;
            }
            //----------------Set SV
            if( mode == 1 || mode == 2 ){
                if(direction * (targetValue - temperature) >= tempStepSize){
                    // when direction is +1, when temperature smaller than tempStepSize, incrase smallshift by a step size.
                    smallShift = temperature + direction * tempStepSize  ;
                }else{
                    //else, smallshift = target value.
                    smallShift = targetValue;
                }
            }

            if( mode == 3 || mode == 4){
                if(direction * (targetValue - smallShift) >= tempStepSize){
                    smallShift = smallShift + direction * tempStepSize  ;
                }else{
                    smallShift = targetValue;
                }
            }

            ui->lineEdit_CurrentSV->setText(QString::number(smallShift) + " C");
            LogMsg("==== Set-temp : " + QString::number(smallShift) + " C. Elapse Time : " + QString::number(totalElapse.elapsed()/1000./60.) + " mins.");

            //setSV(smallShift);
            com_->executeSendRequestSV(smallShift);
            do{
                getTempTimer.start(tempGetTime);
                qDebug()  << "temp control. do-loop 1 = " << tempControlOnOff;
                if(!tempControlOnOff) break;

                //com_->askTemperature();
                //com_->askMV();

                QDateTime date = QDateTime::currentDateTime();
                double MV = data_->getMV();
                fillDataAndPlot(date, temperature, smallShift, MV);
                data_->writeData();
                while(getTempTimer.remainingTime() > 0 ){
                    waitForMSec(com_->timing::getTempTimer);
                    if( nextSV == true){
                        break;
                    }
                }
                muteLog=false;
                //TODO under friQt::endly display
                if(waitTimerStarted){
                    LogMsg(" x", false);
                }else{
                    LogMsg(" /", false);
                }

                if( mode == 1){ //========== for stable mode
                    if( qAbs(temperature - smallShift) <= tempTorr){
                        if( waitTimerStarted == false){
                            waitTimer->start(tempWaitTime);
                            waitTimerStarted = true;
                        }
                    }else{
                        waitTimer->stop();
                        waitTimerStarted = false;
                        nextSV = false;
                    }
                    if( waitTimerStarted == true){
                        double stableTime = (tempWaitTime - waitTimer->remainingTime())/1000.;
                        LogMsg( " temperature stable for : " +  QString::number(stableTime) + " sec. |"
                         + QString::number(smallShift) + " - " + QString::number(temperature) + "| < "  + QString::number(tempTorr) + " C");
                    }
                }

            }while( !nextSV  && tempControlOnOff );
            muteLog = false;

            if (smallShift == targetValue){
                if( mode == 1 ) {
                    lineout = "###=========== Target Temperature Reached =============";
                }else if( mode == 2 || mode == 3 || mode == 4) {
                    lineout = "###=========== Time Up  =============";
                }
                stream << lineout;
                stream.flush();
                LogMsg(lineout);
                break;
            }
        }

        double totalTime = totalElapse.elapsed() /1000./60. + dayCounter * 24. * 60.; // min
        LogMsg("Total time : " + QString::number(totalTime) + " mins = " + QString::number(totalTime/60.) + " hours.");
        double tempChanged = qAbs(iniTemp - temperature);
        LogMsg("Average gradience : " + QString::number(totalTime/tempChanged) + " min/C." );

        //=========== now is the E5CC control
        //only measure temperature
        muteLog = true;
        while(tempControlOnOff){
            getTempTimer.start(tempGetTime);
            qDebug()  << "temp control. do-loop 2 = " << tempControlOnOff;
            //com_->askTemperature();
            //com_->askMV();
            double temperature = data_->getTemperature();
            double MV = data_->getMV();
            QDateTime date = QDateTime::currentDateTime();
            fillDataAndPlot(date, temperature, smallShift, MV);
            data_->writeData();
            while(getTempTimer.remainingTime() != -1 ){
                waitForMSec(com_->timing::getTempTimer);
            }

        };
        muteLog = false;
    }
  ui->checkBoxStatusSTC->setChecked(false);
}

void MainWindow::on_comboBox_AT_currentIndexChanged(int index)
{
    if(!comboxEnable) return;
    com_->executeSendRequestAT(index);
}

void MainWindow::on_pushButton_Connect_clicked(){
  com_->setSerialPortName(ui->comboBox_SeriesNumber->currentData().toString());
  com_->executeConnection();
}

void MainWindow::on_doubleSpinBox_MVlower_valueChanged(double arg1){
    if(!spinBoxEnable) return;
    com_->changeMVlowerValue(arg1);
    LogMsg("Output lower limit is set to be " + QString::number(arg1));
}

void MainWindow::on_doubleSpinBox_MVupper_valueChanged(double arg1){
    if(!spinBoxEnable) return;
    com_->changeMVupperValue(arg1);
    safety_->setMVUpper(arg1);
    LogMsg("Output upper limit is set to be " + QString::number(arg1));
    plot->yAxis2->setRangeLower(com_->getMVupper() + 2);
    plot->replot();
}

void MainWindow::on_pushButton_GetPID_clicked() {com_->askPID("PID");}

void MainWindow::on_comboBox_Mode_currentIndexChanged(int index){
    if(!comboxEnable) return;
    if(index == 1){
        ui->doubleSpinBox_TempTorr->setEnabled(false);
        ui->doubleSpinBox_TempStepSize->setEnabled(true);
        ui->doubleSpinBox_TempStepSize->setValue(0.5);
        ui->comboBox_Mode->setStyleSheet("color: #FF0000");
        ui->label_TimeStable->setStyleSheet("color: #FF0000");
        ui->label_TimeStable->setText("Set-temp changes [min] :");
        ui->label_SV_2->setStyleSheet("");
        ui->lineEdit_SV2->setEnabled(false);
        ui->doubleSpinBox_SV2WaitTime->setEnabled(false);
        ui->label_SV2WaitTime->setStyleSheet("");
    }else if(index == 0){
        ui->doubleSpinBox_TempTorr->setEnabled(true);
        ui->doubleSpinBox_TempStepSize->setEnabled(true);
        ui->doubleSpinBox_TempStepSize->setValue(0.5);
        ui->comboBox_Mode->setStyleSheet("");
        ui->label_TimeStable->setStyleSheet("");
        ui->label_TimeStable->setText("Temp. stable for [min] :");
        ui->label_SV_2->setStyleSheet("");
        ui->lineEdit_SV2->setEnabled(false);
        ui->doubleSpinBox_SV2WaitTime->setEnabled(false);
        ui->label_SV2WaitTime->setStyleSheet("");
    }else if(index == 2){
        ui->doubleSpinBox_TempTorr->setEnabled(false);
        ui->doubleSpinBox_TempStepSize->setEnabled(false);
        ui->doubleSpinBox_TempStepSize->setValue(0.1);
        ui->comboBox_Mode->setStyleSheet("color: #0000FF");
        ui->label_TimeStable->setStyleSheet("color: #0000FF");
        ui->label_TimeStable->setText("Set-temp rate [min/C] :");
        ui->label_SV_2->setStyleSheet("");
        ui->lineEdit_SV2->setEnabled(false);
        ui->doubleSpinBox_SV2WaitTime->setEnabled(false);
        ui->label_SV2WaitTime->setStyleSheet("");
    }else if( index == 3){
        ui->doubleSpinBox_TempTorr->setEnabled(false);
        ui->doubleSpinBox_TempStepSize->setEnabled(false);
        ui->doubleSpinBox_TempStepSize->setValue(0.1);
        ui->comboBox_Mode->setStyleSheet("color: #006325");
        ui->label_TimeStable->setStyleSheet("color: #006325");
        ui->label_TimeStable->setText("Set-temp rate [min/C] :");
        ui->lineEdit_SV2->setEnabled(true);
        ui->label_SV_2->setStyleSheet("color: #006325");
        ui->lineEdit_SV2->setText("92");
        ui->doubleSpinBox_SV2WaitTime->setEnabled(true);
        ui->label_SV2WaitTime->setStyleSheet("color: #006325");
        ui->doubleSpinBox_SV2WaitTime->setValue(10.);
    }
}

void MainWindow::on_comboBox_MemAddress_currentTextChanged(const QString &arg1)
{
    if(!comboxEnable) return;
    quint16 address = ui->comboBox_MemAddress->currentData().toUInt();
    LogMsg("--------- read " + arg1);
    com_->read(QModbusDataUnit::HoldingRegisters, address, 2);
}

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


//!
//! \brief MainWindow::fillDifference
//! \param mute
//! \return double Temperature difference
//! \details Function to subtract the last element from the last element stored in the member variable vtemp_ to obtain the temperature change.
//! If the argument mute is true, the temperature change is not output to the Log message.
//!
double MainWindow::fillDifference(bool mute){
  int size = valltemp_.size();
  if(size < 2) return .0;
  double diff = valltemp_.at(size-1) - valltemp_.at(size-2);
  vdifftemp_.push_back(diff);
  ui->lineEdit_CurrentTempDiff->setText(QString::number(diff) + " C");
  if (!mute) LogMsg("Difference is " + QString::number(diff));
  return diff;
}

void MainWindow::on_actionHelp_Page_triggered(){
    if( helpDialog->isHidden() ) helpDialog->show();
}

void MainWindow::HelpPicNext()
{
  if( picNumber == 4){
      QPixmap image(":fig1.PNG");
      HelpLabel->setPixmap(image);
      picNumber = 1;
      return;
  }
  if( picNumber == 3){
      QPixmap image(":fig2.PNG");
      HelpLabel->setPixmap(image);
      picNumber++;
      return;
  }
  if( picNumber == 2){
      QPixmap image(":fig1.PNG");
      HelpLabel->setPixmap(image);
      picNumber++;
      return;
  }
  if( picNumber == 1){
      QPixmap image(":fig2.PNG");
      HelpLabel->setPixmap(image);
      picNumber++;
      return;
  }
}

void MainWindow::on_action_Setting_plot_triggered(){
  if( plotDialog_->isHidden() ) plotDialog_->show();
}

void MainWindow::on_action_Setting_parameters_for_TempCheck_triggered(){
  if(configureDialog_->isHidden()) configureDialog_->show();
}

/*
void MainWindow::on_action_Setting_Temperature_Drop_triggered(){
  if (tempDropDialog_->isHidden()) tempDropDialog_->show();
}
*/

void MainWindow::setIntervalAskMV(){
  ui->lineEdit_IntervalAskMV->setEnabled(true);
  ui->lineEdit_IntervalAskMV->setText(QString::number(configureDialog_->intervalAskMV_));
  safety_->stop();
  safety_->setIntervalMVCheck(configureDialog_->intervalAskMV_);
  ui->lineEdit_IntervalAskMV->setEnabled(false);
}

void MainWindow::setIntervalAskTemp(){
  ui->lineEdit_IntervalAskTemp->setEnabled(true);
  ui->lineEdit_IntervalAskTemp->setText(QString::number(configureDialog_->intervalAskTemp_));
  safety_->stop();
  safety_->setIntervalTempChange(configureDialog_->intervalAskTemp_);
  ui->lineEdit_IntervalAskTemp->setEnabled(false);
}

void MainWindow::setNumbers(){
  ui->lineEdit_Numbers->setEnabled(true);
  ui->lineEdit_Numbers->setText(QString::number(configureDialog_->numbers_));
  safety_->stop();
  safety_->setNumberOfCheck(configureDialog_->numbers_);
  ui->lineEdit_Numbers->setEnabled(false);
}

void MainWindow::setSafeLimit(){
  ui->lineEdit_SafeLimit->setEnabled(true);
  ui->lineEdit_SafeLimit->setText(QString::number(configureDialog_->safeLimit_));
  safety_->stop();
  safety_->setTempChangeThreshold(configureDialog_->safeLimit_);
  ui->lineEdit_SafeLimit->setEnabled(false);
}

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

void MainWindow::setIgnoreEnable(){
  ui->checkBox_Ignore->setEnabled(true);
  ui->checkBox_Ignore->setChecked(configureDialog_->ignoreEnable_);
  safety_->stop();
  safety_->setEnableTempChangeRange(configureDialog_->ignoreEnable_);
  ui->checkBox_Ignore->setEnabled(false);
}

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

void MainWindow::Run(){
  statusBar()->clearMessage();
  LogMsg("Set Run.");
  com_->executeRun();
  bkgColorChangeable_ = true;
  setColor(1, bkgColorChangeable_);
  ui->lineEdit_msg->setStyleSheet("");
  ui->pushButton_Log->setChecked(true);
  ui->checkBoxStatusRun->setChecked(true);
  countTempCheck_ = 0;
  //sendLINE("Running starts.");
  plotTimer_->start();
  data_->generateSaveFile();
  data_->SetIntervalLog(ui->spinBox_TempRecordTime->value());
  data_->logingStart();
  safety_->setIntervalMVCheck(ui->lineEdit_IntervalAskMV->text().toInt());
  safety_->setIntervalTempChange(ui->lineEdit_IntervalAskTemp->text().toInt());
  safety_->start();
}

void MainWindow::Stop(){
  countTempCheck_ = 0;
  statusBar()->clearMessage();
  com_->executeStop();
  LogMsg("Set Stop.");
  bkgColorChangeable_ = true;
  setColor(0, bkgColorChangeable_);
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
  sendLINE("Running stop.");
}

void MainWindow::Quit(){
  com_->executeStop();
  ui->textEdit_Log->setTextColor(QColor(255,0,0,255));
  LogMsg("Emergency Stop. Check the experimental condition.");
  vtemp_.clear();
  countTempCheck_ = 0;
  ui->textEdit_Log->setTextColor(QColor(0,0,0,255));
  ui->checkBoxStatusRun->setChecked(false);
  ui->checkBoxStatusSTC->setChecked(false);
  ui->pushButton_RunStop->setChecked(false);
  safety_->stop();
  //sendLINE("Emergency Stop!");
  bkgColorChangeable_ = true;
  setColor(3, bkgColorChangeable_);
  bkgColorChangeable_ = false;
  data_->logingStart();
  plotTimer_->start();
}


void MainWindow::setTextTempDrop(bool enable){
  if(enable) ui->checkBox_TempDropEnable->setText(tr("Yes"));
  else ui->checkBox_TempDropEnable->setText(tr("No"));
}

void MainWindow::makePlot(){
  const double setTemperature = ui->lineEdit_SV->text().toDouble();
  QDateTime date = QDateTime::currentDateTime();
  valltemp_.push_back(com_->getTemperature());
  fillDataAndPlot(date, com_->getTemperature(), setTemperature, com_->getMV());
}

void MainWindow::on_checkBox_dataSave_toggled(bool checked)
{
  if(!checked) return;
  data_->generateSaveFile();
}
void MainWindow::setColor(int colorindex, bool enable){
  if (!enable) return;
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

void MainWindow::sendLINE(const QString& message){notify_->setLINE(message, LINEToken_);}

void MainWindow::addPortName(QList<QSerialPortInfo> info){
    LogMsg("-------------- COM Ports found :");
    for (const QSerialPortInfo &portinfo : info) {
        LogMsg(portinfo.portName() + ", " + portinfo.serialNumber() + ", " + portinfo.manufacturer());
        ui->comboBox_SeriesNumber->addItem( portinfo.serialNumber(), (QString) portinfo.portName());
    }
    LogMsg ("--------------");
}


//signals
void MainWindow::updateTemperature(double temperature){
  ui->lineEdit_Temp->setText(QString::number(temperature) + " C");
}

void MainWindow::updateMV(double MV){
  ui->lineEdit_CurrentMV->setText(QString::number(MV) + " %");
}

void MainWindow::updateSV(double SV){
  ui->lineEdit_CurrentSV->setText(QString::number(SV) + " C");
}

void MainWindow::updateMVupper(double MVupper){
  ui->doubleSpinBox_MVupper->setValue(MVupper);
  plot->yAxis2->setRangeUpper(MVupper + 2);
  plot->replot();
}

void MainWindow::updateMVlower(double MVlower){
  ui->doubleSpinBox_MVlower->setValue(MVlower);
}

void MainWindow::updatePID_P(double PID_P){
  ui->lineEdit_P->setText(QString::number(PID_P));
}

void MainWindow::updatePID_I(double PID_I){
  ui->lineEdit_I->setText(QString::number(PID_I));
}

void MainWindow::updatePID_D(double PID_D){
  ui->lineEdit_D->setText(QString::number(PID_D));
}

void MainWindow::updateCheckNumber(int checkNumber){
  if (checkNumber == 0) return;
  ui->textEdit_Log->setTextColor(QColor(255, 0, 0, 255));
  LogMsg("Checking temperature change in " + QString::number(checkNumber));
  ui->textEdit_Log->setTextColor(QColor(0, 0, 0, 255));
  ui->lineEdit_msg->setText("Checking temperature change in " + QString::number(checkNumber));
  setColor(2);
}

void MainWindow::updateStatus(){
  QDateTime date = QDateTime::currentDateTime();
  QString datestr = date.toString("HH:mm:ss").toStdString().c_str();
  ui->lineEdit_msg->setEnabled(true);
  ui->lineEdit_msg->setText("Data is updated @ " + datestr);
  ui->lineEdit_msg->setEnabled(false);
}

void MainWindow::catchLogMsg(const QString& msg){LogMsg(msg);}

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
  ui->lineEdit_SV->setText(QString::number(com_->getSV()));
  LogMsg("Set Stop.");
  QColor color = QColor("palegray");
  QPalette pal = palette();
  pal.setColor(QPalette::Window, color);
}

void MainWindow::connectFailed(){
    ui->textEdit_Log->setTextColor(QColor(255,0,0,255));
    LogMsg("The Omron temperature control cannot be connected on any COM port.");
    ui->textEdit_Log->setTextColor(QColor(0,0,0,255));
    ui->comboBox_SeriesNumber->setEnabled(true);
    ui->pushButton_Connect->setStyleSheet("");
}

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

void MainWindow::finishSendSV(double SV){
   LogMsg("Target temperature is set to be " + QString::number(SV));
}

void MainWindow::catchDanger(int type){
  ui->textEdit_Log->setTextColor(QColor(255,0,0,255));
  switch (type){
    case 0 :
      LogMsg("The maximum allowed temperature has been exceeded.");
      break;
    case 1 :
      LogMsg("Even though the MV output is maximum, the temperature change is less than the threshold");
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

void MainWindow::saveFile(bool sucess){
  if (sucess) LogMsg("Sucess to write data log.");
  else {
      ui->textEdit_Log->setTextColor(QColor(0, 0, 255, 255));
      LogMsg("Failed to srite data log.");
      ui->textEdit_Log->setTextColor(QColor(0, 0, 0, 255));
    }
}

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
}

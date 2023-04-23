//!
//! \file mainwindow.cpp
//! \brief mainwindow for connection to the E5CC
//! \author Daisuke Miura (Original source code xxxxx)
//! \date 2023/3/15
//! \details The original source code did not include safety measures.
//! To improve safety, functions were added to check if the temperature was rising and to detect unexpected sudden temperature drops.
//! The GUI was also modified to set the necessary parameters for these functions.
//!
#include "mainwindow.h"
#include "ui_mainwindow.h"

const QString DESKTOP_PATH = QStandardPaths::locate(QStandardPaths::DesktopLocation, QString(), QStandardPaths::LocateDirectory);
const QString DATA_PATH_2 = DESKTOP_PATH + "Temp_Record";
const QString DATA_PATH = "Z:/triplet/Temp_Record";
//const QString DATA_PATH = "/c/Users/daisuke/OmronPID";

/** enum E5CC_Address */
enum E5CC_Address{
    // QByteArray::fromHex(QString::number(E5CC_Address::setPoint, 16).toStdString().c_str()
    PV = 0x0000, /** Get Temperature */
    MV= 0x0008, /** Get output power */
    SV = 0x0106,/** Get set value */
    MVupper = 0x0A0A, /** Get putput upper */
    MVlower = 0x0A0C, /** Get output lower */

    PID_P=0x0A00, /** Get Proportional */
    PID_I=0x0A02, /** Get Integration */
    PID_D=0x0A04 /** Get Difference */
};

enum timing{
    modbus = 100,
    getTempTimer = 500,
    clockUpdate = 50,
    timeUp = 1000*60*10,
    timeOut = 700
};

//TODO nicer time display on LogMsg

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    msgCount = 0;
    tempControlOnOff = false;
    tempRecordOnOff = false;
    modbusReady = true;
    spinBoxEnable = false;
    muteLog = false;
    tempDecimal = 0.1; // for 0.1
    //tempDecimal = 1.0;

    //======= clock
    clock = new QTimer(this);
    clock->stop();
    connect(clock, SIGNAL(timeout()), this, SLOT(showTime()));
    totalElapse.start();
    dayCounter = 0;
    checkDay = false;
    waitTimer = new QTimer(this);
    waitTimer->stop();
    waitTimer->setSingleShot(false);
    connect(waitTimer, SIGNAL(timeout()), this, SLOT(allowSetNextSV()));

    threadTimer_= new QTimer(this);
    threadTimer_ -> stop();
    threadTimerInterval_ = 300*1000; //msec
    connect(threadTimer_, SIGNAL(timeout()), this, SLOT(checkThreads()));

    //! helpDialog
    helpDialog = new QDialog(this);
    HelpLabel = new QLabel();
    helpDialog->setWindowTitle("Help");
    picNumber = 1;
    QPixmap image(":fig1.PNG");
    HelpLabel->setPixmap(image);
    QPushButton * next = new QPushButton("Next");
    connect(next, SIGNAL(pressed()) , this, SLOT(HelpPicNext()));
    QVBoxLayout *mainLayout = new QVBoxLayout(helpDialog);
    mainLayout->addWidget(HelpLabel);
    mainLayout->addWidget(next);

    //! plotDialog
    plotDialog_ = new PlotDialog(this);
    plotDialog_->setWindowTitle("Setting for Plot");

    /*
    //! configure for TempDrop
    tempDropDialog_ = new TempDropDialog(this);
    tempDropDialog_->setWindowTitle("Setting for TempDrop mode");
    */

  //Check Temp Directory, is not exist, create
    QDir myDir;
    myDir.setPath(DATA_PATH);
    filePath_ = DATA_PATH;
    // if Z:/triplet/Temp_Record does not exist, make directory on desktop.
    if( !myDir.exists()) {
        QMessageBox msgBox;
        msgBox.setText("Cannot locate Z Drive!\n"
                       "Please set the NAS to be Z:\n"
                       "Data will be saved in Desktop/Temp_Record");
        myDir.mkpath(DATA_PATH_2);
        filePath_ = DATA_PATH_2;
    }else{
        LogMsg("Data will be saved in : " + DATA_PATH );
    }


    plot = ui->plot;
    plot->xAxis->setLabel("Time");
    plot->yAxis->setLabel("Temp. [C]");
    plot->addGraph(plot->xAxis, plot->yAxis2);
    plot->graph(0)->setName("Output");
    plot->graph(0)->setPen(QPen(Qt::darkGreen)); // MV
    //plot->graph(0)->setScatterStyle(QCPScatterStyle::ssDisc);
    plot->addGraph();
    plot->graph(1)->setName("Temp.");
    plot->graph(1)->setPen(QPen(Qt::blue)); // PV
    //plot->graph(1)->setScatterStyle(QCPScatterStyle::ssDisc);
    plot->addGraph();
    plot->graph(2)->setName("Set-temp.");
    plot->graph(2)->setPen(QPen(Qt::red)); // SV
    //plot->graph(2)->setScatterStyle(QCPScatterStyle::ssDisc);
    QSharedPointer<QCPAxisTickerDateTime> dateTicker(new QCPAxisTickerDateTime);
    double now = QDateTime::currentDateTime().toSecsSinceEpoch();
    dateTicker->setDateTimeFormat("MM/dd HH:mm:ss");
    plot->xAxis->setTicker(dateTicker);
    plot->xAxis2->setVisible(true);
    plot->yAxis2->setVisible(true);
    plot->yAxis2->setLabel(" Output [%]");
    plot->yAxis2->setRangeLower(0.0);
    plot->xAxis2->setTicks(false);
    plot->yAxis2->setTicks(true);
    plot->xAxis2->setTickLabels(false);
    plot->yAxis2->setTickLabels(true);
    plot->xAxis->setRange(now, now + 1*3600);
    plot->setInteraction(QCP::iRangeZoom,true);
    plot->setInteraction(QCP::iRangeDrag,true);
    plot->axisRect()->setRangeDrag(Qt::Vertical);
    plot->axisRect()->setRangeZoom(Qt::Vertical);
    plot->legend->setVisible(true);
    QCPLayoutGrid *subLayout = new QCPLayoutGrid;
    plot->plotLayout()->addElement(1, 0, subLayout);
    subLayout->setMargins(QMargins(5,0,5,5));
    subLayout->addElement(0, 0, plot->legend);
    plot->legend->setFillOrder(QCPLegend::foColumnsFirst);
    plot->plotLayout()->setRowStretchFactor(1, 0.001);
    plot->axisRect()->setAutoMargins(QCP::msLeft | QCP::msTop | QCP::msBottom);
    plot->axisRect()->setMargins(QMargins(0,0,100,0));
    plot->replot();

    /*
    ui->comboBox_Func->addItem("0x00 Invalid", QModbusPdu::Invalid);
    ui->comboBox_Func->addItem("0x01 Read Coils", QModbusPdu::ReadCoils);
    ui->comboBox_Func->addItem("0x02 Read Discrte Inputs", QModbusPdu::ReadDiscreteInputs);
    ui->comboBox_Func->addItem("0x03 Read Holding Registers", QModbusPdu::ReadHoldingRegisters);
    ui->comboBox_Func->addItem("0x04 Read Input Registers", QModbusPdu::ReadInputRegisters);
    ui->comboBox_Func->addItem("0x05 Write Single Coil", QModbusPdu::WriteSingleCoil);
    ui->comboBox_Func->addItem("0x06 Write Single Register", QModbusPdu::WriteSingleRegister);
    ui->comboBox_Func->addItem("0x07 Read Exception Status", QModbusPdu::ReadExceptionStatus);
    ui->comboBox_Func->addItem("0x08 Diagonstics", QModbusPdu::Diagnostics);
    ui->comboBox_Func->addItem("0x0B Get Comm Event Counter", QModbusPdu::GetCommEventCounter);
    ui->comboBox_Func->addItem("0x0C Get Comm Event Log", QModbusPdu::GetCommEventLog);
    ui->comboBox_Func->addItem("0x0f Write Multiple Coils", QModbusPdu::WriteMultipleCoils);
    ui->comboBox_Func->addItem("0x10 Write Multiple Registers", QModbusPdu::WriteMultipleRegisters);
    ui->comboBox_Func->addItem("0x11 Report Server ID", QModbusPdu::ReportServerId);
    ui->comboBox_Func->addItem("0x14 Read File Record", QModbusPdu::ReadFileRecord);
    ui->comboBox_Func->addItem("0x15 Write File Record", QModbusPdu::WriteFileRecord);
    ui->comboBox_Func->addItem("0x16 Mask Write Register", QModbusPdu::MaskWriteRegister);
    ui->comboBox_Func->addItem("0x17 Read Write Multiple Registers", QModbusPdu::ReadWriteMultipleRegisters);
    ui->comboBox_Func->addItem("0x18 Read Fifo Queue", QModbusPdu::ReadFifoQueue);
    ui->comboBox_Func->addItem("0x2B Encapsulated Interface Transport", QModbusPdu::EncapsulatedInterfaceTransport);
    ui->comboBox_Func->addItem("0x100 Undefined Function Code", QModbusPdu::UndefinedFunctionCode);
    */

    comboxEnable = false;
    ui->comboBox_AT->addItem("AT cancel");
    ui->comboBox_AT->addItem("100% AT execute");
    ui->comboBox_AT->addItem("40% AT execute");
    ui->comboBox_AT->setCurrentIndex(0);

    ui->comboBox_Mode->addItem("Stable", 1);
    ui->comboBox_Mode->addItem("Fixed time", 2);
    ui->comboBox_Mode->addItem("Fixed rate", 3);
    ui->comboBox_Mode->addItem("Normal + Fixed rate", 4);

    ui->comboBox_Mode->setItemData(0, QBrush(Qt::black), Qt::ForegroundRole);
    ui->comboBox_Mode->setItemData(1, QBrush(Qt::red), Qt::ForegroundRole);
    ui->comboBox_Mode->setItemData(2, QBrush(Qt::blue), Qt::ForegroundRole);
    ui->comboBox_Mode->setItemData(3, QBrush(Qt::darkGreen), Qt::ForegroundRole);


    findSeriesPortDevices();
    omron = NULL;

    panalOnOff(false);
    ui->pushButton_Control->setEnabled(false);
    ui-> pushButton_RunStop->setEnabled(false);
    //ui->pushButton_RecordTemp->setEnabled(false);

    //============= some useful addresses
    ui->comboBox_MemAddress->addItem("0x0000 (opt) PV "                     , 0x0000);
    ui->comboBox_MemAddress->addItem("0x0008 (opt) MV heating "             , 0x0008);
    ui->comboBox_MemAddress->addItem("0x000A (opt) MV cooling "            , 0x000A);
    ui->comboBox_MemAddress->addItem("0x0106 (opt) SP "                     , 0x0106);
    ui->comboBox_MemAddress->addItem("0x0108 (opt) Alarm 1 type "           , 0x0108);
    ui->comboBox_MemAddress->addItem("0x010A (opt) Alarm 1 UL "             , 0x010A);
    ui->comboBox_MemAddress->addItem("0x010C (opt) Alarm 1 LL "             , 0x010C);
    ui->comboBox_MemAddress->addItem("0x010E (opt) Alarm 2 type "           , 0x010E);
    ui->comboBox_MemAddress->addItem("0x0110 (opt) Alarm 2 UL "             , 0x0110);
    ui->comboBox_MemAddress->addItem("0x0112 (opt) Alarm 2 LL "             , 0x0112);
    ui->comboBox_MemAddress->addItem("0x0608 (opt) heater current 1 "       , 0x0608);
    ui->comboBox_MemAddress->addItem("0x060A (opt) MV heating "             , 0x060A);
    ui->comboBox_MemAddress->addItem("0x060C (opt) MV cooling "             , 0x060C);
    ui->comboBox_MemAddress->addItem("0x0702 (opt) Prop. band "             , 0x0702);
    ui->comboBox_MemAddress->addItem("0x0704 (opt) Inte. time "             , 0x0704);
    ui->comboBox_MemAddress->addItem("0x0706 (opt) deri. time "             , 0x0706);

    ui->comboBox_MemAddress->addItem("0x071E (adj) MV at stop "             , 0x071E);
    ui->comboBox_MemAddress->addItem("0x0722 (adj) MV at PV Error "         , 0x0722);
    //! modified "0x0A0A -> ""0x0A00" in part of the  first argument in addItem function @ 2023/2/27 by Daisuke Miura.
    //! This is a display issue on the GUI and has nothing to do with functionality.
    ui->comboBox_MemAddress->addItem("0x0A00 (adj) Prop. band "             , 0x0A00);
    ui->comboBox_MemAddress->addItem("0x0A02 (adj) Inte. time "             , 0x0A02);
    ui->comboBox_MemAddress->addItem("0x0A04 (adj) deri. time "             , 0x0A04);
    ui->comboBox_MemAddress->addItem("0x0A0A (adj) MV upper limit "         , 0x0A0A);
    ui->comboBox_MemAddress->addItem("0x0A0C (adj) MV lower limit "         , 0x0A0C);

    ui->comboBox_MemAddress->addItem("0x0710 (ini) Ctrl. period heating "   , 0x0710);
    ui->comboBox_MemAddress->addItem("0x0712 (ini) Ctrl. period cooling "   , 0x0712);
    ui->comboBox_MemAddress->addItem("0x0D06 (ini) Ctrl. output 1 current " , 0x0D06);
    ui->comboBox_MemAddress->addItem("0x0D08 (ini) Ctrl. output 2 current " , 0x0D08);
    ui->comboBox_MemAddress->addItem("0x0D1E (ini) SP upper limit "         , 0x0D1E);
    ui->comboBox_MemAddress->addItem("0x0D20 (ini) SP lower limit "         , 0x0D20);
    ui->comboBox_MemAddress->addItem("0x0D22 (ini) Std heating/cooling "    , 0x0D22);
    ui->comboBox_MemAddress->addItem("0x0D24 (ini) Direct/Reverse opt. "    , 0x0D24);
    ui->comboBox_MemAddress->addItem("0x0D28 (ini) PID on/off "             , 0x0D28);

    ui->comboBox_MemAddress->addItem("0x0500 (protect) Opt/Adj protect "       , 0x0500);
    ui->comboBox_MemAddress->addItem("0x0502 (protect) Init/Comm protect "     , 0x0502);
    ui->comboBox_MemAddress->addItem("0x0504 (protect) Setting Chg. protect "  , 0x0504);
    ui->comboBox_MemAddress->addItem("0x0506 (protect) PF key protect "        , 0x0506);

    ui->comboBox_MemAddress->addItem("0x0E0C (adv) Ctrl. output 1 Assignment "   , 0x0E0C);
    ui->comboBox_MemAddress->addItem("0x0E0E (adv) Ctrl. output 2 Assignment "   , 0x0E0E);
    ui->comboBox_MemAddress->addItem("0x0E20 (adv) Aux. output 1 Assignment "    , 0x0E20);
    ui->comboBox_MemAddress->addItem("0x0E22 (adv) Aux. output 2 Assignment "    , 0x0E22);
    ui->comboBox_MemAddress->addItem("0x0E24 (adv) Aux. output 3 Assignment "    , 0x0E24);

    setColor(0);
    comboxEnable = true;

    configureDialog_ = new ConfigureDialog(this);
    configureDialog_->setWindowTitle("Configure");
    connect(configureDialog_->pushButton_SetParameters_, SIGNAL(clicked(bool)), this, SLOT(setParametersTempCheck()) );

    setIntervalAskMV();
    setIntervalAskTemp();
    setNumbers();
    setSafeLimit();
    setIgnoreRange();

    //! Thread for TempCheck mode.
    threadTempCheck_ = new MyThread();
    threadTempCheck_->interval_ = ui->lineEdit_IntervalAskTemp->text().toInt()*1000; //ms to sec.
    threadTempCheck_->moveToThread(threadTempCheck_);
    QObject::connect(threadTempCheck_, SIGNAL(data_update()), this, SLOT(TempCheck()));

    //! Thread for TempCheck mode. Priotity is set to  QThread::TimeCriticalPriority
    threadMVcheck_ = new MyThread();
    threadMVcheck_->interval_ = ui->lineEdit_IntervalAskMV->text().toInt()*1000; //ms to sec.
    threadMVcheck_->moveToThread(threadMVcheck_);
    QObject::connect(threadMVcheck_, SIGNAL(data_update()), this, SLOT(periodicWork()));

    //! Thread for Logging. Priotity is set to QThread::HighestPriority
    threadLog_ = new MyThread();
    threadLog_->interval_ = ui->spinBox_TempRecordTime->value() * 1000; //ms to sec.;
    threadLog_->moveToThread(threadLog_);
    QObject::connect(threadLog_, SIGNAL(data_update()), this, SLOT(makePlot()));

    //! TempCheck counter is set to be 0.
    countTempCheck_ = 0;
    countDropCheck_ = 0;
    connect(ui->checkBox_TempDropEnable, SIGNAL(clicked(bool)), this, SLOT(setTextTempDrop(bool)));

    ui->textEdit_Log->setTextColor(QColor(34,139,34,255));
    LogMsg("The AT and RUN/STOP do not get from the device. Please be careful.");
    ui->textEdit_Log->setTextColor(QColor(0,0,0,255));

    //! LineNotify
    connect(omron, &QModbusRtuSerialMaster::stateChanged, [=](QModbusDevice::State state) {
        sendLineNotifyConnection(omron);
    });


    dateStart_ = QDateTime::currentDateTime();
    LogMsgBox_ = new QMessageBox;

    pvData.clear();
    mvData.clear();
    svData.clear();
    //! Reserved memory size
    pvData.reserve(vecSize_);
    svData.reserve(vecSize_);
    mvData.reserve(vecSize_);
    vdifftemp_.reserve(vecSize_);
    vtemp_.reserve(10);
    vtemp_.push_back(temperature);

    statusAskMV_ = false;
    statusAskTemp_ = false;
    statusAskSetPoint_ = false;

    ui->lineEdit_DirPath->setText(filePath_);
}
MainWindow::~MainWindow()
{
    if (omron) omron->disconnectDevice();

    clock->stop();
    waitTimer->stop();
    threadMVcheck_->quit();
    threadLog_->quit();
    threadTempCheck_->quit();

    delete waitTimer;
    delete clock;
    delete plot;
    delete omron;
    delete ui;
}


void MainWindow::LogMsg(QString str, bool newLine)
{
    if( muteLog ) return;
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

void MainWindow::findSeriesPortDevices()
{
    LogMsg("-------------- COM Ports found :");
    const auto infos = QSerialPortInfo::availablePorts();
    for (const QSerialPortInfo &info : infos) {
        LogMsg(info.portName() + ", " + info.serialNumber() + ", " + info.manufacturer());
        ui->comboBox_SeriesNumber->addItem( info.serialNumber(), (QString) info.portName());
    }
    LogMsg ("--------------");
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

QString MainWindow::formatHex(int value, int digit)
{
    QString valueStr = QString::number(value, 16).toUpper();
    while(valueStr.size() < digit){
        valueStr.insert(0, "0");
    }
    return valueStr;
}

void MainWindow::waitForMSec(int msec)
{
    //wait for waitTime
    QEventLoop eventLoop;
    QTimer::singleShot(msec, &eventLoop, SLOT(quit()));
    eventLoop.exec();
}

void MainWindow::panalOnOff(bool IO)
{
    ui->lineEdit_SV->setEnabled(IO);
    ui->comboBox_AT->setEnabled(IO);
    ui->pushButton_AskStatus->setEnabled(IO);
    ui->pushButton_GetPID->setEnabled(IO);
    ui->pushButton_SetSV->setEnabled(IO);
    ui->pushButton_RunStop->setEnabled(IO);
    ui->spinBox_TempRecordTime->setEnabled(IO);
    ui->spinBox_TempStableTime->setEnabled(IO);
    ui->spinBox_DeviceAddress->setEnabled(IO);
    ui->doubleSpinBox_TempTorr->setEnabled(IO);
    ui->doubleSpinBox_TempStepSize->setEnabled(IO);
    ui->doubleSpinBox_MVlower->setEnabled(IO);
    ui->doubleSpinBox_MVupper->setEnabled(IO);
    ui->comboBox_Mode->setEnabled(IO);
    //ui->checkBox_MuteLogMsg->setEnabled(IO);
    ui->comboBox_MemAddress->setEnabled(IO);
    ui->lineEdit_SV2->setEnabled(IO);
    ui->doubleSpinBox_SV2WaitTime->setEnabled(IO);
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
    //ui->lineEdit_clock->setText(QString::number(dayCounter) + "/" + t.toString("HH:mm:ss:zzz"));
    //qDebug() << "==========" << t.msec();
}

void MainWindow::allowSetNextSV(){
    nextSV = true;
}

void MainWindow::on_pushButton_AskStatus_clicked(){
  askTemperature();
  askSetPoint();
  askMV();
}

void MainWindow::read(QModbusDataUnit::RegisterType type, quint16 adress, int size)
{
    // accesing Hodling register, address 0x0000, for 2 Byte of value.
    statusBar()->clearMessage();
    QModbusDataUnit ans(type, adress, size);
    respondType = adress;
    modbusReady = false;

    if (auto *reply = omron->sendReadRequest(ans, omronID)) {
        if (!reply->isFinished()){
            connect(reply, &QModbusReply::finished, this, &MainWindow::readReady);
        }else{
            delete reply; // broadcast replies return immediately
        }
    } else {
        statusBar()->showMessage(tr("Read error: ") + omron->errorString(), 0);
    }
}

void MainWindow::readReady()
{
    //LogMsg("------ reading, " + QString::number(respondType));
    auto reply = qobject_cast<QModbusReply *>(sender());

    if (!reply)
        return;

    if (reply->error() == QModbusDevice::NoError) {
        const QModbusDataUnit unit = reply->result();
        if(respondType == E5CC_Address::PV){
            temperature = QString::number(unit.value(1), 10).toDouble() * tempDecimal;
            QString str = tr("Current Temperature : %1 C").arg(QString::number(temperature));
            ui->lineEdit_Temp->setText(QString::number(temperature) + " C");
            LogMsg(str);
        }else if(respondType == E5CC_Address::SV){
            SV = QString::number(unit.value(1), 10).toDouble() * tempDecimal;
            QString str = tr("Current Set Point : %1 C").arg(QString::number(SV));
            ui->lineEdit_CurrentSV->setText(QString::number(SV) + " C");
            LogMsg(str);
        }else if(respondType == E5CC_Address::MV){
            MV = QString::number(unit.value(1), 10).toDouble() * tempDecimal;
            QString str = tr("Current MV : %1 \%").arg(QString::number(MV));
            ui->lineEdit_CurrentMV->setText(QString::number(MV) + " %");
            LogMsg(str);
        }else if(respondType == E5CC_Address::MVupper){
            MVupper = QString::number(unit.value(1), 10).toDouble() * tempDecimal;
            QString str = tr("MV upper limit : %1 \%").arg(QString::number(MVupper));
            ui->doubleSpinBox_MVupper->setValue(MVupper);
            LogMsg(str);
            plot->yAxis2->setRangeUpper(MVupper + 2);
            plot->replot();
        }else if(respondType == E5CC_Address::MVlower){
            MVlower = QString::number(unit.value(1), 10).toDouble() * tempDecimal;
            QString str = tr("MV lower limit : %1 \%").arg(QString::number(MVlower));
            ui->doubleSpinBox_MVlower->setValue(MVlower);
            LogMsg(str);
        }else if(respondType == E5CC_Address::PID_P){
            pid_P = QString::number(unit.value(1), 10).toDouble() * 0.1;
            QString str = tr("P       : %1 ").arg(QString::number(pid_P));
            ui->lineEdit_P->setText(QString::number(pid_P));
            LogMsg(str);
        }else if(respondType == E5CC_Address::PID_I){
            pid_I = QString::number(unit.value(1), 10).toDouble();
            QString str = tr("I (raw) : %1 sec").arg(QString::number(pid_I));
            ui->lineEdit_I->setText(QString::number(pid_I));
            LogMsg(str);
        }else if(respondType == E5CC_Address::PID_D){
            pid_D = QString::number(unit.value(1), 10).toDouble();
            QString str = tr("D (raw) : %1 sec").arg(QString::number(pid_D));
            ui->lineEdit_D->setText(QString::number(pid_D));
            LogMsg(str);
        }else{
            LogMsg("respond count: " + QString::number(unit.valueCount()));
            for (uint i = 0; i < unit.valueCount(); i++) {
                const QString entry = tr("Address: %1, Value: %2").arg(unit.startAddress()).arg(QString::number(unit.value(i), 10));
                LogMsg(entry);
            }
        }

    } else if (reply->error() == QModbusDevice::ProtocolError) {
        statusBar()->showMessage(tr("Read response error: %1 (Mobus exception: 0x%2)").
                                    arg(reply->errorString()).
                                    arg(reply->rawResult().exceptionCode(), -1, 16), 0);
    } else {
        statusBar()->showMessage(tr("Read response error: %1 (code: 0x%2)").
                                    arg(reply->errorString()).
                                    arg(reply->error(), -1, 16), 0);
    }

    reply->deleteLater();
    modbusReady = true;
}

void MainWindow::askTemperature(bool mute){
  statusAskTemp_ = true;
  if(!mute) LogMsg("------ get Temperature.");
  read(QModbusDataUnit::HoldingRegisters, E5CC_Address::PV, 2);
  waitTimming();
  if (temperature >= ui->spinBox_TempUpper->value()) {
      Quit();
      ui->lineEdit_TempCheckCount->setText("Exceeded the upper safety limit temperature.");
      return;
    }
  waitForMSec(200);
  statusAskTemp_ = false;
}

void MainWindow::askSetPoint(bool mute){
  statusAskSetPoint_ = true;
  if(!mute) LogMsg("------ get Set Temperature.");
  read(QModbusDataUnit::HoldingRegisters, E5CC_Address::SV, 2);
  waitTimming();
  waitForMSec(200);
  statusAskSetPoint_ = false;
}

void MainWindow::askMV(bool mute){
  //if (statusAskMV_) waitForMSec(100);
  statusAskMV_ = true;
  if (!mute) LogMsg("------ get Output power.");
  read(QModbusDataUnit::HoldingRegisters, E5CC_Address::MV, 2);
  waitTimming();
  waitForMSec(200);
  statusAskMV_ = false;
}

void MainWindow::askMVupper(bool mute){
  if(!mute)LogMsg("------ get MV upper.");
  read(QModbusDataUnit::HoldingRegisters, E5CC_Address::MVupper, 2);
  waitTimming();

}

void MainWindow::askMVlower(bool mute){
  if(!mute)LogMsg("------ get MV lower.");
  read(QModbusDataUnit::HoldingRegisters, E5CC_Address::MVlower, 2);
  waitTimming();
}

//!
//! \brief MainWindow::waitTimming
//! \details Function to avoid reading wrong values by Asking at the same time
void MainWindow::waitTimming(){
  int i = 0;
  while(!modbusReady){
    i++;
    waitForMSec(timing::modbus);
    if( i > 10 ) modbusReady = true;
  }
}

void MainWindow::getSetting()
{
    askTemperature();
    askSetPoint();
    askMV();

    //ask MV upper Limit
    spinBoxEnable = false;
    askMVupper();
    askMVlower();
    spinBoxEnable = true;

    //get PID constant
    LogMsg("------ get Propertion band.");
    read(QModbusDataUnit::HoldingRegisters, E5CC_Address::PID_P, 2);
    waitTimming();
    LogMsg("------ get integration time.");
    read(QModbusDataUnit::HoldingRegisters, E5CC_Address::PID_I, 2);
    waitTimming();

    LogMsg("------ get derivative time.");
    read(QModbusDataUnit::HoldingRegisters, E5CC_Address::PID_D, 2);
    waitTimming();
}

void MainWindow::setAT(int atFlag)
{
    statusBar()->clearMessage();
    QString cmd;
    if( atFlag == 0){
        cmd = "00 00 03 00";
        ui->lineEdit_SV->setEnabled(true);
        ui->pushButton_SetSV->setEnabled(true);
        LogMsg("Set AT to none.");
    }else if( atFlag == 1){
        cmd = "00 00 03 01";
        ui->lineEdit_SV->setEnabled(false);
        ui->pushButton_SetSV->setEnabled(false);
        LogMsg("Set AT to 100%., disable Set Point.");
    }else if( atFlag == 2){
        cmd = "00 00 03 02";
        ui->lineEdit_SV->setEnabled(false);
        ui->pushButton_SetSV->setEnabled(false);
        LogMsg("Set AT to 40%. disable Set Point.");
    }
    QByteArray value = QByteArray::fromHex(cmd.toStdString().c_str());
    request(QModbusPdu::WriteSingleRegister, value);
}

void MainWindow::setSV(double SV)
{
    statusBar()->clearMessage();
    int sv_input = (qint16) (SV / tempDecimal + 0.5);
    QString valueStr = formatHex(sv_input, 8);
    QString addressStr = formatHex(E5CC_Address::SV, 4);

    QString cmd = addressStr + " 00 02 04" + valueStr;
    QByteArray value = QByteArray::fromHex(cmd.toStdString().c_str());
    request(QModbusPdu::WriteMultipleRegisters, value);
    LogMsg("Target temperature is set to be " + QString::number(SV));
}

void MainWindow::request(QModbusPdu::FunctionCode code, QByteArray cmd)
{
    statusBar()->clearMessage();
    modbusReady = false;
    QModbusPdu pdu;
    pdu.setFunctionCode(code);
    pdu.setData(cmd);
    qDebug() << pdu;

    QModbusRequest ask(pdu);

    if (auto *reply = omron->sendRawRequest(ask, omronID)) {
        if (!reply->isFinished()) {
            connect(reply, &QModbusReply::finished, this, [this, reply]() {
                if (reply->error() == QModbusDevice::ProtocolError) {
                    statusBar()->showMessage(tr("Write response error: %1 (Mobus exception: 0x%2)")
                        .arg(reply->errorString()).arg(reply->rawResult().exceptionCode(), -1, 16), 0);
                } else if (reply->error() != QModbusDevice::NoError) {
                    statusBar()->showMessage(tr("Write response error: %1 (code: 0x%2)").
                        arg(reply->errorString()).arg(reply->error(), -1, 16), 0);
                }
                reply->deleteLater();
                modbusReady = true;
            });
        } else {
            // broadcast replies return immediately
            reply->deleteLater();
        }

    } else {
        statusBar()->showMessage(tr("Write error: ") + omron->errorString(), 0);
    }

}

void MainWindow::on_pushButton_SetSV_clicked()
{
  setSV( ui->lineEdit_SV->text().toDouble());
  LogMsg("Set Temperature is changed to " + ui->lineEdit_SV->text() + " C.");
}

void MainWindow::on_pushButton_Control_clicked()
{
    tempControlOnOff = !tempControlOnOff;
    panalOnOff(!tempControlOnOff);
    ui->actionOpen_File->setEnabled(!tempControlOnOff);
    if(tempControlOnOff) {
        LogMsg("================ Temperature control =====");
        ui->pushButton_Control->setStyleSheet("background-color: rgb(50,137,48)");
        ui->lineEdit_TempCheckCount->setText("Slow Temperature controle mode");
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
        ui->lineEdit_TempCheckCount->clear();
        return;
    }

    double iniTemp = 0;
    if(tempControlOnOff){
        on_pushButton_AskStatus_clicked();
        iniTemp = temperature;
        LogMsg("Current Temperature         : " + QString::number(temperature) + " C.");

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
        double estTotalTime = estSlope * qAbs(temperature-targetValue);
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
        clock->start(timing::clockUpdate);
        totalElapse.start(); // used for clock
        QTimer getTempTimer;
        getTempTimer.setSingleShot(true);

        //########################### mode 4 extra code, go to targetValue_2
        //----- set SV
        setSV(targetValue_2);

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
            askTemperature();
            askMV();

            QDateTime date = QDateTime::currentDateTime();
            fillDataAndPlot(date, temperature, targetValue_2, MV);
            writeData();
            while(getTempTimer.remainingTime() > 0 ){
                waitForMSec(timing::getTempTimer);
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
        askTemperature();
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

            setSV(smallShift);
            do{
                getTempTimer.start(tempGetTime);
                qDebug()  << "temp control. do-loop 1 = " << tempControlOnOff;
                if(!tempControlOnOff) break;

                askTemperature();
                askMV();

                QDateTime date = QDateTime::currentDateTime();
                fillDataAndPlot(date, temperature, smallShift, MV);
                writeData();
                while(getTempTimer.remainingTime() > 0 ){
                    waitForMSec(timing::getTempTimer);
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
            askTemperature();
            askMV();
            QDateTime date = QDateTime::currentDateTime();
            fillDataAndPlot(date, temperature, smallShift, MV);
            writeData();
            while(getTempTimer.remainingTime() != -1 ){
                waitForMSec(timing::getTempTimer);
            }

        };
        muteLog = false;
    }
  ui->checkBoxStatusSTC->setChecked(false);
}

void MainWindow::on_comboBox_AT_currentIndexChanged(int index)
{
    if(!comboxEnable) return;
    setAT(index);
}


void MainWindow::on_pushButton_Connect_clicked()
{
    QString omronPortName = ui->comboBox_SeriesNumber->currentData().toString();
    LogMsg("=========== setting modbus.");
    omronID = ui->spinBox_DeviceAddress->value();
    omron = new QModbusRtuSerialMaster(this);
    omron->setConnectionParameter(QModbusDevice::SerialPortNameParameter, omronPortName);
    omron->setConnectionParameter(QModbusDevice::SerialBaudRateParameter, QSerialPort::Baud9600);
    omron->setConnectionParameter(QModbusDevice::SerialDataBitsParameter, QSerialPort::Data8);
    omron->setConnectionParameter(QModbusDevice::SerialParityParameter, QSerialPort::NoParity);
    omron->setConnectionParameter(QModbusDevice::SerialStopBitsParameter, QSerialPort::TwoStop);
    omron->setTimeout(timing::timeOut);
    omron->setNumberOfRetries(0);

    if(omron->connectDevice()){
        ui->textEdit_Log->setTextColor(QColor(0,0,255,255));
        LogMsg("The Omron temperature control is connected in " + omronPortName + ".");
        ui->textEdit_Log->setTextColor(QColor(0,0,0,255));
        ui->comboBox_SeriesNumber->setEnabled(false);
        ui->pushButton_Connect->setStyleSheet("background-color: rgb(255,127,80)");
        ui->pushButton_Connect->setEnabled(false);
        panalOnOff(true);
        ui->pushButton_Control->setEnabled(true);
        ui->lineEdit_SV2->setEnabled(false);
        ui->doubleSpinBox_SV2WaitTime->setEnabled(false);
        QString title = this->windowTitle();
        this->setWindowTitle(title + " | " + ui->comboBox_SeriesNumber->currentText());
        getSetting();
        ui->lineEdit_SV->setText(QString::number(SV));
        QString cmd = "00 00 01 01";
        LogMsg("Set Stop.");
        QByteArray value = QByteArray::fromHex(cmd.toStdString().c_str());
        request(QModbusPdu::WriteSingleRegister, value);
        QColor color = QColor("palegray");
        QPalette pal = palette();
        pal.setColor(QPalette::Window, color);
    }else{
        ui->textEdit_Log->setTextColor(QColor(255,0,0,255));
        LogMsg("The Omron temperature control cannot be connected on any COM port.");
        ui->textEdit_Log->setTextColor(QColor(0,0,0,255));
        ui->comboBox_SeriesNumber->setEnabled(true);
        ui->pushButton_Connect->setStyleSheet("");
    }

}

void MainWindow::on_doubleSpinBox_MVlower_valueChanged(double arg1)
{
    if(!spinBoxEnable) return;
    if(!modbusReady) return;

    MVlower = arg1;

    int sv_2 = (qint16) (arg1 / tempDecimal + 0.5);
    qDebug() << sv_2;
    QString valueStr = formatHex(sv_2, 8);
    QString addressStr = formatHex(E5CC_Address::MVlower, 4);

    QString cmd = addressStr + " 00 02 04" + valueStr;
    QByteArray value = QByteArray::fromHex(cmd.toStdString().c_str());
    request(QModbusPdu::WriteMultipleRegisters, value);
    LogMsg("Output lower limit is set to be " + QString::number(arg1));

}

void MainWindow::on_doubleSpinBox_MVupper_valueChanged(double arg1)
{
    if(!spinBoxEnable) return;
    if(!modbusReady) return;

    MVupper = arg1;

    int sv_2 = (qint16) (arg1 / tempDecimal + 0.5);
    qDebug() << sv_2;
    QString valueStr = formatHex(sv_2, 8);
    QString addressStr = formatHex(E5CC_Address::MVupper, 4);

    QString cmd = addressStr + " 00 02 04" + valueStr;
    QByteArray value = QByteArray::fromHex(cmd.toStdString().c_str());
    request(QModbusPdu::WriteMultipleRegisters, value);
    LogMsg("Output upper limit is set to be " + QString::number(arg1));

    plot->yAxis2->setRangeLower(MVupper + 2);
    plot->replot();
}

void MainWindow::on_pushButton_GetPID_clicked()
{
  //get PID constant
  LogMsg("------ get Propertion band.");
  read(QModbusDataUnit::HoldingRegisters, E5CC_Address::PID_P, 2);
  waitTimming();
  LogMsg("------ get integration time.");
  read(QModbusDataUnit::HoldingRegisters, E5CC_Address::PID_I, 2);
  waitTimming();
  LogMsg("------ get derivative time.");
  read(QModbusDataUnit::HoldingRegisters, E5CC_Address::PID_D, 2);
}

void MainWindow::on_comboBox_Mode_currentIndexChanged(int index)
{
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
    read(QModbusDataUnit::HoldingRegisters, address, 2);
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
    int diff = abs(date.secsTo(dateStart_));
    int setrange = plotDialog_->displayRange_ * 60;
    if (diff < setrange) plot->xAxis->rescale();
    else plot->xAxis->setRange(intDate - setrange, intDate);
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
  ui->lineEdit_IntervalAskMV->setEnabled(false);
}

void MainWindow::setIntervalAskTemp(){
  ui->lineEdit_IntervalAskTemp->setEnabled(true);
  ui->lineEdit_IntervalAskTemp->setText(QString::number(configureDialog_->intervalAskTemp_));
  ui->lineEdit_IntervalAskTemp->setEnabled(false);
}

void MainWindow::setNumbers(){
  ui->lineEdit_Numbers->setEnabled(true);
  ui->lineEdit_Numbers->setText(QString::number(configureDialog_->numbers_));
  ui->lineEdit_Numbers->setEnabled(false);
}

void MainWindow::setSafeLimit(){
  ui->lineEdit_SafeLimit->setEnabled(true);
  ui->lineEdit_SafeLimit->setText(QString::number(configureDialog_->safeLimit_));
  ui->lineEdit_SafeLimit->setEnabled(false);
}

void MainWindow::setIgnoreRange(){
  ui->lineEdit_IgnoreLower->setEnabled(true);
  ui->lineEdit_IgnoreUpper->setEnabled(true);
  if(configureDialog_->ignoreEnable_){
    ui->lineEdit_IgnoreLower->setText(QString::number(configureDialog_->ignoreLower_));
    ui->lineEdit_IgnoreUpper->setText(QString::number(configureDialog_->ignoreUpper_));
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
  ui->checkBox_Ignore->setEnabled(false);
}

void MainWindow::setParametersTempCheck(bool mute){
  if (!configureDialog_->warnigcheck_) return;
  isSettParametersTempCheck_ = true;
  if (threadMVcheck_->isRunning()) threadMVcheck_->quit();
  if (threadTempCheck_->isRunning()) threadTempCheck_->quit();
  setIntervalAskMV();
  setIntervalAskTemp();
  setNumbers();
  setSafeLimit();
  setIgnoreRange();
  setIgnoreEnable();
  threadMVcheck_->interval_ = ui->lineEdit_IntervalAskMV->text().toInt()*1000; //ms to sec
  threadTempCheck_->interval_ = ui->lineEdit_IntervalAskTemp->text().toInt()*1000; //ms to sec
  if (!mute){
    LogMsg("set to be parameters for TempCheck.");
    LogMsg(configureDialog_->msg_);
  }
  threadMVcheck_->start();
  threadTempCheck_->start();
  isSettParametersTempCheck_ = false;
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
    if(threadLog_->isRunning()) threadLog_->quit();
    ui->checkBoxStatusRun->setChecked(false);
  }
}

void MainWindow::on_pushButton_Log_toggled(bool checked){
  bool connectPID = ui->pushButton_Connect->isChecked();
  if(checked && connectPID){
    ui->pushButton_Log->setText("Logging Stop");
    if(!threadLog_->isRunning()) threadLog_->start();
    ui->checkBoxStatusRecord->setChecked(true);
  }else if (connectPID) {
    ui->pushButton_Log->setText("Logging Start");
    if(threadLog_->isRunning()) threadLog_->quit();
      ui->checkBoxStatusRecord->setChecked(false);
  } else {
    LogMsg("Not connected. Please check COM PORT etc.");
    if(threadLog_->isRunning()) threadLog_->quit();
    ui->checkBoxStatusRecord->setChecked(false);
  }
}

void MainWindow::on_spinBox_TempRecordTime_valueChanged(int arg1){
  if(threadLog_->isRunning()) threadLog_->quit();
  threadLog_->interval_ = arg1 * 1000; //ms to sec
  threadLog_->start();
  LogMsg("Record Temp Interval set to " + QString::number(threadLog_->interval_ *.001) + " seconds.");
}

//!
//! \brief MainWindow::Run
//! \details Function to start heater output of PID.
//! At the same time, it also starts a thread for data logging and periodically checks output values.
//!
void MainWindow::Run(){
  statusBar()->clearMessage();
  QString cmd = "00 00 01 00";
  LogMsg("Set Run.");
  if (threadLog_->isRunning()) threadLog_->quit();
  if (threadMVcheck_->isRunning()) threadMVcheck_->quit();
  if (threadTempCheck_->isRunning()) threadTempCheck_->quit();
  QByteArray value = QByteArray::fromHex(cmd.toStdString().c_str());
  request(QModbusPdu::WriteSingleRegister, value);
  setColor(1);
  ui->lineEdit_TempCheckCount->setStyleSheet("");
  threadMVcheck_->start();
  threadLog_->start();
  ui->pushButton_Log->setChecked(true);
  ui->checkBoxStatusRun->setChecked(true);
  ui->checkBoxStatusPeriodic->setCheckable(true);
  threadTimer_->start(threadTimerInterval_);
  statusRun_ = true;
  sendLine("Running starts.");
  generateSaveFile();
}

//!
//! \brief MainWindow::Stop
//! \details Function to stop PID output.
//! Stops the threads for output values and TempCheck mode but keeps the thread for data logging running.
//!
void MainWindow::Stop(){
  threadMVcheck_->quit();
  threadLog_->quit();
  threadTempCheck_->quit();
  threadTimer_->stop();
  countTempCheck_ = 0;
  statusBar()->clearMessage();
  QString cmd = "00 00 01 01";
  LogMsg("Set Stop.");
  QByteArray value = QByteArray::fromHex(cmd.toStdString().c_str());
  request(QModbusPdu::WriteSingleRegister, value);
  setColor(0);
  ui->checkBoxStatusRun->setChecked(false);
  ui->checkBoxStatusPeriodic->setChecked(false);
  ui->checkBoxStatusTempDrop->setChecked(false);
  ui->checkBoxStautsTempCheck->setChecked(false);
  ui->checkBoxStatusSTC->setChecked(false);
  ui->action_Setting_parameters_for_TempCheck->setEnabled(true);
  ui->lineEdit_TempCheckCount->clear();
  ui->lineEdit_TempCheckCount->setStyleSheet("");
  statusRun_ = false;
  sendLine("Running stop.");
}

/**
 * @brief MainWindow::Quit
 * @details The function for Emergency stop.
 * Executed when TempCheck mode results in an unsafe condition.
 * Interrupted ModBus communication with the device.
 * Almost thread are terminated excluding threadLog_.
 * So, the log continue to be taken after emergency stop.
 * (See TempCheck function for a call to this function.)
 */
void MainWindow::Quit(){
  QString cmd = "00 00 01 01";
  QByteArray value = QByteArray::fromHex(cmd.toStdString().c_str());
  request(QModbusPdu::WriteSingleRegister, value);
  ui->textEdit_Log->setTextColor(QColor(255,0,0,255));
  LogMsg("Emergency Stop. Check the experimental condition.");
  threadMVcheck_->quit();
  if (threadTempCheck_->isRunning()) threadTempCheck_->quit();
  vtemp_.clear();
  LogMsg("Thred stop.");
  countTempCheck_ = 0;
  ui->textEdit_Log->setTextColor(QColor(0,0,0,255));
  ui->checkBoxStatusRun->setChecked(false);
  ui->checkBoxStatusPeriodic->setChecked(false);
  ui->checkBoxStatusTempDrop->setChecked(false);
  ui->checkBoxStautsTempCheck->setChecked(false);
  ui->checkBoxStatusSTC->setChecked(false);
  ui->pushButton_RunStop->setChecked(false);
  statusRun_ = false;
  threadTimer_->stop();
  sendLine("Emergency Stop!");
  setColor(3);
}


//!
//! \brief MainWindow::isIgnore
//! \param check
//! \param temp
//! \return bool true or false
//! \details This function checks whether the current temperature is within the temperature range to be ignored by the TempCheck mode.
//! It is assumed that the first argument is a checkbox to enable or disable the ignore range parameter of TempCheck.
//! The second argument is assumed to be the current temperature.
//!
bool MainWindow::isIgnore(bool check, double temp){
  if (!check) return true;
  double lower = ui->lineEdit_IgnoreLower->text().toDouble();
  double upper = ui->lineEdit_IgnoreUpper->text().toDouble();
  if (temp+lower <= temperature && temp+upper >= temperature) {
    ui->textEdit_Log->setTextColor(QColor(0,0,255,255));
    LogMsg("Set Temperature is " + QString::number(temp));
    LogMsg("Current temperature is outside the TempCheck mode.");
    LogMsg("TempCheck mode does not work in the range");
    ui->textEdit_Log->setTextColor(QColor(0,0,0,255));
    vtemp_.clear();
    countTempCheck_ = 0;
    ui->lineEdit_TempCheckCount->clear();
    ui->lineEdit_TempCheckCount->setStyleSheet("");
    ui->checkBoxStautsTempCheck->setChecked(false);
    threadTempCheck_->start();
    if (statusRun_) setColor(1);
    else setColor(0);
    return false;
  }
  return true;
}


//!
//! \brief MainWindow::isViolate
//! \param vtemp
//! \return bool true or false
//! \details Returns false if safe and not violated, and eturns true if it is violated.
//!
bool MainWindow::isViolate(QVector<double> vtemp){
  double safelimit = ui->lineEdit_SafeLimit->text().toDouble();
  double dtemp = calcMovingAve(vtemp);
  if (dtemp >= safelimit){
    ui->textEdit_Log->setTextColor(QColor(0,0,255,255));
    LogMsg("Safety.");
    LogMsg("Permitted temperature change is " + QString::number(safelimit));
    LogMsg("Observed temperature change is " + QString::number(dtemp));
    threadTempCheck_->start();
    ui->checkBoxStautsTempCheck->setChecked(false);
    if (statusRun_) setColor(1);
    else setColor(0);
    return false;
  } else return true;
}

//!
//! \brief MainWindow::isDrop
//! \param diff
//! \return bool true or false
//! \details This function returns true when the argument is a positive sign. Otherwise false.
//! It is assumed that a temperature change is assigned to the argument.
//!
bool MainWindow::isDrop(double diff, int mode){
  if(ui->pushButton_Control->isChecked()) return false;
  double interval = ui->spinBox_TempRecordTime->value()/60.0; //sec to min.
  switch (mode) {
    case 1:
      if (diff/interval >= ui->doubleSpinBox_TempDrop->value()) return true;
      else return false;
    break;
    default:
      if (diff >= 0) {
        ui->checkBoxStatusTempDrop->setChecked(false);
        return false;
        }
      else {
        ui->checkBoxStatusTempDrop->setChecked(true);
        return true;
        }
    break;
  }
  return false;
}


void MainWindow::setTextTempDrop(bool enable){
  if(enable) ui->checkBox_TempDropEnable->setText(tr("Yes"));
  else ui->checkBox_TempDropEnable->setText(tr("No"));
}

//!
//! \brief MainWindow::TempCheck
//! \details The temperature stores in vtemp_ until the given number is reached.
//! A moving average is calculated from these values. If this moving average is smaller than the threshold value,
//! it is assumed that there is no temperature change and the Quit function is executed.
//! If it is greater than the threshold value, set countTempCheck_ to 0 and exit TempCheck mode.
//! Note that this function does not work if the temperature is in the ignore range given setParamters function.
//!
void MainWindow::TempCheck(){
  if (countTempCheck_ > ui->lineEdit_Numbers->text().toInt()) countTempCheck_ = 0;
  if (statusAskTemp_) waitForMSec(200);
  askTemperature();
  vtemp_.push_back(temperature);
  if (statusAskSetPoint_) waitForMSec(200);
  askSetPoint();
  double targetvalue = SV;
  bool continue0 = isIgnore(ui->checkBox_Ignore->isChecked(), targetvalue);
  if (!continue0) return;
  ui->checkBoxStautsTempCheck->setChecked(true);
  ui->lineEdit_TempCheckCount->setStyleSheet("background-color:yellow; color:red;selection-background-color:red;");
  ui->lineEdit_TempCheckCount->setText("!! TempCheck is working !!");
  ui->lineEdit_TempCheckCount->setStyleSheet("");
  setColor(2);
  ui->textEdit_Log->setTextColor(QColor(255,0,0,255));
  LogMsg("*** CheckTemp start ****");
  LogMsg("The result in " + QString::number(countTempCheck_));
  ui->textEdit_Log->setTextColor(QColor(0,0,0,255));
  ui->lineEdit_TempCheckCount->setEnabled(true);
  ui->lineEdit_TempCheckCount->setText("Checking in " + QString::number(countTempCheck_));
  ui->lineEdit_TempCheckCount->setEnabled(false);
  if (countTempCheck_ < ui->lineEdit_Numbers->text().toInt()){
    countTempCheck_++;
    threadTempCheck_->start();
    return;
  }
  if (isViolate(vtemp_)) {
    QDateTime date = QDateTime::currentDateTime();
    QString datestr = date.toString("yyyyMMdd_HHmmss");
    ui->lineEdit_TempCheckCount->setStyleSheet("background-color:yellow; color:red;selection-background-color:red;");
    ui->lineEdit_TempCheckCount->setText("Emergency Stop at " + datestr);
    vtemp_.clear();
    ui->checkBoxStautsTempCheck->setChecked(false);
    Quit();
    return;
  } else{
    countTempCheck_ = 0;
    vtemp_.clear();
    setColor(1);
    ui->checkBoxStautsTempCheck->setChecked(false);
    return;
  }
}

//!
//! \brief MainWindow::periodicWork
//! \details Evaluates whether the current output value has reached the upper limit.
//! If the current output has reached the upper limit, then ThreadTempCheck_, the thread for TempCheck, is invoked.
//!
void MainWindow::periodicWork(){
  bool mute = true;
  muteLog = true;
  if(threadMVcheck_->isRunning()) ui->checkBoxStatusPeriodic->setChecked(true);
  else ui->checkBoxStatusPeriodic->setChecked(false);
  if (statusAskMV_) waitForMSec(1000);
  askMV(mute);
  muteLog = false;
  if (MV != MVupper) LogMsg("Current MVpower is below the upper limit.");
  else {
    ui->textEdit_Log->setTextColor(QColor(255,0,0,255));
    LogMsg("Current MVpower reaches upper limit.");
    if (!threadTempCheck_->isRunning()) threadTempCheck_->start();
    if (threadMVcheck_->isRunning()) threadMVcheck_->quit();
    else threadMVcheck_->start();
    ui->textEdit_Log->setTextColor(QColor(0,0,0,255));
  }
}

//!
//! \brief MainWindow::makePlot
//! \details Function to get current temperature, output, and set temperature for plotting and writing data.
//! Refer also fillDatAndPlot.
//!
void MainWindow::makePlot(){
  bool mute = true;
  muteLog = true;
  if(threadLog_->isRunning()) ui->checkBoxStatusRecord->setChecked(true);
  else ui->checkBoxStatusRecord->setChecked(false);
  if (statusAskTemp_) waitForMSec(1000);
  askTemperature(mute);
  if (statusAskSetPoint_) waitForMSec(1000);
  askSetPoint(mute);
  if (statusAskMV_) waitForMSec(1000);
  askMV(mute);
  muteLog = false;
  const double setTemperature = ui->lineEdit_SV->text().toDouble();
  QDateTime date = QDateTime::currentDateTime();
  valltemp_.push_back(temperature);
  fillDataAndPlot(date, temperature, setTemperature, MV);
  if(ui->checkBox_dataSave->isChecked()) writeData();
  double diff = fillDifference(true);
  if (!ui->checkBox_TempDropEnable->isChecked()) return;
  if (isDrop(diff, 1) && countDropCheck_ <= 2) {
      ui->lineEdit_TempCheckCount->setText("Temperature drop is detected.");
      setColor(4);
      countDropCheck_++;
  } else if (isDrop(diff, 1) && countDropCheck_ > 2) {
      Quit();
      ui->lineEdit_TempCheckCount->setText("The temperature drop has exceeded the threshold.");
  } else {
      countDropCheck_ = 0;
      setColor(1);
      ui->lineEdit_TempCheckCount->clear();
  }
}

//!
//! \brief MainWindow::writeData
//! \details The Data is saved to the file generated by on_checkBox_dataSave_toggled.
//!
void MainWindow::writeData(){
  if(!ui->checkBox_dataSave->isChecked()) return;
  LogMsg("data save to : " + fileName_);
  QFile output(fileName_);
  QTextStream stream(&output);
  if (!output.exists()){
    output.open(QIODevice::WriteOnly| QIODevice::Text);
    LogMsg(fileName_ + " does not be found. New file was be generated.");
    generateSaveFile();
  }
  QDateTime date = QDateTime::currentDateTime();
  output.open(QIODevice::Append| QIODevice::Text);
  stream << date.toString("MM-dd HH:mm:ss").toStdString().c_str()
         << "\t"
         << date.toSecsSinceEpoch()
         << "\t"
         << QString::number(temperature)
         << "\t"
         << QString::number(SV)
         << "\t"
         << QString::number(MV)
         << Qt::endl;
  output.close();
}

//!
//! \brief MainWindow::on_checkBox_dataSave_toggled
//! \param checked
//! \details File is generated in this checkbos turn to be true (checked).
//!
void MainWindow::on_checkBox_dataSave_toggled(bool checked)
{
  if(!checked) return;
  generateSaveFile();
}

//!
//! \brief MainWindow::generateSavefile
//! \return bool
//! \details try to generate new file.
//! Returns false if the file already exists.
//! If the file does not exist, the header is written to the file and this function returns true.
//!
bool MainWindow::generateSaveFile(){
  QDateTime startTime = QDateTime::currentDateTime();
  QString name = startTime.toString("yyyyMMdd_HHmmss") + ".dat";
  fileName_ = ui->lineEdit_DirPath->text() + "/" + name;
  QFile output(fileName_);
  QTextStream stream(&output);
  if (output.exists()) {
    LogMsg("file already exists.");
    output.close();
    return false;
  }else {
    output.open(QIODevice::WriteOnly| QIODevice::Text);
    stream <<"Date\t"<<"Date_t\t"<<"temp [C]\t"<<"SV [C]\t"<<"Output [%]" <<Qt::endl;
    output.close();
    return true;
  }
}

//!
//! \brief MainWindow::calcMovingAve
//! \param vtemp
//! \return double aberage value of Neighborhood average of 3 points
//!
double MainWindow::calcMovingAve(QVector<double> vtemp){
  double mave =.0;
  int size = 0;
  for (auto i = 1; i < vtemp.size()-1; i++) {
      mave += (vtemp.at(i-1) + vtemp.at(i) + vtemp.at(i+1) -3.0*vtemp.at(0))/3.0;
      size ++;
    }
  mave /= size;
  LogMsg("MovingAve " + QString::number(mave));
  return mave;
}

//!
//! \brief MainWindow::setColor
//! \param colorindex
//! \details setting background colors.
//! The background color of windows is green, yellow, red, and gray if the color index is 1, 2, 3, and default, respectively.
//!
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

void MainWindow::checkThreads(){
    if(isSettParametersTempCheck_) return;
    if(!statusRun_){
        threadTimer_->stop();
        return;
    }
    if(!threadMVcheck_->isRunning()) {
        for (auto i = 0; i < 10; i++){
            threadMVcheck_->quit();
            threadMVcheck_->run();
        }
    }
    if(!threadTempCheck_->isRunning()) {
        for (auto i = 0; i < 10; i++){
            threadTempCheck_->quit();
            threadTempCheck_->run();
        }
    }
    if(!threadLog_->isRunning()) {
        for (auto i = 0; i < 10; i++){
            threadLog_->quit();
            threadLog_->run();
        }
    }
    if(!threadMVcheck_->isRunning()){
        LogMsg("threadMVcheck is something wrong. Emergency stop.");
        Quit();
    }
    if(!threadTempCheck_->isRunning()){
        LogMsg("threadTempCheck is something wrong. Emergency stop.");
        Quit();
    }
    if(!threadLog_->isRunning()){
        LogMsg("threadLog is something wrong. Emergency stop.");
        Quit();
    }
}

void MainWindow::sendLineNotify(const QString& message, const QString& token) {
    QNetworkAccessManager* manager = new QNetworkAccessManager();
    QNetworkRequest request;
    QUrl url("https://notify-api.line.me/api/notify");
    QUrlQuery postData;

    request.setUrl(url);
    request.setRawHeader("Content-Type", "application/x-www-form-urlencoded");
    request.setRawHeader("Authorization", QString("Bearer %1").arg(token).toUtf8());

    postData.addQueryItem("message", message);
    request.setHeader(QNetworkRequest::ContentLengthHeader, postData.toString().length());

    QObject::connect(manager, &QNetworkAccessManager::finished, [=](QNetworkReply* reply) {
        if (reply->error() != QNetworkReply::NoError) qDebug() << "Error while sending LINE Notify:" << reply->errorString();
        reply->deleteLater();
    });
    manager->post(request, postData.toString().toUtf8());
}

void MainWindow::sendLine(const QString& message){
    QString line_token = "9tYexDQw9KHKyJOAI5gIONbXLZgzolIxungdwos5Dyy";
    sendLineNotify(message, line_token);
}

void MainWindow::sendLineNotifyConnection(QModbusRtuSerialMaster * omron){
    QModbusDevice::State state = omron->state();
    switch (state) {
    case QModbusDevice::UnconnectedState:
        sendLine("Modbus communication is disconnected.");
        break;
    case QModbusDevice::ConnectedState:
        sendLine("Modbus communication is connected");
    default:
        break;
    }
}

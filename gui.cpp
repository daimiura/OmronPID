#include "mainwindow.h"
#include "ui_mainwindow.h"

void MainWindow::setupPlot(){
  plot = ui->plot;
  plot->xAxis->setLabel("Time");
  plot->yAxis->setLabel("Temp. [C]");
  plot->addGraph(plot->xAxis, plot->yAxis2);
  plot->graph(0)->setName("Output");
  plot->graph(0)->setPen(QPen(Qt::darkGreen)); // MV
  plot->addGraph();
  plot->graph(1)->setName("Temp.");
  plot->graph(1)->setPen(QPen(Qt::blue)); // PV
  plot->addGraph();
  plot->graph(2)->setName("Set-temp.");
  plot->graph(2)->setPen(QPen(Qt::red)); // SV
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
}

void MainWindow::setupCombBox(){
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
  comboxEnable = true;
}

void MainWindow::setEnabledFalse(){
  tempControlOnOff = false;
  tempRecordOnOff = false;
  spinBoxEnable = false;
  muteLog = false;
  checkDay = false;
  panalOnOff(false);
  ui->pushButton_Control->setEnabled(false);
  ui-> pushButton_RunStop->setEnabled(false);
  bkgColorChangeable_ = true;
}


void MainWindow::initializeVariables(){
  msgCount = 0;
  dayCounter = 0;
  const size_t vecSize_ = 1000;
  pvData.clear();
  mvData.clear();
  svData.clear();
  //! Reserved memory size
  pvData.reserve(vecSize_);
  svData.reserve(vecSize_);
  mvData.reserve(vecSize_);
  vdifftemp_.reserve(vecSize_);
  vtemp_.reserve(10);
  dateStart_ = QDateTime::currentDateTime();
  clock->stop();
  totalElapse.start();
  waitTimer->stop();
  waitTimer->setSingleShot(false);
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
    ui->comboBox_MemAddress->setEnabled(IO);
    ui->lineEdit_SV2->setEnabled(IO);
    ui->doubleSpinBox_SV2WaitTime->setEnabled(IO);
}

void MainWindow::setupDialog(){
  //! helpDialog
  helpDialog = new QDialog(this);
  HelpLabel = new QLabel();
  helpDialog->setWindowTitle("Help");
  picNumber = 1;
  QPixmap image(":/main_screen.PNG");
  HelpLabel->setPixmap(image);
  QPushButton * next = new QPushButton("Next");
  connect(next, SIGNAL(pressed()) , this, SLOT(HelpPicNext()));
  QVBoxLayout *mainLayout = new QVBoxLayout(helpDialog);
  mainLayout->addWidget(HelpLabel);
  mainLayout->addWidget(next);

  //! plotDialog
  plotDialog_ = new PlotDialog(this);
  plotDialog_->setWindowTitle("Setting for Plot");

  //! configureDialog
  configureDialog_ = new ConfigureDialog(this);
  configureDialog_->setWindowTitle("Configure");
  connect(configureDialog_->pushButton_SetParameters_, SIGNAL(clicked(bool)), this, SLOT(setParametersTempCheckChange()) );

}

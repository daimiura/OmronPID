#include "configuredialog.h"
#include "ui_configuredialog.h"
//#include <QtGui>
//#include <QWidget>
#include <QtWidgets>

/**
 * @brief ConfigureDialog::ConfigureDialog
 * @param parent
 * @details Generate and layput labels, butoons, and etc...
 * Connected signal to slot.
 */
ConfigureDialog::ConfigureDialog(QWidget *parent) :
  QDialog(parent),
  ui(new Ui::ConfigureDialog)
{
  //QMutexLocker lock(&mutex_);
  labelAskMV_ = new QLabel(tr("Interval time to Ask MV (min)."));
  labelAskTemp_ = new QLabel(tr("Interval time to Ask Temperature (min)."));
  labelSize_ = new QLabel(tr("Number of times to monitor temperature during TempCheck mode."));
  label_SafeLimit_ = new QLabel(tr("Lower limit of temperature change permited in TempCheck mode (C)."));
  label_IgnoreEnable_ = new QLabel(tr("Enable TempCheck mode to ignore temperature range."));
  label_IgnoreLower_ = new QLabel(tr("Lower limit of temperature to ignore TempCheck mode (C."));
  label_IgnoreUpper_ = new QLabel(tr("Upper limit of temperature to ignore TempCheck mode (C)."));
  label_ETime_ = new QLabel(tr("Log Message"));
  pushButton_SetParameters_ = new QPushButton(tr("Set Parameters"));
  spinBox_IntervalAskMV_ = new QSpinBox();
  spinBox_IntervalAskTemp_ = new QSpinBox();
  spinBox_Numbers_ = new QSpinBox();
  spinBox_SafeLimit_ = new QDoubleSpinBox();
  spinBox_IgnoreLower_ = new QDoubleSpinBox();
  spinBox_IgnoreUpper_ = new QDoubleSpinBox();
  textBrowser_log_ = new QTextBrowser();
  checkBox_IgnoreEnable_ = new QCheckBox();
  QVBoxLayout *Layout = new QVBoxLayout(this);
  Layout->addWidget(labelAskMV_);
  Layout->addWidget(spinBox_IntervalAskMV_);
  Layout->addWidget(labelAskTemp_);
  Layout->addWidget(spinBox_IntervalAskTemp_);
  Layout->addWidget(labelSize_);
  Layout->addWidget(spinBox_Numbers_);
  Layout->addWidget(label_SafeLimit_);
  Layout->addWidget(spinBox_SafeLimit_);
  Layout->addWidget(label_IgnoreEnable_);
  Layout->addWidget(checkBox_IgnoreEnable_);
  Layout->addWidget(label_IgnoreLower_);
  Layout->addWidget(spinBox_IgnoreLower_);
  Layout->addWidget(label_IgnoreUpper_);
  Layout->addWidget(spinBox_IgnoreUpper_);
  Layout->addWidget(label_ETime_);
  Layout->addWidget(textBrowser_log_);
  Layout->addWidget(pushButton_SetParameters_);

  spinBox_IntervalAskMV_->setMaximum(1000);
  spinBox_IntervalAskTemp_->setMaximum(1000);
  spinBox_Numbers_->setMaximum(100);
  spinBox_SafeLimit_->setMaximum(1000);
  spinBox_IgnoreLower_->setMaximum(0);
  spinBox_IgnoreLower_->setMinimum(-100);

  intervalAskMV_ = 10;
  intervalAskTemp_ = 10;
  numbers_ = 10;
  safeLimit_ = 0.5;
  ignoreLower_ = -10;
  ignoreUpper_ = +10;
  ignoreEnable_= true;

  spinBox_IntervalAskMV_->setValue(intervalAskMV_);
  spinBox_IntervalAskTemp_->setValue(intervalAskTemp_);
  spinBox_Numbers_->setValue(numbers_);
  spinBox_SafeLimit_->setValue(safeLimit_);
  spinBox_IgnoreLower_->setValue(ignoreLower_);
  spinBox_IgnoreUpper_->setValue(ignoreUpper_);
  checkBox_IgnoreEnable_->setChecked(ignoreEnable_);


  spinBox_IntervalAskMV_->setSingleStep(10);
  spinBox_IntervalAskTemp_->setSingleStep(10);
  spinBox_SafeLimit_->setSingleStep(0.1);

  setValues();
  connect(spinBox_IntervalAskTemp_, SIGNAL(valueChanged(int)), this, SLOT(setValues()));
  connect(spinBox_IntervalAskTemp_, SIGNAL(valueChanged(int)), this, SLOT(setValues()));
  connect(spinBox_Numbers_, SIGNAL(valueChanged(int)), this, SLOT(setValues()));
  connect(spinBox_IgnoreLower_, SIGNAL(valueChanged(double)), this, SLOT(setValues()));
  connect(spinBox_IgnoreUpper_, SIGNAL(valueChanged(double)), this, SLOT(setValues()));
  connect(spinBox_SafeLimit_, SIGNAL(valueChanged(double)), this, SLOT(setValues()));
  connect(checkBox_IgnoreEnable_, SIGNAL(stateChanged(int)), this, SLOT(setValues()));
  connect(pushButton_SetParameters_, SIGNAL(clicked(bool)), this, SLOT(setValues()));
}

ConfigureDialog::~ConfigureDialog()
{
  delete ui;
}



/**
 * @brief ConfigureDialog::SetValues
 * @details The member variables in ConfigureDialog class are set to the values of boxes.
 */
void ConfigureDialog::setValues(){
  QMutexLocker lock(&mutex_);
  intervalAskMV_ = spinBox_IntervalAskMV_->value();
  intervalAskTemp_ = spinBox_IntervalAskTemp_->value();
  numbers_ = spinBox_Numbers_->value();
  safeLimit_ = spinBox_SafeLimit_->value();
  ignoreLower_ = spinBox_IgnoreLower_->value();
  ignoreUpper_ = spinBox_IgnoreUpper_->value();
  ignoreEnable_ = checkBox_IgnoreEnable_->isChecked();
  etime_ = numbers_ * intervalAskTemp_;
  textBrowser_log_->setText("*** Send these parameters ***");
  textBrowser_log_->append("Ask MV with interval :" + QString::number(intervalAskMV_) + " min");
  textBrowser_log_->append("Ask Temp with interval : " + QString::number(intervalAskTemp_) + " min");
  textBrowser_log_->append("Average size : " + QString::number(numbers_));
  textBrowser_log_->append("Safety limit : " + QString::number(safeLimit_) + " C");
  if (ignoreEnable_) textBrowser_log_->append("Temperature range to ignore mode is <font color=red>enable</font>");
  else textBrowser_log_->append("Temperature range to ignore mode is <font color=blue>unenable</font>");
  textBrowser_log_->append("Temperature range to ignore : " + QString::number(ignoreLower_) + " =< target value =< " + QString::number(ignoreUpper_));
  textBrowser_log_->append("Estimated time to take while TepCheck mode " + QString::number(etime_));
}

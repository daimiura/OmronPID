#include "configuredialog.h"
#include "ui_configuredialog.h"
#include <QtGui>

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
  QVBoxLayout *Layout = new QVBoxLayout(this);
  Layout->addWidget(labelAskMV_);
  Layout->addWidget(spinBox_IntervalAskMV_);
  Layout->addWidget(labelAskTemp_);
  Layout->addWidget(spinBox_IntervalAskTemp_);
  Layout->addWidget(labelSize_);
  Layout->addWidget(spinBox_Numbers_);
  Layout->addWidget(label_SafeLimit_);
  Layout->addWidget(spinBox_SafeLimit_);
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

  spinBox_IntervalAskMV_->setValue(10);
  spinBox_IntervalAskTemp_->setValue(30);
  spinBox_Numbers_->setValue(10);
  spinBox_SafeLimit_->setValue(0.5);
  spinBox_IgnoreLower_->setValue(-10);
  spinBox_IgnoreUpper_->setValue(+10);

  spinBox_IntervalAskMV_->setSingleStep(10);
  spinBox_IntervalAskTemp_->setSingleStep(10);
  spinBox_SafeLimit_->setSingleStep(0.1);

  calcEstimatedTime();
  //setValues();
  connect(spinBox_IntervalAskTemp_, SIGNAL(valueChanged(int)), this, SLOT(calcEstimatedTime()));
  connect(spinBox_Numbers_, SIGNAL(valueChanged(int)), this, SLOT(calcEstimatedTime()));
  connect(pushButton_SetParameters_, SIGNAL(clicked(bool)), this, SLOT(setValues()));
}

ConfigureDialog::~ConfigureDialog()
{
  delete ui;
}

/**
* @brief ConfigureDialog::SetValues
* @details The function provides time estimation during TempCheck mode.
*/
void ConfigureDialog::calcEstimatedTime(){
  QMutexLocker lock(&mutex_);
  int number = spinBox_Numbers_->value();
  int interval = spinBox_IntervalAskTemp_->value();
  etime_ = number * interval;
  textBrowser_log_->setText("Estimated time to take while TepCheck mode " + QString::number(etime_));
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

  textBrowser_log_->setText("*** Send these parameters ***");
  textBrowser_log_->append("Ask MV with interval :" + QString::number(intervalAskMV_) + " min");
  textBrowser_log_->append("Ask Temp with interval : " + QString::number(intervalAskTemp_) + " min");
  textBrowser_log_->append("Average size : " + QString::number(numbers_));
  textBrowser_log_->append("Safety limit : " + QString::number(safeLimit_) + " C");
  textBrowser_log_->append("Temperature range to ignore : " + QString::number(ignoreLower_) + " =< target value =< " + QString::number(ignoreUpper_));
  textBrowser_log_->append("Estimated time to take while TepCheck mode " + QString::number(etime_));
}

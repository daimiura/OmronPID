#include "configuredialog.h"
//#include "ui_configuredialog.h"
#include <QtWidgets>

/**
 * @brief ConfigureDialog::ConfigureDialog
 * @param parent
 * @details Generate and layput labels, butoons, and etc...
 * Connected signal to slot.
 */
ConfigureDialog::ConfigureDialog(QWidget *parent) :
  QDialog(parent)
{
  labelAskMV_ = new QLabel(tr("Interval time to Ask output power (sec)."));
  labelAskTemp_ = new QLabel(tr("Interval time to Ask Temperature (sec)."));
  labelSize_ = new QLabel(tr("Number of times to monitor temperature during TempCheck mode."));
  label_SafeLimit_ = new QLabel(tr("Lower limit of temperature change permited in TempCheck mode (C)."));
  label_IgnoreEnable_ = new QLabel(tr("Enable TempCheck mode to ignore temperature range."));
  label_IgnoreLower_ = new QLabel(tr("Lower limit of temperature to ignore TempCheck mode (C."));
  label_IgnoreUpper_ = new QLabel(tr("Upper limit of temperature to ignore TempCheck mode (C)."));
  label_ETime_ = new QLabel(tr("Log Message"));
  pushButton_SetParameters_ = new QPushButton(tr("Set Parameters"));
  pushButton_SetParameters_->setCheckable(true);
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

  //! configure boxes and set
  spinBox_IntervalAskMV_->setMaximum(1000);
  spinBox_IntervalAskTemp_->setMaximum(1000);
  spinBox_Numbers_->setMaximum(100);
  spinBox_SafeLimit_->setMaximum(1000);
  spinBox_IgnoreLower_->setMaximum(0);
  spinBox_IgnoreLower_->setMinimum(-100);
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

  //! QMessageBox
  warningMessageBox_.setIcon(QMessageBox::Warning);
  warningMessageBox_.setText(tr("Are you sure you want to change the TempCheck parameters?"));
  warningMessageBox_.setWindowTitle(tr("Warning"));
  warningMessageBox_.setStandardButtons(QMessageBox::Yes | QMessageBox::No);

  connect(spinBox_IntervalAskTemp_, SIGNAL(valueChanged(int)), this, SLOT(setValues()));
  connect(spinBox_IntervalAskTemp_, SIGNAL(valueChanged(int)), this, SLOT(setValues()));
  connect(spinBox_Numbers_, SIGNAL(valueChanged(int)), this, SLOT(setValues()));
  connect(spinBox_IgnoreLower_, SIGNAL(valueChanged(double)), this, SLOT(setValues()));
  connect(spinBox_IgnoreUpper_, SIGNAL(valueChanged(double)), this, SLOT(setValues()));
  connect(spinBox_SafeLimit_, SIGNAL(valueChanged(double)), this, SLOT(setValues()));
  connect(checkBox_IgnoreEnable_, SIGNAL(stateChanged(int)), this, SLOT(setValues()));
  connect(pushButton_SetParameters_, SIGNAL(clicked(bool)), this, SLOT(warningShow(bool)));
}

/*
ConfigureDialog::~ConfigureDialog()
{
  delete ui;
}
*/
/**
 * @brief ConfigureDialog::SetValues
 * @details The member variables in ConfigureDialog class are set to the values of boxes.
 */
void ConfigureDialog::setValues(){
  intervalAskMV_ = spinBox_IntervalAskMV_->value();
  intervalAskTemp_ = spinBox_IntervalAskTemp_->value();
  numbers_ = spinBox_Numbers_->value();
  safeLimit_ = spinBox_SafeLimit_->value();
  ignoreLower_ = spinBox_IgnoreLower_->value();
  ignoreUpper_ = spinBox_IgnoreUpper_->value();
  ignoreEnable_ = checkBox_IgnoreEnable_->isChecked();
  etime_ = numbers_ * intervalAskTemp_;
  textBrowser_log_->setText("*** Send these parameters ***");
  textBrowser_log_->append("Ask Output power every :" + QString::number(intervalAskMV_) + " sec");
  textBrowser_log_->append("Ask Temp. every : " + QString::number(intervalAskTemp_) + " sec");
  textBrowser_log_->append("Average size : " + QString::number(numbers_));
  textBrowser_log_->append("Safety limit : " + QString::number(safeLimit_) + " C");
  if (ignoreEnable_) {
    textBrowser_log_->append("Temperature range to ignore mode is <font color=red>enable</font>.");
    textBrowser_log_->append("Temperature range to ignore : " + QString::number(ignoreLower_) + " =< target value =< " + QString::number(ignoreUpper_));
  } else {
    textBrowser_log_->append("Temperature range to ignore mode is <font color=blue>unenable</font>");
    textBrowser_log_->append("Temperature range to ignore : set to None.");
    textBrowser_log_->append("<font color=blue>Check the box to enable. </font>");
  }
  textBrowser_log_->append("Estimated time to take while TepCheck mode " + QString::number(etime_));
  msg_ = textBrowser_log_->toPlainText();
}

void ConfigureDialog::warningShow(bool checked){
  if (!checked) return;
  warnigcheck_ = false;
  int res = warningMessageBox_.exec();
  if (res == QMessageBox::Yes){
      warnigcheck_ = true;
      setValues();
  } else warnigcheck_ = false;
  pushButton_SetParameters_->setChecked(false);
  warningMessageBox_.hide();
  this->hide();
}

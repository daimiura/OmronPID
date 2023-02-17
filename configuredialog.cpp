#include "configuredialog.h"
#include "ui_configuredialog.h"
#include <QtGui>

ConfigureDialog::ConfigureDialog(QWidget *parent) :
  QDialog(parent),
  ui(new Ui::ConfigureDialog)
{
  labelAskMV_ = new QLabel(tr("Interval time to Ask MV (min)."));
  labelAskTemp_ = new QLabel(tr("Interval time to Ask Temperature (min)."));
  labelSize_ = new QLabel(tr("Number of times to monitor temperature during TempCheck mode."));
  label_SafeLimit_ = new QLabel(tr("Lower limit of temperature change permited in TempCheck mode."));
  label_ETime_ = new QLabel(tr("Estimated time to  for single TempCheck mode (min)."));
  pushButton_SetParameters_ = new QPushButton(tr("Set Parameters"));
  spinBox_IntervalAskMV_ = new QSpinBox();
  spinBox_IntervalAskTemp_ = new QSpinBox();
  spinBox_Numbers_ = new QSpinBox();
  spinBox_SafeLimit_ = new QDoubleSpinBox();
  textBrowser_Etime_ = new QTextBrowser();
  QVBoxLayout *Layout = new QVBoxLayout(this);
  Layout->addWidget(labelAskMV_);
  Layout->addWidget(spinBox_IntervalAskMV_);
  Layout->addWidget(labelAskTemp_);
  Layout->addWidget(spinBox_IntervalAskTemp_);
  Layout->addWidget(labelSize_);
  Layout->addWidget(spinBox_Numbers_);
  Layout->addWidget(label_SafeLimit_);
  Layout->addWidget(spinBox_SafeLimit_);
  Layout->addWidget(label_ETime_);
  Layout->addWidget(textBrowser_Etime_);
  Layout->addWidget(pushButton_SetParameters_);


  spinBox_IntervalAskMV_->setValue(10);
  spinBox_IntervalAskTemp_->setValue(30);
  spinBox_Numbers_->setValue(10);
  spinBox_SafeLimit_->setValue(0.5);

  spinBox_IntervalAskMV_->setSingleStep(10);
  spinBox_IntervalAskTemp_->setSingleStep(30);
  spinBox_SafeLimit_->setSingleStep(0.1);

  spinBox_IntervalAskMV_->setMaximum(1000);
  spinBox_IntervalAskTemp_->setMaximum(1000);
  spinBox_Numbers_->setMaximum(100);
  spinBox_SafeLimit_->setMaximum(1000);




  CalcEstimatedTime();
  connect(spinBox_IntervalAskTemp_, SIGNAL(valueChanged(int)), this, SLOT(CalcEstimatedTime()));
  connect(spinBox_Numbers_, SIGNAL(valueChanged(int)), this, SLOT(CalcEstimatedTime()));
}

ConfigureDialog::~ConfigureDialog()
{
  delete ui;
}

void ConfigureDialog::CalcEstimatedTime(){
  int interval = spinBox_IntervalAskTemp_->value();
  int numbers = spinBox_Numbers_->value();
  int etime = interval * numbers;
  textBrowser_Etime_->setText(QString::number(etime));
}

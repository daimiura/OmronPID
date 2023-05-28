#include "tempdropdialog.h"
#include <QtWidgets>

/**
 * @brief TempDropDialog::TempDropDialog
 * @param parent
 */
TempDropDialog::TempDropDialog(QWidget *parent) :
  QDialog(parent)
{
  labelTemp_ = new QLabel(tr("The Descending temperature at which the emergency stop operates (C/min)"));
  labelEnable_ = new QLabel(tr("Enable？"));
  checkboxEnable_ = new QCheckBox;
  spinBoxTemp_ = new QDoubleSpinBox;
  browser_ = new QTextBrowser;
  QVBoxLayout *Layout = new QVBoxLayout(this);
  Layout->addWidget(labelEnable_);
  Layout->addWidget(checkboxEnable_);
  Layout->addWidget(labelTemp_);
  Layout->addWidget(spinBoxTemp_);
  Layout->addWidget(browser_);
  spinBoxTemp_->setMinimum(0.1);
  spinBoxTemp_->setMaximum(100);
  spinBoxTemp_->setSingleStep(0.1);
  spinBoxTemp_->setValue(10.0);
  checkboxEnable_->setCheckable(true);
  checkboxEnable_->setChecked(true);
  setValues();
  connect(spinBoxTemp_, SIGNAL(valueChanged(double)), this, SLOT(setValues()));
  connect(checkboxEnable_, SIGNAL(stateChanged(int)), this, SLOT(setValues()));
}

void TempDropDialog::setValues(){
  enable_ = this->checkboxEnable_->isChecked();
  temp_ = this->spinBoxTemp_->value();
  if(!enable_) browser_->setText(tr("<font color=blue>Emergency stop due to temperature drop is unenabled.</font>"));
  else {
    browser_->setText(tr("TempDrop mode was enabled."));
    browser_->append(tr("Emergency stop when the temperature drops more than ") + QString::number(temp_) + tr(" C/min."));
  }
}

#include "plotdialog.h"
//#include "ui_plotdialog.h"
#include <QtWidgets>

/**
 * @brief PlotDialog::PlotDialog
 */
PlotDialog::PlotDialog(QWidget *parent) :
  QDialog(parent)
{
  labelRange_ = new QLabel(tr("How many minutes ago do you want to plot from?"));
  spinBoxDisplayRange_ = new QSpinBox();
  browser_ = new QTextBrowser();
  QVBoxLayout *Layout = new QVBoxLayout(this);
  Layout->addWidget(labelRange_);
  Layout->addWidget(spinBoxDisplayRange_);
  Layout->addWidget(browser_);
  spinBoxDisplayRange_->setMinimum(1);
  spinBoxDisplayRange_->setMaximum(1440);
  spinBoxDisplayRange_->setSingleStep(30);
  spinBoxDisplayRange_->setValue(30);

  displayRange_ = spinBoxDisplayRange_->value();
  setValues();
  connect(spinBoxDisplayRange_, SIGNAL(valueChanged(int)), this, SLOT(setValues()));
}

void PlotDialog::setValues(){
  displayRange_ = this->spinBoxDisplayRange_->value();
  browser_->setText(tr("Plot data from ") + QString::number(displayRange_) + tr(" min ago to the present."));
}

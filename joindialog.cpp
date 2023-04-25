#include "joindialog.h"
#include <QtWidgets>

JoinDialog::JoinDialog(QWidget *parent):
  QDialog(parent)
{
  QPixmap pixmap("LINEQR_RIKEN.png");
  labelQR_ = new QLabel;
  //labelQR_ -> setPixmap(pixmap);
  labelQR_->setPixmap(pixmap.scaled(labelQR_->size(), Qt::KeepAspectRatio));
  //labelQR_->setFixedSize(pixmap.size());
  QVBoxLayout* layout = new QVBoxLayout(this);
  layout->addWidget(labelQR_);
}

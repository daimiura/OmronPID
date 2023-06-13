#include "joinlinedialog.h"
#include <QPixmap>

JoinLINEDialog::JoinLINEDialog(QWidget *parent, QString file) : QDialog(parent){
  setWindowTitle("Join LINE Group");
  // Load QR code image from resource file
  QPixmap qrImage(file);
  qrLabel_ = new QLabel(this);
  qrLabel_->setPixmap(qrImage);
  // Add QR code image label to layout
  QVBoxLayout *mainLayout = new QVBoxLayout(this);
  mainLayout->addWidget(qrLabel_);
}

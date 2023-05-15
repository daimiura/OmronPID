#include "joinlinedialog.h"
#include <QPixmap>

JoinLINEDialog::JoinLINEDialog(QWidget *parent) : QDialog(parent){
  setWindowTitle("Join LINE Group");
  QPixmap qrImage(":/LINEQR_RIKEN.png");
  qrLabel_ = new QLabel(this);
  qrLabel_->setPixmap(qrImage);
  QVBoxLayout *mainLayout = new QVBoxLayout(this);
  mainLayout->addWidget(qrLabel_);
}

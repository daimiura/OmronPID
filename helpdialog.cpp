#include "helpdialog.h"
#include <QPixmap>
#include <QLayout>

HelpDialog::HelpDialog(QWidget *parent) : QDialog(parent){
    setWindowTitle("Help");
    picNumber_ = 2;
    helpLabel_ = new QLabel(this);
    QPixmap image(":/main_screen.PNG");
    helpLabel_->setPixmap(image);
    next_ = new QPushButton("Next");
    connect(next_, SIGNAL(pressed()) , this, SLOT(HelpPicNext()));
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(helpLabel_);
    mainLayout->addWidget(next_);
}

void HelpDialog::HelpPicNext(){
  if(helpLabel_ == nullptr) return;

  if( picNumber_ == 4){
      QPixmap image(":screen_color.PNG");
      helpLabel_->setPixmap(image);
      picNumber_ = 1;
      return;
  }
  if( picNumber_ == 3){
      QPixmap image(":/config_screen.png");
      helpLabel_->setPixmap(image);
      picNumber_++;
      return;
  }
  if( picNumber_ == 2){
      QPixmap image(":/safety_screen.PNG");
      helpLabel_->setPixmap(image);
      picNumber_++;
      return;
  }
  if( picNumber_ == 1){
      QPixmap image(":/main_screen.PNG");
      helpLabel_->setPixmap(image);
      picNumber_++;
      return;
  }
}

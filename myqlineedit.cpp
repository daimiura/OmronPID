#include "myqlineedit.h"

MyQLineEdit::MyQLineEdit()
{

}

void MyQLineEdit::ShowInt(int i){
  this->setEnabled(true);
  auto str = QString::number(i);
  this->setText(str);
  this->setEnabled(false);
}

void MyQLineEdit::ShowDouble(double d){
  auto str = QString::number(d);
  this->setText(str);
}

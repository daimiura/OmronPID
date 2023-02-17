#ifndef MYQLINEEDIT_H
#define MYQLINEEDIT_H

#include <QLineEdit>

class MyQLineEdit : public QLineEdit
{
public:
  MyQLineEdit();
signals:
  void ShowInt(int);
  void ShowDouble(double);
};

#endif // MYQLINEEDIT_H

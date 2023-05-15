#ifndef HELPDIALOG_H
#define HELPDIALOG_H

#include <QDialog>
#include <QLabel>
#include <QPushButton>

class HelpDialog : public QDialog
{
  Q_OBJECT
public:
  explicit HelpDialog(QWidget *parent = nullptr);

private slots:
  void HelpPicNext();

private:
  QLabel *helpLabel_{nullptr};
  QPushButton *next_{nullptr};
  int picNumber_{2};
};

#endif // HELPDIALOG_H

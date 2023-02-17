#ifndef CONFIGUREDIALOG_H
#define CONFIGUREDIALOG_H

#include <QDialog>


class QSpinBox;
class QDoubleSpinBox;
class QTextBrowser;
class QLabel;


namespace Ui {
  class ConfigureDialog;
}

class ConfigureDialog : public QDialog
{
  Q_OBJECT

public:
  explicit ConfigureDialog(QWidget *parent = nullptr);
  ~ConfigureDialog();
  QSpinBox * spinBox_IntervalAskMV_;
  QSpinBox * spinBox_IntervalAskTemp_;
  QSpinBox * spinBox_Numbers_;
  QDoubleSpinBox * spinBox_SafeLimit_;
  QTextBrowser * textBrowser_Etime_;
  QPushButton * pushButton_SetParameters_;

private slots:
  //void on_spinBox_Interval_temp_valueChanged(int arg1);
  void CalcEstimatedTime();

private:
  Ui::ConfigureDialog *ui;
  QLabel * labelAskMV_;
  QLabel * labelAskTemp_;
  QLabel * labelSize_;
  QLabel * label_SafeLimit_;
  QLabel * label_ETime_;
};

#endif // CONFIGUREDIALOG_H

/**
* @file confoguredialog.h
* @brief The dialog to set parameters of TempCheck mode.
* @author Daisuke Miura
* @date 2023/2/15
* @details The values of spin boxes are send to the boxes on mainwindow.ui.
*/

#ifndef CONFIGUREDIALOG_H
#define CONFIGUREDIALOG_H

#include <QDialog>
#include <QMutex>
#include <QMutexLocker>

class QSpinBox;
class QDoubleSpinBox;
class QTextBrowser;
class QLabel;
class QCheckBox;

/**
* @namespace UI
*/
namespace Ui {
  class ConfigureDialog;
}

/**
* @brief configuredialog
* @details Boxes, buttons, and labels are defined.
* As slot, calcEstimatedTime and  setValues are defined.
*/
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
  QDoubleSpinBox * spinBox_IgnoreLower_;
  QDoubleSpinBox * spinBox_IgnoreUpper_;
  QTextBrowser * textBrowser_log_;
  QPushButton * pushButton_SetParameters_;
  QCheckBox * checkBox_IgnoreEnable_;

private slots:
  void setValues();

private:
  Ui::ConfigureDialog *ui;
  QLabel * labelAskMV_;
  QLabel * labelAskTemp_;
  QLabel * labelSize_;
  QLabel * label_SafeLimit_;
  QLabel * label_IgnoreEnable_;
  QLabel * label_IgnoreLower_;
  QLabel * label_IgnoreUpper_;
  QLabel * label_ETime_;

  int intervalAskMV_;
  int intervalAskTemp_;
  int numbers_;
  int etime_;
  double safeLimit_;
  double ignoreUpper_;
  double ignoreLower_;
  bool ignoreEnable_;
  QMutex mutex_;
};

#endif // CONFIGUREDIALOG_H

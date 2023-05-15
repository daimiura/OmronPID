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
#include <QMessageBox>

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
  //~ConfigureDialog();

  bool ignoreEnable_{true};
  int intervalAskMV_{10};
  int intervalAskTemp_{10};
  int numbers_{10};
  int etime_{0};
  double safeLimit_{0.3};
  double ignoreUpper_{-10.0};
  double ignoreLower_{10.0};
  bool warnigcheck_{false};
  QString msg_{};
  QMessageBox warningMessageBox_{nullptr};
  QPushButton * pushButton_SetParameters_{nullptr};

private slots:
  void setValues();
  void warningShow(bool checked);

private:
  Ui::ConfigureDialog *ui;
  QLabel * labelAskMV_{nullptr};
  QLabel * labelAskTemp_{nullptr};
  QLabel * labelSize_{nullptr};
  QLabel * label_SafeLimit_{nullptr};
  QLabel * label_IgnoreEnable_{nullptr};
  QLabel * label_IgnoreLower_{nullptr};
  QLabel * label_IgnoreUpper_{nullptr};
  QLabel * label_ETime_{nullptr};

  QSpinBox * spinBox_IntervalAskMV_{nullptr};
  QSpinBox * spinBox_IntervalAskTemp_{nullptr};
  QSpinBox * spinBox_Numbers_{nullptr};
  QDoubleSpinBox * spinBox_SafeLimit_{nullptr};
  QDoubleSpinBox * spinBox_IgnoreLower_{nullptr};
  QDoubleSpinBox * spinBox_IgnoreUpper_{nullptr};
  QTextBrowser * textBrowser_log_{nullptr};
  QCheckBox * checkBox_IgnoreEnable_{nullptr};
};

#endif // CONFIGUREDIALOG_H

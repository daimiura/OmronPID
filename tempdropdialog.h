/**
* @file tempdropdialog.h
* @brief The dialog to set parameters of TempDrop.
* @author Daisuke Miura
* @date 2023/3/23
* @details The values of spin boxes are send to the boxes on mainwindow.ui.
*/

#ifndef TEMPDROPDIALOG_H
#define TEMPDROPDIALOG_H

#include <QDialog>
class QDoubleSpinBox;
class QTextBrowser;
class QLabel;
class QCheckBox;

/**
 * @class The PlotDialog class
 */
class TempDropDialog : public QDialog
{
  Q_OBJECT

public:
  explicit TempDropDialog(QWidget *parent = nullptr);
  bool enable_;
  double temp_;

private slots:
  void setValues();
private:
  QLabel *labelTemp_, *labelEnable_;
  QDoubleSpinBox *spinBoxTemp_;
  QCheckBox *checkboxEnable_;
  QTextBrowser *browser_;
};

#endif // TEMPDROPDIALOG_H

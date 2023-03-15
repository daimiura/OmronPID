/**
* @file plotdialog.h
* @brief The dialog to set parameters of qcustomplot.
* @author Daisuke Miura
* @date 2023/3/9
* @details The values of spin boxes are send to the boxes on mainwindow.ui.
*/

#ifndef PLOTDIALOG_H
#define PLOTDIALOG_H

#include <QDialog>
#include <QMutex>
class QSpinBox;
class QTextBrowser;
class QLabel;


/**
 * @brief The PlotDialog class
 */
class PlotDialog : public QDialog
{
  Q_OBJECT

public:
  explicit PlotDialog(QWidget *parent = nullptr);
  int displayRange_;

private slots:
  void setValues();
private:
  QLabel *labelRange_;
  QSpinBox * spinBoxDisplayRange_;
  QTextBrowser *browser_;
};

#endif // PLOTDIALOG_H

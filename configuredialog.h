/**
 * @file configuredialog.h
 * @brief Header file for the ConfigureDialog class
*/
#ifndef CONFIGUREDIALOG_H
#define CONFIGUREDIALOG_H

#include <QDialog>
#include <QMessageBox>

class QSpinBox;
class QDoubleSpinBox;
class QTextBrowser;
class QLabel;
class QCheckBox;

/**

@class ConfigureDialog
@brief Dialog for configuring parameters of the Omron_PID application
*/
class ConfigureDialog : public QDialog
{
  Q_OBJECT
public:
/**
@brief Constructs a ConfigureDialog object
@param parent The parent widget of the dialog
*/
  explicit ConfigureDialog(QWidget *parent = nullptr);
  bool ignoreEnable_{true}; /**< Flag indicating whether to ignore invalid readings */
  int intervalAskMV_{10}; /**< Interval for requesting MV data */
  int intervalAskTemp_{10}; /**< Interval for requesting temperature data */
  int numbers_{10}; /**< Number of data points for graphing */
  int etime_{0}; /**< Elapsed time */
  double safeLimit_{0.3}; /**< Safety limit for process variable */
  double ignoreUpper_{-10.0}; /**< Upper bound for ignoring invalid readings */
  double ignoreLower_{10.0}; /**< Lower bound for ignoring invalid readings */
  bool warnigcheck_{false}; /**< Flag indicating whether the warning dialog is checked */
  QString msg_{}; /**< Message to display in the warning dialog */
  QMessageBox warningMessageBox_{nullptr}; /**< Warning message box */
  QPushButton *pushButton_SetParameters_{nullptr}; /**< Button for setting parameters */

private slots:
  /**

  @brief Sets the configured values when the Set Parameters button is clicked
  */
  void setValues();
  /**

  @brief Shows the warning dialog
  @param checked Flag indicating whether the warning checkbox is checked
  */
  void warningShow(bool checked);
  private:
  QLabel *labelAskMV_{nullptr}; /**< Label for intervalAskMV_ */
  QLabel *labelAskTemp_{nullptr}; /**< Label for intervalAskTemp_ */
  QLabel *labelSize_{nullptr}; /**< Label for numbers_ */
  QLabel *label_SafeLimit_{nullptr}; /**< Label for safeLimit_ */
  QLabel *label_IgnoreEnable_{nullptr}; /**< Label for ignoreEnable_ */
  QLabel *label_IgnoreLower_{nullptr}; /**< Label for ignoreLower_ */
  QLabel *label_IgnoreUpper_{nullptr}; /**< Label for ignoreUpper_ */
  QLabel *label_ETime_{nullptr}; /**< Label for etime_ */

  QSpinBox *spinBox_IntervalAskMV_{nullptr}; /**< Spin box for intervalAskMV_ */
  QSpinBox *spinBox_IntervalAskTemp_{nullptr}; /**< Spin box for intervalAskTemp_ */
  QSpinBox *spinBox_Numbers_{nullptr}; /**< Spin box for numbers_ */
  QDoubleSpinBox *spinBox_SafeLimit_{nullptr}; /**< Double spin box for safeLimit_ */
  QDoubleSpinBox *spinBox_IgnoreLower_{nullptr}; /**< Double spin box for ignoreLower_ */
  QDoubleSpinBox *spinBox_IgnoreUpper_{nullptr}; /**< Double spin box for ignoreUpper_ */
  QTextBrowser *textBrowser_log_{nullptr}; /**< Text browser for logging */
  QCheckBox *checkBox_IgnoreEnable_{nullptr}; /**< Checkbox for ignoreEnable_ */
};

#endif // CONFIGUREDIALOG_H

#ifndef HELPDIALOG_H
#define HELPDIALOG_H

#include <QDialog>
#include <QLabel>
#include <QPushButton>

/**
 * @brief The HelpDialog class displays a dialog with help information.
 */
class HelpDialog : public QDialog
{
  Q_OBJECT
public:
  /**
   * @brief Constructs a HelpDialog with a given parent widget.
   * @param parent The parent widget of the dialog.
   */
  explicit HelpDialog(QWidget *parent = nullptr);

private slots:
  /**
   * @brief Advances to the next picture in the help dialog.
   */
  void HelpPicNext();

private:
  QLabel *helpLabel_{nullptr}; /**< The label displaying the help picture. */
  QPushButton *next_{nullptr}; /**< The button to go to the next help picture. */
  int picNumber_{2}; /**< The current picture number in the help dialog. */
};

#endif // HELPDIALOG_H

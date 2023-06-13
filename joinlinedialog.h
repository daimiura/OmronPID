/**
 * @file jpindialog.h
 * @brief Header file for the JoinDialog class
*/

#ifndef JOINLINEDIALOG_H
#define JOINLINEDIALOG_H

#include <QDialog>
#include <QLabel>
#include <QVBoxLayout>

/**
 * @brief The JoinLINEGroup class displays a dialog for joining a LINE group.
 */
class JoinLINEDialog : public QDialog
{
    Q_OBJECT
public:
  /**
   * @brief Constructs a new JoinLINEGroup dialog with the specified parent widget.
   * @param parent The parent widget of the dialog.
   */
    explicit JoinLINEDialog(QWidget *parent = nullptr, QString file = nullptr);

private:
    QLabel *qrLabel_; /**< The label displaying the QR code image.*/
};

#endif // JOINLINEDIALOG_H

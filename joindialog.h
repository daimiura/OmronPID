#ifndef JOINDIALOG_H
#define JOINDIALOG_H

#include <QDialog>
#include <QMutex>
#include <QMutexLocker>
#include <QMessageBox>
#include <QPixmap>

class QLabel;

namespace Ui {
  class ConfigureDialog;
}

class JoinDialog : public QDialog
{
    Q_OBJECT
public:
    explicit JoinDialog(QWidget *parent = nullptr);
private:
    QLabel * label_;
    QLabel * labelQR_;
};

#endif // JOINDIALOG_H

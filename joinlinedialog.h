#ifndef JOINLINEDIALOG_H
#define JOINLINEDIALOG_H

#include <QDialog>
#include <QLabel>
#include <QVBoxLayout>

class JoinLINEDialog : public QDialog
{
    Q_OBJECT
public:
    explicit JoinLINEDialog(QWidget *parent = nullptr);

private:
    QLabel *qrLabel_;
};

#endif // JOINLINEDIALOG_H

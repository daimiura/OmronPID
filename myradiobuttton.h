#ifndef MYRADIOBUTTTON_H
#define MYRADIOBUTTTON_H

#include <QRadioButton>
#include <QDebug>
#include <QLayout>


class MyPushButton : public QRadioButton
    Q_OBJECT

public:
    MyRadioButton(QRadioButton *radioButton, QWidget *parent = nullptr);
    MyRadioButton(QWidget *parent = nullptr);

public slots:
};

#endif // MyRadioButton_H

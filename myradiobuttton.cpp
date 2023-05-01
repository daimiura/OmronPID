#ifndef MYRADIOBUTTON_H
#define MYRADIOBUTTON_H

#include <QRadioButton>
#include <QDebug>
#include <QLayout>


class MyRadioButton : public QRadioButton
{
    Q_OBJECT

public:
    MyRadioButton(QRadioButton *pushButton, QWidget *parent = nullptr);
    MyRadioButton(QWidget *parent = nullptr);

public slots:
};

#endif // MYRADIOBUTTON_H

#ifndef MYPUSHBUTTON_H
#define MYPUSHBUTTON_H

#include <QPushButton>
#include <QDebug>
#include <QLayout>


class MyPushButton : public QPushButton
{
    Q_OBJECT

public:
    MyPushButton(QPushButton *pushButton, QWidget *parent = nullptr);
    MyPushButton(QWidget *parent = nullptr);

public slots:
};

#endif // MYPUSHBUTTON_H

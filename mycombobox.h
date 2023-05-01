#ifndef MYCOMBOBOX_H
#define MYCOMBOBOX_H

#include <QComboBox>
#include <QDebug>
#include <QLayout>


class MyComboBox : public QComboBox
{
    Q_OBJECT

public:
    MyComboBox(QComboBox *comboBox, QWidget *parent = nullptr);
    MyComboBox(QWidget *parent = nullptr);

public slots:
};

#endif // MYCOMBOBOX_H

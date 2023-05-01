#include "mypushbutton.h"
#include <QDebug>
#include <QLayout>


MyPushButton::MyPushButton(QPushButton *pushButton, QWidget *parent) : QPushButton(parent)
{
    QRect buttonGeometry = pushButton->geometry();
    QSize buttonSize = buttonGeometry.size();
    QPoint buttonPosition = buttonGeometry.topLeft();

    setStyleSheet(pushButton->styleSheet());
    setText(pushButton->text());
    setIcon(pushButton->icon());
    setIconSize(pushButton->iconSize());
    parent->layout()->addWidget(this);
    setGeometry(QRect(buttonPosition, buttonSize));
}

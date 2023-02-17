#include "mythread.h"

MyThread::MyThread(QObject *parent) :
    QThread(parent)
{
}

void MyThread::run(){
    QTimer timer;
    connect(&timer, SIGNAL(timeout()), this, SLOT(timerHit()), Qt::DirectConnection );
    timer.setInterval(interval_); //ms
    timer.start();
    exec();
    timer.stop();
}

void MyThread::timerHit(){
    emit data_update();
}

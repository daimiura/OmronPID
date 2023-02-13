#include "mythread.h"

MyThread::MyThread(QObject *parent) :
    QThread(parent)
{
}

void MyThread::run(){
    QTimer timer;
    connect(&timer, SIGNAL(timeout()), this, SLOT(timerHit()), Qt::DirectConnection );
    timer.setInterval(1000); //ms
    timer.start();
    exec();
    timer.stop();
}

void MyThread::timerHit(){
    static int i;
    emit data_update( i++ );
}

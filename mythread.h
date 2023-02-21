#ifndef MYTHREAD_H
#define MYTHREAD_H

#include <QThread>
#include <QMutex>
#include <QMutexLocker>
#include <QTimer>

class MyThread : public QThread
{
    Q_OBJECT
public:
    explicit MyThread(QObject *parent = nullptr);
    void run();
    int interval_;
signals:
    void data_update();

public slots:
private slots:
    void timerHit();
private:
    QMutex mutex_;
};

#endif // MYTHREAD_H

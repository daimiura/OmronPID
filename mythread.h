#ifndef MYTHREAD_H
#define MYTHREAD_H

#include <QThread>
#include <QTimer>

class MyThread : public QThread
{
    Q_OBJECT
public:
    explicit MyThread(QObject *parent = nullptr);
    void run();

signals:
    void data_update(int i);

public slots:

private slots:
    void timerHit();
};

#endif // MYTHREAD_H

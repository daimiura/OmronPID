#ifndef NOTIFY_H
#define NOTIFY_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>

class Notify : public QObject
{
    Q_OBJECT

public:
    Notify(QObject *parent = nullptr);
    virtual ~Notify();
    void setLINE(const QString& message, const QString& token);

private slots:
    void handleNetworkReply(QNetworkReply *reply);

private:
    QNetworkAccessManager networkManager_;
    QUrl url_;
};

#endif // NOTIFY_H

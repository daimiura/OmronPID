#include "Notify.h"
#include <QUrlQuery>

Notify::Notify(QObject *parent)
    : QObject(parent)
{
}

Notify::~Notify() {}


void Notify::setLINE(const QString& message, const QString& token){
    QNetworkRequest request;
    QUrl url("https://notify-api.line.me/api/notify");
    QUrlQuery postData;
    request.setUrl(url);
    request.setRawHeader("Content-Type", "application/x-www-form-urlencoded");
    request.setRawHeader("Authorization", QString("Bearer %1").arg(token).toUtf8());
    postData.addQueryItem("message", message);
    request.setHeader(QNetworkRequest::ContentLengthHeader, postData.toString().length());
    connect(&networkManager_, &QNetworkAccessManager::finished, this, &Notify::handleNetworkReply);
    networkManager_.post(request, postData.toString().toUtf8());
}

void Notify::handleNetworkReply(QNetworkReply *reply){
    if (reply->error() != QNetworkReply::NoError) qDebug() << "Error while sending LINE Notify:" << reply->errorString();
    reply->deleteLater();
}

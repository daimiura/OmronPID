#include "Notify.h"
#include <QUrlQuery>

/**
 * @brief Constructs a Notify object with a parent object
 * @param parent The parent object of this object
 */
Notify::Notify(QObject *parent)
    : QObject(parent),
      url_("https://notify-api.line.me/api/notify")
{
}

/**
 * @brief Destroys the Notify object
 */
Notify::~Notify() {}


/**
 * @brief Sends a notification to LINE messaging API with the provided message and token.
 *
 * @param message The message to send.
 * @param token The LINE API token to authenticate with.
 *
 * This method creates a HTTP POST request to the LINE messaging API using the provided message and token. The request
 * is sent using the QNetworkAccessManager class. The method waits for the reply from the server in a non-blocking
 * manner by connecting to the QNetworkAccessManager::finished signal. Once the reply is received, the
 * Notify::handleNetworkReply() method is called to process the reply.
 */
void Notify::setLINE(const QString& message, const QString& token){
    QNetworkRequest request;
    QUrlQuery postData;
    request.setUrl(url_);
    request.setRawHeader("Content-Type", "application/x-www-form-urlencoded");
    request.setRawHeader("Authorization", QString("Bearer %1").arg(token).toUtf8());
    postData.addQueryItem("message", message);
    request.setHeader(QNetworkRequest::ContentLengthHeader, postData.toString().length());
    connect(&networkManager_, &QNetworkAccessManager::finished, this, &Notify::handleNetworkReply);
    networkManager_.post(request, postData.toString().toUtf8());
}

/**
 * @brief Handles the network reply from the service
 * @param reply The network reply from the service
 */
void Notify::handleNetworkReply(QNetworkReply *reply){
    if (reply->error() != QNetworkReply::NoError)
        qDebug() << "Error while sending LINE Notify:" << reply->errorString();
    reply->deleteLater();
}

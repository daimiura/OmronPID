#ifndef NOTIFY_H
#define NOTIFY_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>

/**
@brief The Notify class provides a way to send notifications to different services
*/
class Notify : public QObject
{
Q_OBJECT
public:
  /**
  * @brief Constructs a Notify object with a parent object
  * @param parent The parent object of this object
  */
  Notify(QObject *parent = nullptr);

  /**
   * @brief Destroys the Notify object
   */
  virtual ~Notify();

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
void setLINE(const QString& message, const QString& token);
private slots:
  /**
  * @brief Handles the network reply from the service
  * @param reply The network reply from the service
  */
  void handleNetworkReply(QNetworkReply *reply);

private:
  QNetworkAccessManager networkManager_{};
  QUrl url_{};
};

#endif // NOTIFY_H

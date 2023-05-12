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
 * @brief Sets the message to be sent to the LINE service
 * @param message The message to be sent
 * @param token The token to be used for authentication
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

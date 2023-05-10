#ifndef DATASUMMARY_H
#define DATASUMMARY_H

#include <QObject>

class DataSummary: public QObject {
  Q_OBJECT
public:
  explicit Data(Communication* com, QObject* parent = nullptr);

  Q_PROPERTY(double temperature READ temperature NOTIFY temperatureChanged)
  Q_PROPERTY(double mv READ mv NOTIFY mvChanged)
  Q_PROPERTY(double sv READ sv NOTIFY svChanged)
  Q_PROPERTY(int interval READ interval WRITE setInterval NOTIFY intervalChanged)

  double temperature() const;
  double mv() const;
  double sv() const;
  void setInterval(int msec);

signals:
  void temperatureChanged();
  void mvChanged();
  void svChanged();
  void intervalChanged();

private slots:
  void updateData();

private:
  Communication* com_;
  QTimer* timer_;
  double temperature_;
  double mv_;
  double sv_;
  int interval_;
};



#endif // DATASUMMARY_H

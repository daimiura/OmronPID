#ifndef DATASUMMARY_H
#define DATASUMMARY_H

#include "communication.h"
#include <QObject>

class Communication;

class DataSummary : public QObject
{
    Q_OBJECT

public:
    explicit DataSummary(Communication* com);
    double getTemperature() const;
    double getMV() const;
    double getSV() const;
    void setFileName(QString name);
    void setSave(bool save);

    bool generateSaveFile();
    int writeData();

signals:
    void temperatureChanged(double temperature);
    void mvChanged(double mv);
    void svChanged(double sv);
    void FileSave(bool sucess);

private slots:
    void setTemperature(double temperature);
    void setMV(double mv);
    void setSV(double sv);


private:
    Communication *com_;
    double temperature_{};
    double mv_{};
    double sv_{};
    QString fileName_{};
    QString filePath_{};
    bool save_{true};



    const QString DESKTOP_PATH = QStandardPaths::locate(QStandardPaths::DesktopLocation, QString(), QStandardPaths::LocateDirectory);
    const QString DATA_PATH_2 = DESKTOP_PATH + "Temp_Record";
    const QString DATA_PATH = "Z:/triplet/Temp_Record";
};

#endif // DATASUMMARY_H

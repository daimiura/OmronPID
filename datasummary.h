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
    QString getFileName() const;
    QString getFilePath() const;
    void setFileName(QString name);
    bool generateSaveFile();
    void writeData();
    void logingStart();
    void logingStop();
    void SetIntervalLog(int interval){setIntervalLog(interval);}

signals:
    void temperatureChanged(double temperature);
    void mvChanged(double mv);
    void svChanged(double sv);
    void FileSave(bool sucess);

private slots:
    void setTemperature(double temperature);
    void setMV(double mv);
    void setSV(double sv);
    void setSave(bool save);
    void setIntervalLog(int interval);

private:
    Communication *com_;
    double temperature_{};
    double mv_{};
    double sv_{};
    QString fileName_{};
    QString filePath_{};
    bool save_{true};
    QTimer *logTimer_;
    int intervalLog_{10 * 1000};

    const QString desktopPath_{QStandardPaths::locate(QStandardPaths::DesktopLocation, QString(), QStandardPaths::LocateDirectory)};
    const QString dataPath2_{desktopPath_ + "Temp_Record"};
    const QString dataPath_{"Z:/triplet/Temp_Record"};
};

#endif // DATASUMMARY_H

#ifndef DATASUMMARY_H
#define DATASUMMARY_H

#include "communication.h"
#include <QObject>

class Communication;

/**
 * @brief Class representing summary information of data.
 */
class DataSummary : public QObject
{
    Q_OBJECT

public:
    /**
     * @brief Constructor for DataSummary class.
     * @param com Pointer to Communication class.
     */
    explicit DataSummary(Communication* com);

    /**
     * @brief Gets the current temperature.
     * @return The temperature value.
     */
    double getTemperature() const;

    /**
     * @brief Gets the current MV value.
     * @return The MV value.
     */
    double getMV() const;

    /**
     * @brief Gets the current MV upper value.
     * @return The MV upper value.
     */
    double getMVUpper() const;

    /**
     * @brief Gets the current MV lower value.
     * @return The MV lower value.
     */
    double getMVLower() const;


    /**
     * @brief Gets the current SV value.
     * @return The SV value.
     */
    double getSV() const;


    /**
     * @brief Gets the file name for saving data.
     * @return The file name.
     */
    QString getFileName() const;

    /**
     * @brief Gets the file path for saving data.
     * @return The file path.
     */
    QString getFilePath() const;

    /**
     * @brief Gets the Qtimer for saving data.
     * @return The Qtimer pointer.
     */
    QTimer* getLogTimer() const;

    /**
     * @brief Sets the file name for saving data.
     * @param name The file name.
     */
    void setFileName(QString name);

    /**
     * @brief Generates and saves the data to file.
     * @return true if the file was saved successfully, false otherwise.
     */
    bool generateSaveFile();

    /**
     * @brief Writes data to file.
     */
    void writeData();

    /**
     * @brief Starts logging.
     */
    void logingStart();

    /**
     * @brief Stops logging.
     */
    void logingStop();

    /**
     * @brief Sets the interval for logging data.
     * @param interval The interval in milliseconds.
     */
    void SetIntervalLog(int interval){setIntervalLog(interval);}

    /**
     * @brief Check if the timer for logging data is currently running.
     */
    bool isTimerLogRunning() const;

signals:
    /**
     * @brief Signal emitted when the temperature changes.
     * @param temperature The new temperature value.
     */
    void temperatureChanged(double temperature);

    /**
     * @brief Signal emitted when the MV value changes.
     * @param mv The new MV value.
     */
    void mvChanged(double mv);

    /**
     * @brief Signal emitted when the SV value changes.
     * @param sv The new SV value.
     */
    void svChanged(double sv);

private slots:
    /**
     * @brief Slot to set the temperature value.
     * @param temperature The new temperature value.
     */
    void setTemperature(double temperature);

    /**
     * @brief Slot to set the MV value.
     * @param mv The new MV value.
     */
    void setMV(double mv);

    /**
     * @brief Slot to set the MV upper value.
     * @param mv The new MV upper value.
     */
    void setMVUpper(double mvUpper);

    /**
     * @brief Slot to set the MV lower value.
     * @param mv The new MV lower value.
     */
    void setMVLower(double mvLower);

    /**
     * @brief Slot to set the SV value.
     * @param sv The new SV value.
     */
    void setSV(double sv);

    /**
     * @brief Slot to set whether the file was saved successfully.
     * @param save true if the file was saved successfully, false otherwise.
     */
    void setSave(bool save);

    /**
     * @brief Slot to set the interval for logging data.
     * @param interval The interval in milliseconds.
     */
    void setIntervalLog(int interval);


private:
    const QString desktopPath_{QStandardPaths::locate(QStandardPaths::DesktopLocation, QString(), QStandardPaths::LocateDirectory)};
    const QString dataPath2_{desktopPath_ + "Temp_Record"};
    const QString dataPath_{"Z:/triplet/Temp_Record"};
    Communication *com_{nullptr};
    double temperature_{};
    double mv_{};
    double mvUpper_{};
    double mvLower_{};
    double sv_{};
    QString fileName_{};
    QString filePath_{};
    bool save_{true};
    QTimer *logTimer_{nullptr};
    int intervalLog_{10 * 1000};
};

#endif // DATASUMMARY_H

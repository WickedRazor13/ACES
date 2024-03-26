#ifndef MEASUREMENTMODULE_H
#define MEASUREMENTMODULE_H

#include <QObject>
#include <QFile>
#include <QTextStream>
#include <QDateTime>
#include <QMap>

class MeasurementModule : public QObject
{
    Q_OBJECT
public:

    enum eventType {
        display = 0,
        correct,
        incorrect,
        STDinfo,
        Meminfo
    };
    Q_ENUM(eventType)

    explicit MeasurementModule(QObject *parent = nullptr, int game = 0);
    ~MeasurementModule();

    void writeCSV(const QString& data);
    QString eventToString(eventType t);

public slots:
    void logEvent(eventType t);
    void startCount();

signals:
    void gameStarted();

private:
    QFile logFile;
    QDateTime lastEventTime;
    QDateTime gameStartTime;

    int game; // 0 = STD, 1 = memory
};

#endif // MEASUREMENTMODULE_H

#ifndef MEASUREMENTMODULE_H
#define MEASUREMENTMODULE_H

#include <QObject>
#include <QFile>
#include <QTextStream>
#include <QDateTime>
#include <QMap>
#include "bluetoothmanager.h"

class MeasurementModule : public QObject
{
    Q_OBJECT
public:

    enum eventType {
        Correct = 0,
        Incorrect,
        STDstart,
        STDend,
        MemStart,
        MemEnd,
        MemBreak,
        Display,
        TetStart,
        TetEnd,
        TetNewPiece,
        TetPlace,
        TetRowClear
    };
    Q_ENUM(eventType)

    explicit MeasurementModule(QObject *parent = nullptr, int game = 0);
    ~MeasurementModule();

    void writeCSV(const QString& data);
    QString eventToString(eventType t);

public slots:
    void logEvent(eventType t, BluetoothManager *b);
    void startCount();

signals:
    void gameStarted();

private:
    QFile logFile;
    QDateTime lastEventTime;
    QDateTime gameStartTime;

    int game; // 0 = STD, 1 = memory, 2 = Tetris
};

#endif // MEASUREMENTMODULE_H

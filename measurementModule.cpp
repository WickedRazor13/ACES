#include "measurementModule.h"

MeasurementModule::MeasurementModule(QObject *parent, int game)
    : QObject{parent}
{
    if (game == 0) {
        logFile.setFileName("STD_log.csv");
    }
    else if (game == 1) {
        logFile.setFileName("Mem_log.csv");
    }
    else if (game == 2) {
        logFile.setFileName("Tet_log.csv");
    }
    logFile.open(QIODevice::WriteOnly | QIODevice::Text);

    writeCSV("TimeStamp, TimeSinceLastEvent (ms), TotalTime (ms), EventType");

    connect(this, &MeasurementModule::gameStarted, this, &MeasurementModule::startCount);
}

MeasurementModule::~MeasurementModule()
{
    // Close the logfile
    logFile.close();
}

void MeasurementModule::logEvent(eventType t, BluetoothManager *b)
{
    // Calculate the time since the last event
    QDateTime currentTime = QDateTime::currentDateTime();
    qint64 timeSinceLastEvent = lastEventTime.msecsTo(currentTime);

    // Update the last event time
    lastEventTime = currentTime;

    // Calculate the total time since the start of the game
    qint64 totalTime = gameStartTime.msecsTo(currentTime);
    QString currentTimeString = currentTime.toString("hh:mm:ss.zzz");

    QString event = currentTimeString + ", "
                    + QString::number(timeSinceLastEvent) + ", "
                    + QString::number(totalTime) + ", "
                    + eventToString(t) + "\n";

    // Write the time since the last event to the CSV file

    //writeCSV(currentTimeString + ", " + QString::number(timeSinceLastEvent) + ", " + QString::number(totalTime) + ", " + event);
    writeCSV(event);
    QByteArray info = event.toUtf8();
    b->sendData(info);  // Send over bluetooth
}

void MeasurementModule::startCount()
{
    lastEventTime = QDateTime::currentDateTime();
    gameStartTime = QDateTime::currentDateTime();
    writeCSV("\n");
}

void MeasurementModule::writeCSV(const QString& data)
{
    QTextStream stream(&logFile);
    stream << data << "\n";
}

QString MeasurementModule::eventToString(eventType t)
{
    static QMap<eventType, QString> enumMap = {
        {Correct, "Correct"},
        {Incorrect, "Incorrect"},
        {STDstart, "STDstart"},
        {STDend, "STDend"},
        {MemStart, "MemStart"},
        {MemEnd, "MemEnd"},
        {MemBreak, "MemBreak"},
        {Display, "Display"},
        {TetStart, "TetStart"},
        {TetEnd, "TetEnd"},
        {TetNewPiece, "TetNewPiece"},
        {TetPlace, "TetPlace"},
        {TetRowClear, "TetRowClear"}
    };

    if (enumMap.contains(t)) {
        return enumMap.value(t);
    } else {
        return "Unknown";
    }
}

#ifndef BLUETOOTHMANAGER_H
#define BLUETOOTHMANAGER_H

#include <QObject>
#include <QBluetoothSocket>
#include <QBluetoothDeviceDiscoveryAgent>
#include <QtBluetooth/qbluetoothhostinfo.h>
#include <QtBluetooth/qbluetoothlocaldevice.h>
#include <QtBluetooth/qbluetoothserver.h>

#define CONTROLLER_MAC "0C:8B:95:94:B4:96"

class BluetoothManager : public QObject
{
    Q_OBJECT

public:
    explicit BluetoothManager(QObject *parent = nullptr);
    void startScan(); // Function to start scanning for Bluetooth devices
    void sendData(const QByteArray &data);

private slots:
    void deviceDiscovered(const QBluetoothDeviceInfo &deviceInfo); // Slot to handle device discovery
    void clientConnected();
    void socketConnected();
    void socketDisconnected();
    void socketError(QBluetoothSocket::SocketError error);
    void readyRead(); // Slot to handle incoming data

signals:
    void deviceDiscoveredSignal(const QString &deviceName); // Signal emitted when a Bluetooth device is discovered
    void dataReceived(const QByteArray &data); // Signal emitted when data is received

private:
    QBluetoothServer *rfcommServer = nullptr;
    QBluetoothSocket *socket;
    QBluetoothDeviceDiscoveryAgent *discoveryAgent;
    QBluetoothServiceInfo serviceInfo;
    QList<QBluetoothHostInfo> localAdapters;
    QBluetoothAddress localAdapter;
};

#endif // BLUETOOTHMANAGER_H

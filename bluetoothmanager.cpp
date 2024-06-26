#include "bluetoothmanager.h"

// Stimulation logfile name (received from controller)
QString fileName = "/received_log.csv";

BluetoothManager::BluetoothManager(QObject *parent) : QObject(parent) {
    // Delete old received file
    QString filePath = QDir::currentPath() + fileName;
    QFile file(filePath);
    if (file.exists()) {
        if (!file.remove()) {
            qDebug() << "Error removing existing log file";
            // You can choose to handle this error condition as needed
        }
    }

    localAdapters = QBluetoothLocalDevice::allDevices();
    // Output each QBluetoothHostInfo object in the list to the debug stream
    for (const auto& hostInfo : localAdapters) {
        qDebug() << "Bluetooth Adapter Address: " << hostInfo.address().toString();
        qDebug() << "Bluetooth Adapter Name: " << hostInfo.name();
    }

    // make discoverable
    if (!localAdapters.isEmpty()) {
        QBluetoothLocalDevice adapter(localAdapters.at(0).address());
        localAdapter = localAdapters.at(0).address();
        adapter.setHostMode(QBluetoothLocalDevice::HostDiscoverable);

    } else {
        qWarning("Local adapter is not found! The application might work incorrectly.");
    }

    discoveryAgent = new QBluetoothDeviceDiscoveryAgent(this);
    // Connect deviceDiscovered signal to the slot
    connect(discoveryAgent, SIGNAL(deviceDiscovered(QBluetoothDeviceInfo)),
            this, SLOT(deviceDiscovered(QBluetoothDeviceInfo)));

    //! [Create the server]
    rfcommServer = new QBluetoothServer(QBluetoothServiceInfo::RfcommProtocol, this);
    connect(rfcommServer, &QBluetoothServer::newConnection,
            this, &BluetoothManager::clientConnected);
    bool result = rfcommServer->listen(localAdapter);
    if (!result) {
        qWarning() << "Cannot bind chat server to" << localAdapter.toString();
        return;
    }
    //! [Create the server]

    socket = new QBluetoothSocket(QBluetoothServiceInfo::RfcommProtocol);
    connect(socket, &QBluetoothSocket::connected, this, &BluetoothManager::socketConnected);
    connect(socket, &QBluetoothSocket::disconnected, this, &BluetoothManager::socketDisconnected);
    //connect(socket, SIGNAL(error(QBluetoothSocket::SocketError)), this, SLOT(socketError(QBluetoothSocket::SocketError)));

    //! [Service name, description and provider]
    serviceInfo.setAttribute(QBluetoothServiceInfo::ServiceName, tr("Bt Test"));
    serviceInfo.setAttribute(QBluetoothServiceInfo::ServiceDescription,
                             tr("Example bluetooth please work"));
    serviceInfo.setAttribute(QBluetoothServiceInfo::ServiceProvider, tr("ACES"));
    //! [Service name, description and provider]

    //! [Service UUID set]
    QString Uuid = QStringLiteral("00001101-0000-1000-8000-00805F9B34FB");
    QBluetoothUuid serviceUuid(Uuid);
    serviceInfo.setServiceUuid(QBluetoothUuid(serviceUuid));
    //! [Service UUID set]

    //! [Service Discoverability]
    const auto groupUuid = QBluetoothUuid(QBluetoothUuid::ServiceClassUuid::PublicBrowseGroup);
    QBluetoothServiceInfo::Sequence publicBrowse;
    publicBrowse << QVariant::fromValue(groupUuid);
    serviceInfo.setAttribute(QBluetoothServiceInfo::BrowseGroupList, publicBrowse);
    //! [Service Discoverability]

    QBluetoothServiceInfo::Sequence protocolDescriptorList;
    QBluetoothServiceInfo::Sequence protocol;
    protocol << QVariant::fromValue(QBluetoothUuid(QBluetoothUuid::ProtocolUuid::L2cap));
    protocolDescriptorList.append(QVariant::fromValue(protocol));
    protocol.clear();
    protocol << QVariant::fromValue(QBluetoothUuid(QBluetoothUuid::ProtocolUuid::Rfcomm))
             << QVariant::fromValue(quint8(rfcommServer->serverPort()));
    protocolDescriptorList.append(QVariant::fromValue(protocol));
    serviceInfo.setAttribute(QBluetoothServiceInfo::ProtocolDescriptorList,
                             protocolDescriptorList);

    serviceInfo.registerService(localAdapter);
    startScan();
}

void BluetoothManager::clientConnected()
{
    socket = rfcommServer->nextPendingConnection();
    if (!socket)
        return;

    connect(socket, &QBluetoothSocket::readyRead, this, &BluetoothManager::readyRead);
    //connect(socket, &QBluetoothSocket::disconnected,
//            this, QOverload<>::of(&ChatServer::clientDisconnected));
//    clientSockets.append(socket);
//    clientNames[socket] = socket->peerName();
//    emit clientConnected(socket->peerName());
    sendData("hi");
    qDebug("what up it works");
}

void BluetoothManager::startScan()
{
    // Start scanning for Bluetooth devices
    discoveryAgent->start();
}

void BluetoothManager::deviceDiscovered(const QBluetoothDeviceInfo &device)
{
    qInfo() << "New Device Found: " << device.name() << " ( " << device.address().toString() << " )";
    //if (device.name() == "CC_Classic")
    if (device.address().toString() == CONTROLLER_MAC)
    {
        discoveryAgent->stop();
        QString sppUuid = QStringLiteral("00001101-0000-1000-8000-00805F9B34FB");
        QBluetoothUuid uuid(sppUuid);
        socket->connectToService(device.address(), uuid, QIODeviceBase::ReadWrite);
        qDebug() << socket->state();
    }
}

void BluetoothManager::sendData(const QByteArray &data) {
    socket->write(data);
}

void BluetoothManager::socketConnected() {
    qDebug() << "Connected to device.";
    socket->write("Connected\n");
    connect(socket, &QBluetoothSocket::readyRead, this, &BluetoothManager::readyRead);
}

void BluetoothManager::socketDisconnected() {
    qDebug() << "Disconnected from device.";

}

void BluetoothManager::socketError(QBluetoothSocket::SocketError error) {
    qDebug() << "Socket error:" << error;

}

void BluetoothManager::readyRead() {
    // Read incoming data from the socket
    QByteArray data = socket->readAll();

    QByteArray fileData = data;
    saveFile(fileData);
    //The below code is for if you want to receive other bluetooth messages beyond a file (doesn't quite work)
//    if (data.startsWith("ACES:")) {
//        // If the incoming data indicates a file transmission
//        // Extract the file data
//        qDebug() << "file detected";
//        QByteArray fileData = data.mid(5); // Skip "ACES:" prefix
//        saveFile(fileData);
//    } else {
//        qDebug() << data;
//        emit dataReceived(data);
//    }
}

void BluetoothManager::saveFile(const QByteArray &fileData)
{
    QString filePath = QDir::currentPath() + fileName; // Define the file path
    QFile file(filePath);

    if (file.open(QIODevice::WriteOnly | QIODevice::Append)) {
        // Open the file in write mode
        qint64 bytesWritten = file.write(fileData); // Write the file data

        if (bytesWritten == -1) {
            // Error handling if writing fails
            qDebug() << "Error writing to file:" << file.errorString();
        }

        file.close(); // Close the file
    } else {
        // Error handling if file opening fails
        qDebug() << "Error opening file for writing:" << file.errorString();
    }
}

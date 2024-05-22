#include "bluetoothdevice.h"

BluetoothDevice::BluetoothDevice(QObject *parent) : QObject(parent)
{
    m_socket = new QBluetoothSocket(QBluetoothServiceInfo::RfcommProtocol);

    QObject::connect(m_socket, &QBluetoothSocket::connected, this, &BluetoothDevice::onConnected);
    QObject::connect(m_socket, &QBluetoothSocket::disconnected, this, &BluetoothDevice::onDisconnected);
    QObject::connect(m_socket, &QBluetoothSocket::stateChanged, this, &BluetoothDevice::onStateChanged);
    QObject::connect(m_socket, QOverload<QBluetoothSocket::SocketError>::of(&QBluetoothSocket::error), this, &BluetoothDevice::onErrorOccured);
}

BluetoothDevice::~BluetoothDevice()
{
    if (m_socket != nullptr)
    {
        m_socket->close();
        delete m_socket;
        m_socket = nullptr;
    }
}

QString BluetoothDevice::name()
{    
    return m_selectedDevice.name();
}

BluetoothDevice::Status BluetoothDevice::status()
{
    Status status = Status::Disconnected;

    switch (m_socket->state())
    {
    case QBluetoothSocket::UnconnectedState:
    case QBluetoothSocket::BoundState: status = Status::Disconnected; break;

    case QBluetoothSocket::ServiceLookupState:
    case QBluetoothSocket::ConnectingState: status = Status::Connecting; break;

    case QBluetoothSocket::ConnectedState:
    case QBluetoothSocket::ListeningState:
    case QBluetoothSocket::ClosingState: status = Status::Connected; break;
    }

    return status;
}

void BluetoothDevice::setSelectedDevice(const QBluetoothDeviceInfo deviceInfo)
{
    if (deviceInfo.isValid())
    {
        m_selectedDevice = deviceInfo;
        connectToSelectedDevice();
    }
}

void BluetoothDevice::reconnect()
{
    connectToSelectedDevice();
}

void BluetoothDevice::onConnected()
{

}

void BluetoothDevice::onDisconnected()
{
    connectToSelectedDevice();
}

void BluetoothDevice::onStateChanged(QBluetoothSocket::SocketState state)
{
    emit statusChanged();
}

void BluetoothDevice::onErrorOccured(QBluetoothSocket::SocketError error)
{
    if (m_selectedDevice.isValid())
    {
        QTimer::singleShot(1000, this, SLOT(reconnect()));
    }
}

void BluetoothDevice::sendData(QString data)
{
    qDebug() << data;
    if (m_socket->isOpen()) {
        m_socket->write(data.toUtf8());
    }
}

void BluetoothDevice::connectToSelectedDevice()
{
    if (m_selectedDevice.isValid())
    {
        m_socket->connectToService(m_selectedDevice.address(), QBluetoothUuid::SerialPort);
    }
}



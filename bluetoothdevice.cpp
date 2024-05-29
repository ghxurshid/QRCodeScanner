#include "bluetoothdevice.h"

BluetoothDevice::BluetoothDevice(QObject *parent) : QObject(parent)
{
    m_socket = new QBluetoothSocket(QBluetoothServiceInfo::RfcommProtocol);

    QObject::connect(m_socket, &QBluetoothSocket::connected, this, &BluetoothDevice::onConnected);
    QObject::connect(m_socket, &QBluetoothSocket::disconnected, this, &BluetoothDevice::onDisconnected);
    QObject::connect(m_socket, &QBluetoothSocket::stateChanged, this, &BluetoothDevice::onStateChanged);
    QObject::connect(m_socket, &QBluetoothSocket::readyRead, this, &BluetoothDevice::onDataReceived);
    QObject::connect(m_socket, QOverload<QBluetoothSocket::SocketError>::of(&QBluetoothSocket::error), this, &BluetoothDevice::onErrorOccured);

    QTimer::singleShot(1000, this, [this](){
        QString text = "*1-11,1-12,2-34,2-45,3-17,4-26,#";
        emit dataReceived(text);
    });
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
    Q_UNUSED(state)
    emit statusChanged();
}

void BluetoothDevice::onErrorOccured(QBluetoothSocket::SocketError error)
{
    Q_UNUSED(error)
    if (m_selectedDevice.isValid())
    {
        QTimer::singleShot(1000, this, SLOT(reconnect()));
    }
}

void BluetoothDevice::sendData(QString data)
{
    qDebug() << data;
    if (m_socket && m_socket->isOpen()) {
        auto packet = data.toUtf8();
        packet.push_back('\r');
        packet.push_back('\n');
        m_socket->write(packet);
    }
}

void BluetoothDevice::onDataReceived()
{
    if (m_socket && m_socket->isOpen()) {
        auto data = m_socket->readAll();
        emit dataReceived(QString(data));
    }
}

void BluetoothDevice::connectToSelectedDevice()
{
    if (m_selectedDevice.isValid())
    {
        m_socket->connectToService(m_selectedDevice.address(), QBluetoothUuid::SerialPort);
    }
}



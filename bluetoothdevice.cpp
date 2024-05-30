#include "bluetoothdevice.h"

BluetoothDevice::BluetoothDevice(QObject *parent) : QObject(parent)
{
    m_socket = new QBluetoothSocket(QBluetoothServiceInfo::RfcommProtocol);

    QObject::connect(m_socket, &QBluetoothSocket::connected, this, &BluetoothDevice::onConnected);
    QObject::connect(m_socket, &QBluetoothSocket::disconnected, this, &BluetoothDevice::onDisconnected);
    QObject::connect(m_socket, &QBluetoothSocket::stateChanged, this, &BluetoothDevice::onStateChanged);
    QObject::connect(m_socket, &QBluetoothSocket::readyRead, this, &BluetoothDevice::onDataReceived);
    QObject::connect(m_socket, QOverload<QBluetoothSocket::SocketError>::of(&QBluetoothSocket::error), this, &BluetoothDevice::onErrorOccured);

//    QTimer::singleShot(1000, this, [this](){
//        QString text = "*1-11,";
//        emit dataReceived(text);
//    });

//    QTimer::singleShot(1200, this, [this](){
//        QString text = "1-12,2-34,2-45,3";
//        emit dataReceived(text);
//    });

//    QTimer::singleShot(1400, this, [this](){
//        QString text = "-17,4-26*";
//        emit dataReceived(text);
//    });


//    QTimer::singleShot(1600, this, [this](){
//        QString text = "*1-11,";
//        emit dataReceived(text);
//    });

//    QTimer::singleShot(1800, this, [this](){
//        QString text = "1-12,2-34,2-45,3";
//        emit dataReceived(text);
//    });

//    QTimer::singleShot(2000, this, [this](){
//        QString text = "-17,4-26#";
//        emit dataReceived(text);
//    });

    QTimer::singleShot(2000, this, [this](){
        QString text = "*1-11,1-12,2-34,2-45,3-17,4-26,4-33,4-57,4-75,5-00,5-36,5-44,5-73,6-51,6-61,6-65,6-73,7-41,7-53,7-71,9-81,9-82,9-91,7-58,3-39,5-42,8-68,2-79,4-94,6-07,8-55,1-47,3-62,5-83,7-90,2-16,4-28,6-34,9-51,1-73,3-89,5-02,7-15,8-26,9-31,4-48,6-54,7-65,9-87,2-93#";
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



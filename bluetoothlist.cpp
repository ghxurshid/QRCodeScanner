#include "bluetoothlist.h"

BluetoothList::BluetoothList(QObject *parent) : QAbstractListModel (parent)
{
    QObject::connect(&m_agent, &QBluetoothDeviceDiscoveryAgent::canceled, this, &BluetoothList::onCancelled);
    QObject::connect(&m_agent, &QBluetoothDeviceDiscoveryAgent::deviceDiscovered, this, &BluetoothList::onDeviceDiscovered);
    QObject::connect(&m_agent, &QBluetoothDeviceDiscoveryAgent::deviceUpdated, this, &BluetoothList::onDeviceUpdated);
    QObject::connect(&m_agent, &QBluetoothDeviceDiscoveryAgent::finished, this, &BluetoothList::onFinished);

    m_agent.start();
}

int BluetoothList::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return m_agent.discoveredDevices().count();
}

QVariant BluetoothList::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    auto devices = m_agent.discoveredDevices();

    if (index.row() < 0 || index.row() >= devices.count())
        return QVariant();

    auto device = devices[index.row()];

    if (role == Name)
        return device.name();

    if (role == Address)
        return device.address().toString();

    if (role == Rssi)
        return device.rssi();

    if (role == IsValid)
        return device.isValid();

    return QVariant();
}

void BluetoothList::onCancelled()
{

}

void BluetoothList::onDeviceDiscovered(const QBluetoothDeviceInfo &deviceInfo)
{

}

void BluetoothList::onDeviceUpdated(const QBluetoothDeviceInfo &deviceInfo, QBluetoothDeviceInfo::Fields updatedFields)
{

}

void BluetoothList::onError(QBluetoothDeviceDiscoveryAgent::Error error)
{

}

void BluetoothList::onFinished()
{

}

QHash<int, QByteArray> BluetoothList::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[Name] = "name";
    roles[Address] = "address";
    roles[Rssi] = "rssi";
    roles[IsValid] = "isValid";
    return roles;
}

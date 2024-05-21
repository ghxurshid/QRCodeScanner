#include "bluetoothlist.h"

BluetoothList::BluetoothList(QObject *parent) : QAbstractListModel (parent)
{
    QObject::connect(&m_agent, &QBluetoothDeviceDiscoveryAgent::canceled, this, &BluetoothList::onCancelled);
    QObject::connect(&m_agent, &QBluetoothDeviceDiscoveryAgent::deviceDiscovered, this, &BluetoothList::onDeviceDiscovered);
    QObject::connect(&m_agent, &QBluetoothDeviceDiscoveryAgent::deviceUpdated, this, &BluetoothList::onDeviceUpdated);
    QObject::connect(&m_agent, &QBluetoothDeviceDiscoveryAgent::finished, this, &BluetoothList::onFinished);

    if (QSysInfo::productType() == "windows")
     {
        m_devices.append(QBluetoothDeviceInfo(QBluetoothAddress("00:11:22:33:44:55"), QString("Device Alpha"), 5));
        m_devices.append(QBluetoothDeviceInfo(QBluetoothAddress("55:44:33:22:11:00"), QString("Device Beta"), 10));
        m_devices.append(QBluetoothDeviceInfo(QBluetoothAddress("11:22:33:44:55:66"), QString("Device Gamma"), 15));
        m_devices.append(QBluetoothDeviceInfo(QBluetoothAddress("22:33:44:55:66:77"), QString("Device Delta"), 20));
        m_devices.append(QBluetoothDeviceInfo(QBluetoothAddress("33:44:55:66:77:88"), QString("Device Epsilon"), 25));
        m_devices.append(QBluetoothDeviceInfo(QBluetoothAddress("44:55:66:77:88:99"), QString("Device Zeta"), 30));
        m_devices.append(QBluetoothDeviceInfo(QBluetoothAddress("55:66:77:88:99:00"), QString("Device Eta"), 35));
        m_devices.append(QBluetoothDeviceInfo(QBluetoothAddress("66:77:88:99:00:11"), QString("Device Theta"), 40));
        m_devices.append(QBluetoothDeviceInfo(QBluetoothAddress("77:88:99:00:11:22"), QString("Device Iota"), 45));
        m_devices.append(QBluetoothDeviceInfo(QBluetoothAddress("88:99:00:11:22:33"), QString("Device Kappa"), 50));
        m_devices.append(QBluetoothDeviceInfo(QBluetoothAddress("99:00:11:22:33:44"), QString("Device Lambda"), 55));
        m_devices.append(QBluetoothDeviceInfo(QBluetoothAddress("AA:BB:CC:DD:EE:FF"), QString("Device Mu"), 60));
        m_devices.append(QBluetoothDeviceInfo(QBluetoothAddress("BB:CC:DD:EE:FF:AA"), QString("Device Nu"), 65));
        m_devices.append(QBluetoothDeviceInfo(QBluetoothAddress("CC:DD:EE:FF:AA:BB"), QString("Device Xi"), 70));
        m_devices.append(QBluetoothDeviceInfo(QBluetoothAddress("DD:EE:FF:AA:BB:CC"), QString("Device Omicron"), 75));
     }

    m_agent.start();
}

int BluetoothList::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return m_devices.count();
}

QVariant BluetoothList::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    auto devices = m_agent.discoveredDevices();

    if (index.row() < 0 || index.row() >= m_devices.count())
        return QVariant();

    auto device = m_devices[index.row()];

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
    if (deviceInfo.isValid())
    {
        beginInsertRows(QModelIndex(), m_devices.count(), m_devices.count());
        m_devices.append(deviceInfo);
        endInsertRows();
    }
}

void BluetoothList::onDeviceUpdated(const QBluetoothDeviceInfo &deviceInfo, QBluetoothDeviceInfo::Fields updatedFields)
{
    if (deviceInfo.isValid())
    {
        for (int i = 0; i < m_devices.count(); i ++)
        {
            if (m_devices[i].address().toUInt64() == deviceInfo.address().toUInt64())
            {
                m_devices.replace(i, deviceInfo);
                emit dataChanged(createIndex(i, 0), createIndex(i, 0));
                break;
            }
        }
    }
}

void BluetoothList::onError(QBluetoothDeviceDiscoveryAgent::Error error)
{
    Q_UNUSED(error)
}

void BluetoothList::onFinished()
{

}

void BluetoothList::select(int index)
{
    if (index < m_devices.count())
    {
        auto device = m_devices[index];
        emit deviceSelected (device);
    }
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

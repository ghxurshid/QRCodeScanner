#ifndef BLUETOOTHLIST_H
#define BLUETOOTHLIST_H

#include <QObject>
#include <QAbstractListModel>
#include <QtBluetooth/QBluetoothDeviceDiscoveryAgent>
#include <QtBluetooth/QBluetoothLocalDevice>
#include <QtBluetooth/QBluetoothDeviceInfo>

class BluetoothList : public QAbstractListModel
{
    Q_OBJECT
public:
    BluetoothList(QObject *parent = nullptr);

    enum MyRoles {
        Name = Qt::UserRole + 1,
        Address = Qt::UserRole + 2,
        Rssi = Qt::UserRole + 3,
        IsValid = Qt::UserRole + 4
    };

    // Методы, необходимые для реализации абстрактной модели данных
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role) const override;

signals:
    void deviceSelected(const QBluetoothDeviceInfo &deviceInfo);

public slots:
    void onCancelled();
    void onDeviceDiscovered(const QBluetoothDeviceInfo &deviceInfo);
    void onDeviceUpdated(const QBluetoothDeviceInfo &deviceInfo, QBluetoothDeviceInfo::Fields updatedFields);
    void onError(QBluetoothDeviceDiscoveryAgent::Error error);
    void onFinished();

    void select(int index);

protected:
    QHash<int, QByteArray> roleNames() const override;

private:
    QBluetoothDeviceDiscoveryAgent m_agent;
    QVector<QBluetoothDeviceInfo> m_devices;
};

#endif // BLUETOOTHLIST_H

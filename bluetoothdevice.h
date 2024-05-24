#ifndef BLUETOOTHDEVICE_H
#define BLUETOOTHDEVICE_H

#include <QObject>
#include <QTimer>
#include <QBluetoothSocket>
#include <QBluetoothDeviceInfo>
#include <QBluetoothLocalDevice>
#include <QBluetoothServiceInfo>

class BluetoothDevice : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString name READ name NOTIFY nameChanged)
    Q_PROPERTY(Status status READ status NOTIFY statusChanged)
public:
    enum Status {
        Connected,
        Connecting,
        Disconnected
    };
    Q_ENUM(Status)

    explicit BluetoothDevice(QObject *parent = nullptr);
    ~BluetoothDevice();

    QString name();
    Status status();

signals:
    void nameChanged();
    void statusChanged();
    void errorOccured(QString message);

public slots:
    void setSelectedDevice(const QBluetoothDeviceInfo deviceInfo);
    void reconnect();

    void onConnected();
    void onDisconnected();
    void onStateChanged(QBluetoothSocket::SocketState state);
    void onErrorOccured(QBluetoothSocket::SocketError error);

    void sendData(QString data);

protected:
    void connectToSelectedDevice();  

private:
    int m_status;
    QBluetoothSocket* m_socket;
    QBluetoothDeviceInfo m_selectedDevice;
    QTimer timer;
};

#endif // BLUETOOTHDEVICE_H

#ifndef BLUETOOTHDEVICE_H
#define BLUETOOTHDEVICE_H

#include <QObject>

class BluetoothDevice : public QObject
{
    Q_OBJECT
public:
    explicit BluetoothDevice(QObject *parent = nullptr);

signals:

public slots:
};

#endif // BLUETOOTHDEVICE_H
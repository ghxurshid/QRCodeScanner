#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QDebug>
#include <QQmlContext>

#include <Qt>
#include "QZXing.h"
#include <QBluetoothDeviceInfo>

#include "bluetoothlist.h"
#include "bluetoothdevice.h"
#include "activitymanager.h"

int main(int argc, char *argv[])
{
    qmlRegisterType<BluetoothList>("Bluetooth", 1, 0, "BluetoothList");
    qmlRegisterType<BluetoothDevice>("Bluetooth", 1, 0, "BluetoothDevice");
    qmlRegisterType<ActivityManager>("Bluetooth", 1, 0, "ActivityManager");

    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QGuiApplication app(argc, argv);
    QQmlApplicationEngine engine;
    QZXing::registerQMLTypes();
    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));
    if (engine.rootObjects().isEmpty())
        return -1;

    return app.exec();
}

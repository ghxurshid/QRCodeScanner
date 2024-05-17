#include <QGuiApplication>
#include <QQmlApplicationEngine>

#include "qrcodedecoder.h"
#include "videofilter.h"

int main(int argc, char *argv[])
{
    //Registering custom qml types
    qmlRegisterType<QRCodeDecoder>("QRCodeDecoder", 1, 0, "QRCodeDecoder");
    qmlRegisterType<VideoFilter>("CustomFilters", 1, 0, "VideoFilter");

    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    QGuiApplication app(argc, argv);

    QQmlApplicationEngine engine;

    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));
    if (engine.rootObjects().isEmpty())
        return -1;

    return app.exec();
}

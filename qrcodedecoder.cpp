#include "qrcodedecoder.h"
#include <QQmlEngine>
#include <QQmlContext>
#include <QQuickImageProvider>
#include <QDebug>
#include <QTime>

QRCodeDecoder::QRCodeDecoder(QObject *parent) : QZXing(parent)
{
    connect(this, &QZXing::error, [](QString err) {
        qDebug() << err;
    });

    connect(this, &QZXing::decodingStarted, [this]() {
        qDebug() << "\nDecoding is started ...";
        busy = true;

    });

    connect(this, &QZXing::decodingFinished, [this](bool status) {
        qDebug() << "Decode finished: " << status << ", elapsed time: " << getProcessTimeOfLastDecoding() << " ms";
        busy = false;
    });

    setDecoder( QZXing::DecoderFormat_QR_CODE );
    setSourceFilterType(QZXing::SourceFilter_ImageNormal);
    setTryHarderBehaviour(QZXing::TryHarderBehaviour_ThoroughScanning | QZXing::TryHarderBehaviour_Rotate);

    busy = false;
}
static QTime lastTime;
void QRCodeDecoder::processImage(const QImage &image)
{
    auto currentTime = QTime::currentTime();

    if (!busy)
    {
        //    emit decodeFinished(result);
        QString result = decodeImage(image);
    }

    qDebug() << lastTime.msecsTo(currentTime);
    lastTime = currentTime;
}

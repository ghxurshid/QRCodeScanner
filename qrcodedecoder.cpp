#include "qrcodedecoder.h"
#include <QQmlEngine>
#include <QQmlContext>
#include <QQuickImageProvider>
#include <QDebug>

QRCodeDecoder::QRCodeDecoder(QObject *parent) : QZXing(parent)
{
    connect(this, &QZXing::error, [](QString err) {
        //qDebug() << err;
    });

    connect(this, &QZXing::decodingStarted, []() {
        //qDebug() << "Decoding is started ...";
    });

    connect(this, &QZXing::decodingFinished, [](bool status) {
    });

    setDecoder( QZXing::DecoderFormat_QR_CODE );
    setSourceFilterType(QZXing::SourceFilter_ImageNormal);
    setTryHarderBehaviour(QZXing::TryHarderBehaviour_ThoroughScanning | QZXing::TryHarderBehaviour_Rotate);
}

void QRCodeDecoder::processImage(const QImage &image)
{
    QString result = decodeImage(image);
    emit decodeFinished(result);
}

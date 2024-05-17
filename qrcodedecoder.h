#ifndef IMAGEPROCESSOR_H
#define IMAGEPROCESSOR_H

#include <QObject>
#include <QZXing.h>

class QRCodeDecoder : public QZXing
{
    Q_OBJECT
public:
    explicit QRCodeDecoder(QObject *parent = nullptr);

signals:
    void decodeFinished(QString string);

public slots:
    void processImage(const QImage &image);
};

#endif // IMAGEPROCESSOR_H

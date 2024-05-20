#ifndef IMAGEPROCESSOR_H
#define IMAGEPROCESSOR_H

#include <QObject>
#include <QZXing.h>
#include <QTime>

class QRCodeDecoder : public QZXing
{
    Q_OBJECT
public:
    explicit QRCodeDecoder(QObject *parent = nullptr);

signals:
    void decodeFinished(QString string);

public slots:
    void processImage(const QImage &image);

private:
    bool busy;
};

#endif // IMAGEPROCESSOR_H

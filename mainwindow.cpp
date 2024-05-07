#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDebug>
#include <QCamera>
#include <QVideoProbe>
#include <QCameraViewfinder>
#include <QGraphicsPixmapItem>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    //Decoder configuration
    decoder.connect(&decoder, &QZXing::error, [](QString err) {
        //qDebug() << err;
    });

    decoder.connect(&decoder, &QZXing::decodingStarted, []() {
        //qDebug() << "Decoding is started ...";
    });

    decoder.connect(&decoder, &QZXing::decodingFinished, [](bool status) {
        if (status) {
            qDebug() << "Decoding finished with status: " << status;
        }

    });

    decoder.setDecoder( QZXing::DecoderFormat_QR_CODE | QZXing::DecoderFormat_EAN_13 );

    decoder.setSourceFilterType(QZXing::SourceFilter_ImageNormal | QZXing::SourceFilter_ImageInverted);
    decoder.setSourceFilterType(QZXing::SourceFilter_ImageNormal);
    decoder.setTryHarderBehaviour(QZXing::TryHarderBehaviour_ThoroughScanning | QZXing::TryHarderBehaviour_Rotate);

    //Camera configuration
    QCamera *camera = new QCamera;
    QCameraViewfinder *viewfinder = new QCameraViewfinder(this);
    viewfinder->setFixedSize(640, 480);

    camera->setViewfinder(viewfinder);

    camera->setCaptureMode(QCamera::CaptureVideo);

    QVideoProbe *videoProbe = new QVideoProbe(this);

    if (videoProbe->setSource(camera)) {
        // Probing succeeded, videoProbe->isValid() should be true.
        connect(videoProbe, SIGNAL(videoFrameProbed(QVideoFrame)),
                this, SLOT(detectBarcodes(QVideoFrame)));
    }

    camera->start();
}

MainWindow::~MainWindow()
{
    delete ui;
}

static bool busy = false;
void MainWindow::detectBarcodes(const QVideoFrame &frame)
{
    if (busy) return;

    busy = true;
    QVideoFrame cloneFrame(frame);
    cloneFrame.map(QAbstractVideoBuffer::ReadOnly);

    const QImage::Format imageFormat = QVideoFrame::imageFormatFromPixelFormat(cloneFrame.pixelFormat());
    QImage image;

    if (imageFormat != QImage::Format_Invalid) {
        image = QImage(cloneFrame.bits(),
                       cloneFrame.width(),
                       cloneFrame.height(),
                       cloneFrame.bytesPerLine(),
                       imageFormat);

        QGraphicsScene scene;
        ui->graphicsView->setScene(&scene);
        QPixmap pixmap = QPixmap::fromImage(image);
        QGraphicsPixmapItem *pixmapItem = new QGraphicsPixmapItem(pixmap);
        scene.addItem(pixmapItem);

        QString result = decoder.decodeImage(image);
        qDebug() << result;
    } else {
        // Формат не поддерживается напрямую, требуется преобразование
        // Здесь можно использовать конвертеры, например, для YUV -> RGB
    }

    cloneFrame.unmap();
    busy = false;
}

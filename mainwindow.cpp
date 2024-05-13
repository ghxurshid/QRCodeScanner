#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDebug>
#include <QCamera>
#include <QDialog>
#include <QLineEdit>
#include <QVideoProbe>
#include <QFormLayout>
#include <QListWidget>
#include <QDialogButtonBox>
#include <QCameraViewfinder>
#include <QGraphicsPixmapItem>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // - - - - - - - - Decoder configuration - - - - - - - - - - - - - - -
    decoder.connect(&decoder, &QZXing::error, [](QString err) {
        //qDebug() << err;
    });

    decoder.connect(&decoder, &QZXing::decodingStarted, []() {
        //qDebug() << "Decoding is started ...";
    });

    decoder.connect(&decoder, &QZXing::decodingFinished, [](bool status) {
    });

    decoder.setDecoder( QZXing::DecoderFormat_QR_CODE );
    decoder.setSourceFilterType(QZXing::SourceFilter_ImageNormal);
    decoder.setTryHarderBehaviour(QZXing::TryHarderBehaviour_ThoroughScanning | QZXing::TryHarderBehaviour_Rotate);
    qDebug() <<"getEnabledFormats" << decoder.getEnabledFormats();
    qDebug() <<"getTryHarder" << decoder.getTryHarder();
    qDebug() <<"getTryHarderBehaviour" << decoder.getTryHarderBehaviour();
    qDebug() <<"getSourceFilterType" << decoder.getSourceFilterType();
    qDebug() <<"getAllowedExtensions" << decoder.getAllowedExtensions();
    qDebug() <<"foundedFormat" << decoder.foundedFormat();
    qDebug() <<"charSet" << decoder.charSet();


    // - - - - - - - Camera configuration - - - - - - - - - - - - - - - -
    QCamera *camera = new QCamera;
//    QCameraViewfinder *viewfinder = new QCameraViewfinder(this);


//    int _width = this->width() - 60;
//    viewfinder->setGeometry(30, 0, _width, static_cast<int>(_width * 0.75));
//    camera->setViewfinder(viewfinder);
//    camera->setCaptureMode(QCamera::CaptureVideo);

//    QCameraViewfinderSettings viewfinderSettings;

//    camera->setViewfinderSettings(viewfinderSettings);

//    QVideoProbe *videoProbe = new QVideoProbe(this);

//    if (videoProbe->setSource(camera)) {
//        // Probing succeeded, videoProbe->isValid() should be true.
//        connect(videoProbe, SIGNAL(videoFrameProbed(QVideoFrame)),
//                this, SLOT(detectBarcodes(QVideoFrame)));
//    }

    //camera->start();
}

MainWindow::~MainWindow()
{
    delete ui;
}

static bool busy = false;
static int  skip = 0;
void MainWindow::detectBarcodes(const QVideoFrame &frame)
{
    if (busy) return;

    if (skip ++ < 8) return;

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

        QString result = decoder.decodeImage(image);
        if (result.size() > 0) {
            ui->label->setText(result);
        }
    }

    cloneFrame.unmap();
    busy = false;
    skip = 0;
}

void MainWindow::on_pushButton_clicked()
{qDebug() << "PushButton";
    QDialog dialog(this);
    QFormLayout form(&dialog);
    // Add some text above the fields
    form.addRow(new QLabel("Select Wi-Fi"));

    //List of wi-fi devices
    QListWidget* listView = new QListWidget(&dialog);
    listView->resize(200, 150);

    for(int i = 0 ; i < 10; i ++)
        listView->addItem(QString(i));

    form.addRow(listView);

    //Edit area for password
    QLineEdit *lineEdit = new QLineEdit(&dialog);

    QString label = QString("Pass: ");
    form.addRow(label, lineEdit);

    // Add some standard buttons (Cancel/Ok) at the bottom of the dialog
    QDialogButtonBox buttonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel,
                               Qt::Horizontal, &dialog);
    form.addRow(&buttonBox);

    QObject::connect(&buttonBox, SIGNAL(accepted()), &dialog, SLOT(accept()));
    QObject::connect(&buttonBox, SIGNAL(rejected()), &dialog, SLOT(reject()));

    // Show the dialog as modal
    if (dialog.exec() == QDialog::Accepted)
    {
        // If the user didn't dismiss the dialog, do something with the fields

        int idx = listView->currentRow();
    }
}

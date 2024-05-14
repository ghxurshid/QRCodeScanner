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
#include <QBluetoothDeviceDiscoveryAgent>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // - - - - - - Bluetooth socket configuration - - - - - - - - - - - -
    socket = new QBluetoothSocket(QBluetoothServiceInfo::RfcommProtocol);

    QObject::connect(socket, &QBluetoothSocket::connected, [this]() {
        this->ui->pushButton->setStyleSheet("background-color: rgb(0, 255, 0);");
    });

    QObject::connect(socket, &QBluetoothSocket::disconnected, [this]() {
        this->ui->pushButton->setStyleSheet("");
    });

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
    QCameraViewfinder *viewfinder = new QCameraViewfinder(ui->frame);

    viewfinder->setGeometry(QRect(QPoint(0, 0), QPoint(ui->frame->width(), ui->frame->height())));
    camera->setViewfinder(viewfinder);

    qDebug() << "viewfinder.geometry: " << viewfinder->geometry();


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

    if (imageFormat != QImage::Format_Invalid)
    {
        image = QImage(cloneFrame.bits(),
                       cloneFrame.width(),
                       cloneFrame.height(),
                       cloneFrame.bytesPerLine(),
                       imageFormat);

        QString result = decoder.decodeImage(image);

        if (result.size() > 0)
        {
            ui->label->setText(result);

            if (socket->isOpen())
            {
                socket->write(result.toUtf8());
            }
        }
    }

    cloneFrame.unmap();
    busy = false;
    skip = 0;
}

void MainWindow::on_pushButton_clicked()
{
    QDialog dialog(this);
    QFormLayout form(&dialog);
    // Add some text above the fields
    form.addRow(new QLabel("Select Bluetooth device"));

    //List of wi-fi devices
    QListWidget* listView = new QListWidget(&dialog);
    listView->resize(200, 150);

    QBluetoothDeviceDiscoveryAgent *discoveryAgent = new QBluetoothDeviceDiscoveryAgent(this);
    QObject::connect(discoveryAgent, &QBluetoothDeviceDiscoveryAgent::deviceDiscovered,
                     [listView](QBluetoothDeviceInfo deviceInfo) {
        listView->addItem(QString(deviceInfo.name()) + " " + QString(deviceInfo.rssi()));
    });

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
        discoveryAgent->stop();
        auto devices = discoveryAgent->discoveredDevices();

        auto device = devices[idx];

        if (socket != nullptr && socket->isOpen())
        {
            socket->disconnectFromService();
        }

        socket->connectToService(device.address(), QBluetoothUuid::SerialPort);
    }
}

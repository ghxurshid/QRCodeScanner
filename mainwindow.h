#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QVideoFrame>
#include <QBluetoothSocket>

#include <QZXing.h>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();


public slots:
    void detectBarcodes(const QVideoFrame &frame);

private slots:
    void on_pushButton_clicked();

private:
    Ui::MainWindow *ui;

    QZXing decoder;
    QBluetoothSocket* socket;

};

#endif // MAINWINDOW_H

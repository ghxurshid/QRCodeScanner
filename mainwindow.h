#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QVideoFrame>

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

private:
    Ui::MainWindow *ui;

    QZXing decoder;
};

#endif // MAINWINDOW_H

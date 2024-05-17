#include "videofilter.h"
#include <QtDebug>

VideoFilterRunnable::VideoFilterRunnable(VideoFilter *filter)
    : m_filter(filter)
{
}

QVideoFrame VideoFilterRunnable::run(QVideoFrame *input, const QVideoSurfaceFormat &surfaceFormat, RunFlags flags)
{
    if (!input || !input->isValid())
        return *input;

    input->map(QAbstractVideoBuffer::ReadOnly);
    QImage image(
        input->bits(),
        input->width(),
        input->height(),
        input->bytesPerLine(),
        QVideoFrame::imageFormatFromPixelFormat(input->pixelFormat())
    );
    input->unmap();

    qDebug() << image.rect();

    m_filter->processFrame(image);
    return *input;
}

VideoFilter::VideoFilter(QObject *parent)
    : QAbstractVideoFilter(parent)
{
}

QVideoFilterRunnable *VideoFilter::createFilterRunnable()
{
    return new VideoFilterRunnable(this);
}

void VideoFilter::processFrame(const QImage &image)
{
    // Здесь добавьте код для обработки изображения
    QImage processedImage = image; // Ваша логика обработки

    // Emit the signal with the processed image if needed
    emit newImageArrived(processedImage);
}
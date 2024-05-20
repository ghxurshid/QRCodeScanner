#ifndef VIDEOFILTER_H
#define VIDEOFILTER_H

#include <QObject>
#include <QAbstractVideoFilter>
#include <QVideoFilterRunnable>

class VideoFilter;

class VideoFilterRunnable : public QVideoFilterRunnable
{
public:
    VideoFilterRunnable(VideoFilter *filter);

    QVideoFrame run(QVideoFrame *input, const QVideoSurfaceFormat &surfaceFormat, RunFlags flags) override;

private:
    VideoFilter *m_filter;
};

// - - - - - - - - - - - - - - - - - - - - - - - - -

class VideoFilter : public QAbstractVideoFilter
{
    Q_OBJECT
public:
    explicit VideoFilter(QObject *parent = nullptr);

    QVideoFilterRunnable *createFilterRunnable() override;

signals:
    void newImageArrived(const QImage &image);

public slots:
    void processFrame(const QImage &image);
};

#endif // VIDEOFILTER_H

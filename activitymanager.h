#ifndef ACTIVITYMANAGER_H
#define ACTIVITYMANAGER_H

#include <QObject>

class ActivityManager : public QObject
{
    Q_OBJECT
public:
    explicit ActivityManager(QObject *parent = nullptr);

    Q_INVOKABLE void keepScreenOn(bool on);

signals:

public slots:
};

#endif // ACTIVITYMANAGER_H

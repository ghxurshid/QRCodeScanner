#include "activitymanager.h"
#include <QDebug>
#ifdef ANDROID
#include <QtAndroid>
#include <QtAndroidExtras/QAndroidJniObject>
#include <QtAndroidExtras/QAndroidJniEnvironment>
#endif

ActivityManager::ActivityManager(QObject *parent) : QObject(parent)
{

}

void ActivityManager::keepScreenOn(bool on)
{qDebug() << on;
#ifdef ANDROID
    QtAndroid::runOnAndroidThread([on]() {
        QAndroidJniObject activity = QtAndroid::androidActivity();
                if (activity.isValid()) {
                    QAndroidJniObject window = activity.callObjectMethod("getWindow", "()Landroid/view/Window;");
                    if (window.isValid()) {
                        const int FLAG_KEEP_SCREEN_ON = 128;
                        if (on) {
                            window.callMethod<void>("addFlags", "(I)V", FLAG_KEEP_SCREEN_ON);
                        } else {
                            window.callMethod<void>("clearFlags", "(I)V", FLAG_KEEP_SCREEN_ON);
                        }
                    }
                }
                QAndroidJniEnvironment env;
                if (env->ExceptionCheck()) {
                    env->ExceptionClear();
                }
    });
#endif
}

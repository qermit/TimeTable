#include "systemwatch.h"
#if defined(Q_OS_MAC)
#include "systemwatch_mac.h"
#elif defined(Q_OS_WIN32)
#include "systemwatch_win.h"
#else
#include "systemwatch_unix.h"
#endif

#include <QApplication>

SystemWatch::SystemWatch() : QObject(qApp)
{
}

SystemWatch* SystemWatch::instance()
{
    if (!instance_) {
#if defined(Q_WS_MAC)
        instance_ = new MacSystemWatch();
#elif defined(Q_WS_WIN)
        instance_ = new WinSystemWatch();
#else
        instance_ = new UnixSystemWatch();
#endif
    }
    return instance_;
}


SystemWatch* SystemWatch::instance_ = 0;

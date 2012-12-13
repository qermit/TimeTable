#ifndef SYSTEMWATCH_H
#define SYSTEMWATCH_H

#include <qobject.h>

class SystemWatch : public QObject
{
    Q_OBJECT

public:
    static SystemWatch* instance();

signals:
    void sleep();
    void idleSleep();
    void wakeup();

protected:
    SystemWatch();

private:
    static SystemWatch* instance_;
};

#endif

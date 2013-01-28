#ifndef __SYSTEMWATCH_H__
#define __SYSTEMWATCH_H__

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

#endif // __SYSTEMWATCH_H__

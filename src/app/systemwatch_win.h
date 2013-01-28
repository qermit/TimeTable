#ifndef __SYSTEMWATCH_WIN_H__
#define __SYSTEMWATCH_WIN_H__

#include "systemwatch.h"

#include <qt_windows.h>
#include <wtsapi32.h>
#include <winbase.h>
#include <winuser.h>
#include <ws2spi.h>

class WinSystemWatch : public SystemWatch
{
public:
    WinSystemWatch();
    ~WinSystemWatch();
private:
    bool processWinEvent(MSG *m, long* result);
private:
    class MessageWindow;
    MessageWindow* _msgWnd;
};

#endif // __SYSTEMWATCH_WIN_H__

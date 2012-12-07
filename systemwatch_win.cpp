/*
 * systemwatch_win.cpp - Detect changes in the system state (Windows).
 * Copyright (C) 2005, 2008  James Chaldecott, Maciej Niedzielski
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

#include "systemwatch_win.h"

#include <QWidget>
#include <windows.h>
#include <QtCore>
#include <QtGui>
#include <QLibrary>

// workaround for the very old MinGW version bundled with Qt
#ifndef PBT_APMSUSPEND
#include <pbt.h>
#endif

/*
	Implementor notes:

	This class needs to get Windows messages.
	The easiest way is to get them from a top level QWidget instance.
	There was an attempt to use QApplication::winEventFilter(),
	but - as its name says - this is a filter, so all messages to
	all widgets go through it. So as a consequence, sleep() and wakeup()
	are emited many times during one event.

	Right now, there is a dummy window created just for SystemWatch.
	This may seem to be an unnecesary waste of resources, but the example
	above shows that too aggressive optimizations may hurt.
	A possible solution "in between" would be to catch events in already
	existing window (main window, probably)
	and pass them (by using ugly casting) directly to processWinEvent()
	But this would break the beauty of this tool.
*/

// -----------------------------------------------------------------------------
// WinSystemWatch
// -----------------------------------------------------------------------------

class WinSystemWatch::MessageWindow : public QWidget
{
public:
	MessageWindow(WinSystemWatch *parent)
        : syswatch(parent)
    {
		create();	// really create the window to enable messages

        QLibrary wtsapi32Lib("wtsapi32");

        wtsapi32Lib.load();
        wtsapi32Lib.loadHints();

        if(wtsapi32Lib.isLoaded())
        {
            QLibrary user32Lib("user32");

            user32Lib.load();
            user32Lib.loadHints();

            if(user32Lib.isLoaded())
            {
//                WTSRS = (MyPrototype2)wtsapi32Lib.resolve("WTSRegisterSessionNotification");
                WTSRegisterSessionNotification = (RegisterSessionNotification)wtsapi32Lib.resolve("WTSRegisterSessionNotification");
                if(WTSRegisterSessionNotification)
                {
                    WTSRegisterSessionNotification(MessageWindow::winId(),0);
                }
            }
        }
	}

	bool winEvent(MSG *m, long* result) 
	{
		if (syswatch->processWinEvent(m, result)) 
		{
			return true;
		}
		else 
		{
			return QWidget::winEvent(m, result);
		}
	}

	WinSystemWatch *syswatch;

    typedef bool (*RegisterSessionNotification)(HWND,DWORD);
    RegisterSessionNotification WTSRegisterSessionNotification;

//    typedef bool (*MyPrototype2)(HWND,DWORD);
//    MyPrototype2 WTSRS;
};



WinSystemWatch::WinSystemWatch() 
{
	d = new MessageWindow(this);
}

WinSystemWatch::~WinSystemWatch()
{
	delete d;
	d = 0;
}

bool WinSystemWatch::processWinEvent(MSG *m, long* result)
{
	qDebug() << QDateTime::currentDateTime().toString() << "m->message=" << m->message;
	qDebug() << QDateTime::currentDateTime().toString() << "m->wParam=" << m->wParam;

/*	if(WM_POWERBROADCAST == m->message)
	{
		switch (m->wParam) 
		{
			case PBT_APMSUSPEND:
				emit sleep();
				break;

			case PBT_APMRESUMESUSPEND:
				emit wakeup();
				break;

			case PBT_APMRESUMECRITICAL:
				// The system previously went into SUSPEND state (suddenly)
				// without sending PBT_APMSUSPEND.  Net connections are
				// probably invalid.  Not sure what to do about this.
				// Maybe:
				emit sleep();
				emit wakeup();
				break;

			case PBT_APMQUERYSUSPEND:
				// TODO: Check if file transfers are running, and don't go
				// to sleep if there are.  To refuse to suspend, we somehow
				// need to return BROADCAST_QUERY_DENY from the actual
				// windows procedure.
				break;
            case 32787: //PBT_POWERSETTINGCHANGE:
				qDebug() << QDateTime::currentDateTime().toString() << "PBT_POWERSETTINGCHANGE";
				break;
		}
	}
    else
*/
    if(m->wParam == 0x5)
    {
        qDebug()<<"WTS_SESSION_LOGON"<<"\n";
        emit wakeup();
    }
    else if(m->wParam == 0x6)
    {
        qDebug()<<"WTS_SESSION_LOGOFF"<<"\n";
        emit sleep();
    }
    else if(m->wParam == 0x7)
    {
        qDebug()<<"WTS_SESSION_LOCK"<<"\n";
        emit sleep();
    }
    else if(m->wParam == 0x8)
    {
        qDebug()<<"WTS_SESSION_UNLOCK"<<"\n";
        emit wakeup();
    }

	return false; // Let Qt handle the right return value.
}

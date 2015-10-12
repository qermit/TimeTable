TEMPLATE = app
TARGET = timetable
CONFIG += debug
DESTDIR = ../build/bin
OBJECTS_DIR += ../build/bin

QT += core gui sql widgets

HEADERS   = database.h \
            mainwindow.h \
	    systemwatch.h \
	    timedelegate.h \
	    daysmodel.h \
	    systemwatch_win.h
RESOURCES += app.qrc
SOURCES  += main.cpp \
            mainwindow.cpp \
            systemwatch.cpp \
            timedelegate.cpp \
	    daysmodel.cpp \
	    systemwatch_win.cpp

TRANSLATIONS = ./lang/timetable_ru.ts \
			   ./lang/timetable_en.ts


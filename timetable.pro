HEADERS   = database.h \
            mainwindow.h \
	    systemwatch.h \
	    timedelegate.h \
	    daysmodel.h \
	    systemwatch_win.h
RESOURCES = timetable.qrc
SOURCES   = main.cpp \
            mainwindow.cpp \
            systemwatch.cpp \
            timedelegate.cpp \
	    daysmodel.cpp \
	    systemwatch_win.cpp

QT += sql
QT += xml

# install
target.path = ./bin/
sources.files = $$SOURCES *.h $$RESOURCES $$FORMS timetable.pro
sources.path = ./timetable
INSTALLS += target sources

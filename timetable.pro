HEADERS   = database.h \
            dialog.h \
            mainwindow.h \
	    systemwatch.h \
	    timedelegate.h \
	    daysmodel.h \
	    systemwatch_win.h
RESOURCES = timetable.qrc
SOURCES   = dialog.cpp \
            main.cpp \
            mainwindow.cpp \
            systemwatch.cpp \
            timedelegate.cpp \
	    daysmodel.cpp \
	    systemwatch_win.cpp

QT += sql
QT += xml

# install
target.path = ./bin/
sources.files = $$SOURCES *.h $$RESOURCES $$FORMS timetable.pro *.xml images
sources.path = ./timetable
INSTALLS += target sources

symbian {
    TARGET.UID3 = 0xA000D7CF
    include($$QT_SOURCE_TREE/examples/symbianpkgrules.pri)
}

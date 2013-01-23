QT += sql
TARGET = base
TEMPLATE = lib
target.path = /usr/lib/timetable/base
DESTDIR = ../build/lib
QMAKE_CXXFLAGS += -D_FILE_OFFSET_BITS=64

sources.files += $$HEADERS_EXPORT
sources.path = /usr/include/timetable

INSTALLS += target \
			sources 

QT += gui sql
TEMPLATE        = lib
CONFIG         += plugin

INCLUDEPATH     += ../../base

HEADERS         = ncreporter.h \
	exportdialog.h

SOURCES         = ncreporter.cpp \
	exportdialog.cpp 

TARGET          = $$qtLibraryTarget(nc-reporter-plugin)
DESTDIR         = ../../build/bin/plugins

INCLUDEPATH += C:/NCReport/2.8.7.MinGW.eval/include

CONFIG( debug, debug|release ) {
	LIBS += C:/NCReport/2.8.7.MinGW.eval/lib/libncreportd2.a
} else {
	LIBS += C:/NCReport/2.8.7.MinGW.eval/lib/libncreport2.a
}



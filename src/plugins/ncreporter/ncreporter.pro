QT += gui sql widgets
TEMPLATE        = lib
CONFIG         += plugin

INCLUDEPATH     += ../../base

HEADERS         = ncreporter.h \
	exportdialog.h \
    datetimerenderer.h

SOURCES         = ncreporter.cpp \
	exportdialog.cpp \ 
    datetimerenderer.cpp

TARGET          = $$qtLibraryTarget(nc-reporter-plugin)
DESTDIR         = ../../build/bin/plugins

INCLUDEPATH += C:/NCReport/2.8.7.MinGW.eval/include

CONFIG( debug, debug|release ) {
	LIBS += C:/NCReport/2.8.7.MinGW.eval/lib/libncreportd2.a
} else {
	LIBS += C:/NCReport/2.8.7.MinGW.eval/lib/libncreport2.a
}

TRANSLATIONS = ./lang/nc_reporter_plugin_ru.ts \
			   ./lang/nc_reporter_plugin_en.ts


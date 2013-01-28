#ifndef __REPORTER_INTERFACE_H__
#define __REPORTER_INTERFACE_H__

#include <QtCore/qplugin.h>
#include <QString>
#include <QIcon>

 class ReporterInterface
 {
 public:
	 virtual ~ReporterInterface() {}

	 virtual QString type() = 0;
	 virtual QString getTranslationFile(const QString& locale) = 0;

	 virtual void generateReport() = 0;
 };

 Q_DECLARE_INTERFACE(ReporterInterface, "org.timetable.timetable.plugin/1.0");

#endif // __REPORTER_INTERFACE_H__

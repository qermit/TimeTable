#ifndef REPORTER_INTERFACE_H
#define REPORTER_INTERFACE_H

#include <QtCore/qplugin.h>
#include <QString>
#include <QIcon>

 class ReporterInterface
 {
 public:
	 virtual ~ReporterInterface() {}

	 virtual QString type() = 0;
	 virtual void generateReport() = 0;
 };

 Q_DECLARE_INTERFACE(ReporterInterface, "org.timetable.timetable.plugin/1.0");

#endif // REPORTER_INTERFACE_H

#ifndef __NC_REPORTER_PLUGIN_H__
#define __NC_REPORTER_PLUGIN_H__

#include <QObject>
#include <QtCore/qplugin.h>
#include "../../base/reporter_interface.h"
#include "exportdialog.h"

class NcReporter : public QObject, public ReporterInterface
{
    Q_OBJECT
    Q_INTERFACES(ReporterInterface)

public:
    NcReporter();
    ~NcReporter();
private:
    virtual QString type();
    virtual QString getTranslationFile(const QString& locale);
    virtual void generateReport();
 };


#endif // __NC_REPORTER_PLUGIN_H__


#include "ncreporter.h"
#include "exportdialog.h"
 
NcReporter::NcReporter() 
{
}

NcReporter::~NcReporter()
{
}

QString NcReporter::type()
{
	QString type("NcReporter");
	return type;
}

QString NcReporter::getTranslationFile(const QString& locale)
{
	return QString("nc_reporter_plugin_" + locale);
}


void NcReporter::generateReport()
{
    ExportDialog* dialog = new ExportDialog();
    dialog->exec();
}

Q_EXPORT_PLUGIN2(nc-reporter-plugin, NcReporter);

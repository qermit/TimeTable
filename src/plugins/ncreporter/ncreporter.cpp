
#include "ncreporter.h"
#include "exportdialog.h"
 
NcReporter::NcReporter() 
{
    int a = 0;
}

NcReporter::~NcReporter()
{
}

QString NcReporter::type()
{
	QString type("NcReporter");
	return type;
}

void NcReporter::generateReport()
{
    ExportDialog* dialog = new ExportDialog();
    dialog->exec();
}

Q_EXPORT_PLUGIN2(nc-reporter-plugin, NcReporter);

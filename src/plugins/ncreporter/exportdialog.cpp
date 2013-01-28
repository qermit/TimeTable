#include "ncreport.h"
#include "ncreportoutput.h"
#include "ncreportpreviewoutput.h"
#include "ncreportpreviewwindow.h"
#include "datetimerenderer.h"

#include <QSqlDatabase>
#include <QSqlError>
#include <QMessageBox>

#include "exportdialog.h"

ExportDialog::ExportDialog(QWidget* parent) : QDialog(parent)
{
    QGroupBox* inputWidgetBox = createInputWidgets();
    QDialogButtonBox* buttonBox = createButtons();

    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(inputWidgetBox);
    layout->addWidget(buttonBox);
    setLayout(layout);

    setWindowTitle(tr("Add record"));

    _reporter = new NCReport();
}

ExportDialog::~ExportDialog()
{
    delete _reporter;
}

QDialogButtonBox* ExportDialog::createButtons()
{
    QPushButton* closeButton = new QPushButton(tr("&Close"));
    QPushButton* exportButton = new QPushButton(tr("&Export"));

    closeButton->setDefault(true);

    connect(closeButton, SIGNAL(clicked()), this, SLOT(close()));
    connect(exportButton, SIGNAL(clicked()), this, SLOT(doExport()));

    QDialogButtonBox *buttonBox = new QDialogButtonBox;
    buttonBox->addButton(exportButton, QDialogButtonBox::AcceptRole);
    buttonBox->addButton(closeButton, QDialogButtonBox::RejectRole);

    return buttonBox;
}

QGroupBox* ExportDialog::createInputWidgets()
{
    QGroupBox* mainBox = new QGroupBox(this);
    QLabel* intervalLabel = new QLabel(tr("Interval"), this);

    QGroupBox* intervalBox = new QGroupBox();
    intervalBox->setStyleSheet("border:0;");
    intervalBox->setMinimumWidth(100);
    intervalBox->setMinimumHeight(100);

    QRadioButton *radio1 = new QRadioButton(tr("&All"));
    radio1->setChecked(true);
    QRadioButton *radio2 = new QRadioButton(tr("Select"));
    _leftDate = new QDateEdit(QDate::currentDate());
    _leftDate->setMinimumHeight(20);
    _leftDate->setEnabled(!radio1->isChecked());
    _rightDate = new QDateEdit(QDate::currentDate());
    _rightDate->setMinimumHeight(20);
    _rightDate->setEnabled(!radio1->isChecked());
    connect(radio1, SIGNAL(clicked()), this, SLOT(setAllRecords()));
    connect(radio2, SIGNAL(clicked()), this, SLOT(setRecordsFromInterval()));

    QVBoxLayout *vbox = new QVBoxLayout;
    vbox->addWidget(radio1);
    vbox->addWidget(radio2);
    vbox->addWidget(_leftDate);
    vbox->addWidget(_rightDate);
    intervalBox->setLayout(vbox);

    QLabel* outLabel = new QLabel(tr("Output"));
    QComboBox* outBox = new QComboBox(mainBox);
    outBox->addItem(tr("Preview"));
    outBox->addItem(tr("Pdf"));
    outBox->addItem(tr("Cvs"));
    outBox->addItem(tr("Excel"));

    QGridLayout *mainLayout = new QGridLayout;
    mainLayout->addWidget(intervalLabel, 0, 0, Qt::AlignTop);
    mainLayout->addWidget(intervalBox, 0, 1);
    mainLayout->addWidget(outLabel, 1, 0);
    mainLayout->addWidget(outBox, 1, 1);
    mainBox->setLayout(mainLayout);

    return mainBox;
}

void ExportDialog::doExport()
{
    if ( !connectDB("northwind") )	// connect SQL database using connection id: northwind
        return;

    uint left = QDateTime::fromString(_leftDate->isEnabled() ? _leftDate->text() : "01.10.1970", "dd.MM.yyyy").toUTC().toTime_t();
    uint right = QDateTime::fromString(_rightDate->isEnabled() ? _rightDate->text() : "31.12.2100", "dd.MM.yyyy").toUTC().toTime_t();
    if(left > right)
    {
        QMessageBox::warning( 0, tr("Error"), tr("Incorrect report interval"));
        return;
    }

    _reporter->reset();  // reset report
    _reporter->setReportSource( NCReportSource::File ); // set report source type
    _reporter->setReportFile("report_general.xml");
    _reporter->addParameter("left", QVariant(left));
    _reporter->addParameter("right", QVariant(right));

    TimeItemRendering* timeRenderer = new TimeItemRendering();
    timeRenderer->setID("timeCustom");
    _reporter->addItemRenderingClass(timeRenderer);

    DateItemRendering* dateRenderer = new DateItemRendering();
    dateRenderer->setID("dateCustom");
    _reporter->addItemRenderingClass(dateRenderer);

    _reporter->runReportToPreview(); // run to preview output

    if (_reporter->hasError())
    {
        QMessageBox::information( 0, tr("Report error"), _reporter->lastErrorMsg());
        return;
    }
    else
    {
        NCReportPreviewWindow *pv = new NCReportPreviewWindow();	// create preview window
        pv->setOutput( (NCReportPreviewOutput*)_reporter->output() );  // add output to the window
        pv->setWindowModality(Qt::ApplicationModal );	// set modality
        pv->setAttribute( Qt::WA_DeleteOnClose );	// set attrib
        pv->show();	// show
    }
    close();
}

bool ExportDialog::connectDB( const QString& id )
{
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    if(!db.isValid())
    {
        QMessageBox::warning( 0, tr("Database error"), QObject::tr("Could not load database driver.") );
        return false;
    }
    db.setDatabaseName("timetable.db");

    if(!db.open())
    {
        QMessageBox::warning( 0, tr("NCReport error"), QObject::tr("Cannot open database: ")+db.lastError().databaseText() );
        return false;
    }

    return true;
}

void ExportDialog::setAllRecords()
{
    _leftDate->setEnabled(false);
    _rightDate->setEnabled(false);
}

void ExportDialog::setRecordsFromInterval()
{
    _leftDate->setEnabled(true);
    _rightDate->setEnabled(true);
}

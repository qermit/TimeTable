#include "mainwindow.h"

#include <QtGui>
#include <QtSql>
#include <QtXml>
#include "systemwatch.h"
#include "daysmodel.h"
#include "timedelegate.h"

MainWindow::MainWindow(const QString &hourTable, QWidget *parent)
    : QMainWindow(parent)
{
    setWindowFlags( Qt::CustomizeWindowHint | Qt::WindowMinimizeButtonHint | Qt::WindowCloseButtonHint );

    _model = new QSqlRelationalTableModel(this);
    _model->setTable(hourTable);
    _model->setFilter("");
    _model->select();
    _newRecordId = _model->rowCount();

    _daysModel = new DaysModel(*_model, this);

    QGroupBox *days = createDaysGroupBox();
    QGroupBox *details = createDetailsGroupBox();

    changeDate(QDate::currentDate());

    connect(_model, SIGNAL(rowsInserted(QModelIndex,int,int)),
            this, SLOT(updateHeader(QModelIndex,int,int)));
    connect(_model, SIGNAL(rowsRemoved(QModelIndex,int,int)),
            this, SLOT(updateHeader(QModelIndex,int,int)));

    QGridLayout *layout = new QGridLayout;
    layout->addWidget(days, 0, 0);
    layout->addWidget(details, 1, 0);
    layout->setColumnStretch(0, 1);

    QWidget *widget = new QWidget;
    widget->setLayout(layout);
    setCentralWidget(widget);
    createMenuBar();

    resize(350, 500);
    setWindowTitle(tr("Timetable"));

    createActions();
    createTrayIcon();

    QIcon icon = QIcon(":images/timetable_icon.png");
    trayIcon->setIcon(icon);
    setWindowIcon(icon);

    trayIcon->show();

    connect(trayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)),
          this, SLOT(iconActivated(QSystemTrayIcon::ActivationReason)));

    // Connect to the system monitor
    SystemWatch* sw = SystemWatch::instance();
    connect(sw, SIGNAL(sleep()), this, SLOT(doSleep()));
    connect(sw, SIGNAL(wakeup()), this, SLOT(doWakeup()));

    addNewRecord(QDateTime::currentDateTimeUtc());

    _workedHoursTimer = new QTimer(this);
    connect(_workedHoursTimer, SIGNAL(timeout()), this, SLOT(workedHoursUpdate()));
    _workedHoursTimer->start(60*1000);
}

MainWindow::~MainWindow()
{
    finalizeLastRecord(QDateTime::currentDateTimeUtc());

    delete _daysModel;
    delete _model;
}

void MainWindow::createTrayIcon()
{
    trayIconMenu = new QMenu(this);
    trayIconMenu->addAction(_restoreAction);
    trayIconMenu->addSeparator();
    trayIconMenu->addAction(_quitAction);

    trayIcon = new QSystemTrayIcon(this);
    trayIcon->setContextMenu(trayIconMenu);
}

void MainWindow::createActions()
{
    _restoreAction = new QAction(tr("&Restore"), this);
    connect(_restoreAction, SIGNAL(triggered()), this, SLOT(showNormal()));

    _quitAction = new QAction(tr("&Quit"), this);
    connect(_quitAction, SIGNAL(triggered()), qApp, SLOT(quit()));
}

void MainWindow::iconActivated(QSystemTrayIcon::ActivationReason reason)
{
    switch (reason)
    {
    case QSystemTrayIcon::MiddleClick:
        //showMessage();
        break;
    case QSystemTrayIcon::Trigger:
    case QSystemTrayIcon::DoubleClick:
        showNormal();
        break;
    default:
        break;
    }
}

void MainWindow::changeEvent(QEvent* e)
{
    switch (e->type())
    {
    case QEvent::WindowStateChange:
    {
        if (this->windowState() & Qt::WindowMinimized)
        {
            QTimer::singleShot(250, this, SLOT(hide()));
        }
        break;
    }
    default:
        break;
    }

    QMainWindow::changeEvent(e);
}

void MainWindow::changeDate(int row)
{
    if (row > 0)
    {
        QModelIndex index = _model->relationModel(2)->index(row, 1);
        _model->setFilter("day = '" + index.data().toString() + '\'') ;
    }
    else if (row == 0)
    {
        _model->setFilter(QString());
    }
    else
    {
        return;
    }
}

void MainWindow::changeDate(QModelIndex index)
{
    _calendar->setSelectedDate(index.data().toDate());

    updateDetails(index.data().toDate());
    updateWeekHours(index.data().toDate());
    updateDayHours(index.data().toDate());
}

void MainWindow::changeDate(const QDate& date)
{
    updateDetails(date);
    updateWeekHours(date);
    updateDayHours(date);
}

QGroupBox* MainWindow::createDaysGroupBox()
{
    _calendar = new QCalendarWidget;
    _calendar->setMinimumDate(QDate(1900, 1, 1));
    _calendar->setMaximumDate(QDate(3000, 1, 1));
    _calendar->setGridVisible(true);

    connect(_calendar, SIGNAL(clicked(const QDate&)),
            this, SLOT(changeDate(const QDate&)));

    _hoursPerWeek = new QLabel();
    updateWeekHours(QDate::currentDate());

    QGroupBox *box = new QGroupBox(tr("Days"));

    QGridLayout *layout = new QGridLayout;
    layout->addWidget(_calendar, 0, 0);
    layout->addWidget(_hoursPerWeek, 1, 0, Qt::AlignRight);
    box->setLayout(layout);
    box->resize(50, 50);

    return box;
}

QGroupBox* MainWindow::createDetailsGroupBox()
{
    QGroupBox *box = new QGroupBox(tr("Details"));

    _detailsView = new QTableView;
    _detailsView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    _detailsView->setSortingEnabled(true);
    _detailsView->setSelectionBehavior(QAbstractItemView::SelectRows);
    _detailsView->setSelectionMode(QAbstractItemView::SingleSelection);
    _detailsView->setShowGrid(false);
    _detailsView->verticalHeader()->hide();
    _detailsView->setAlternatingRowColors(true);
    _detailsView->setModel(_model);
    adjustHeader();
    _detailsView->setItemDelegateForColumn(2, new DateFormatDelegate("dddd dd MMM yyyy", this));
    _detailsView->setItemDelegateForColumn(3, new TimeFormatDelegate("hh:mm:ss", this));
    _detailsView->setItemDelegateForColumn(4, new TimeFormatDelegate("hh:mm:ss", this));
    _detailsView->horizontalHeader()->setResizeMode(2, QHeaderView::Stretch);

    QLocale locale = _detailsView->locale();
    locale.setNumberOptions(QLocale::OmitGroupSeparator);
    _detailsView->setLocale(locale);

    connect(_detailsView, SIGNAL(clicked(QModelIndex)),
            this, SLOT(showAlbumDetails(QModelIndex)));
    connect(_detailsView, SIGNAL(activated(QModelIndex)),
            this, SLOT(showAlbumDetails(QModelIndex)));

    _hoursPerDay = new QLabel();

    QGridLayout* layout = new QGridLayout;
    layout->addWidget(_detailsView, 0, 0);
    layout->addWidget(_hoursPerDay, 1, 0, Qt::AlignRight);
    box->setLayout(layout);

    return box;
}

QGroupBox* MainWindow::createHoursGroupBox()
{
    _hoursView = new QTableView;
    _hoursView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    _hoursView->setSortingEnabled(true);
    _hoursView->setSelectionBehavior(QAbstractItemView::SelectRows);
    _hoursView->setSelectionMode(QAbstractItemView::SingleSelection);
    _hoursView->setShowGrid(false);
    _hoursView->verticalHeader()->hide();
    _hoursView->setAlternatingRowColors(true);
    _hoursView->setModel(_daysModel);
    _hoursView->horizontalHeader()->setResizeMode(0, QHeaderView::Stretch);
    _hoursView->resizeColumnToContents(1);

    QDate current = QDate::currentDate();
    QModelIndex index = _daysModel->index(current.dayOfYear() - 1, 0);
    _hoursView->setCurrentIndex(index);

    connect(_hoursView, SIGNAL(clicked(QModelIndex)),
            this, SLOT(changeDate(QModelIndex)));
    connect(_hoursView, SIGNAL(activated(QModelIndex)),
            this, SLOT(changeDate(QModelIndex)));

    _hoursPerWeek = new QLabel();

    updateWeekHours(QDate::currentDate());

    QGroupBox* box = new QGroupBox(tr("Hours"));

    QGridLayout *layout = new QGridLayout;
    layout->addWidget(_hoursView, 0, 0);
    layout->addWidget(_hoursPerWeek, 1, 0, Qt::AlignRight);
    box->setLayout(layout);

    return box;
}

void MainWindow::createMenuBar()
{
    QAction *quitAction = new QAction(tr("&Quit"), this);
    QAction *aboutAction = new QAction(tr("&About"), this);
    QAction *aboutQtAction = new QAction(tr("About &Qt"), this);

    quitAction->setShortcuts(QKeySequence::Quit);

    QMenu *fileMenu = menuBar()->addMenu(tr("&File"));
    fileMenu->addAction(quitAction);

    QMenu *helpMenu = menuBar()->addMenu(tr("&Help"));
    helpMenu->addAction(aboutAction);
    helpMenu->addAction(aboutQtAction);

    connect(quitAction, SIGNAL(triggered(bool)), this, SLOT(close()));
    connect(aboutAction, SIGNAL(triggered(bool)), this, SLOT(about()));
    connect(aboutQtAction, SIGNAL(triggered(bool)), qApp, SLOT(aboutQt()));
}

void MainWindow::updateHeader(QModelIndex, int, int)
{
    adjustHeader();
}

void MainWindow::adjustHeader()
{
    _detailsView->hideColumn(1);
    _detailsView->horizontalHeader()->setResizeMode(4, QHeaderView::Stretch);
    _detailsView->horizontalHeader()->model()->setHeaderData(0, Qt::Horizontal, tr("Id"));
    _detailsView->horizontalHeader()->model()->setHeaderData(2, Qt::Horizontal, tr("Date"));
    _detailsView->horizontalHeader()->model()->setHeaderData(3, Qt::Horizontal, tr("Start"));
    _detailsView->horizontalHeader()->model()->setHeaderData(4, Qt::Horizontal, tr("End"));

    _detailsView->resizeColumnToContents(0);
    _detailsView->resizeColumnToContents(2);
    _detailsView->resizeColumnToContents(3);
    _detailsView->resizeColumnToContents(4);
}

void MainWindow::about()
{
    //! @todo Update dialog with some additional info, and remove note about missing reports
    QMessageBox::about(this, tr("Timetable"),
                       tr("<p>The <b>Timetable</b> is a small tool allowing to count "
                          "time of your PC activity (could be used as working time counter)."
                          "The collected data are kept in a database</p>"
                          "<p>The tool also provides time reports which are useful for tracking work time during"
                          " a week/month/year. <i>Maybe not right now, but definitely will ;)</i> </p>"));
}

void MainWindow::doSleep()
{
    _workedHoursTimer->stop();

    _sleepStartTime = QDateTime::currentDateTimeUtc();
}

void MainWindow::doWakeup()
{
    QDateTime currentDt(QDateTime::currentDateTimeUtc());
    if(currentDt.toTime_t() - _sleepStartTime.toTime_t() <= 15 * 60)
    {
        //Do nothing according to requirements. 15 minutes...
    }
    else if(currentDt.toTime_t() - _sleepStartTime.toTime_t() > 15 * 60 &&
            currentDt.toTime_t() - _sleepStartTime.toTime_t() <= 60 * 60 &&
            QMessageBox::information( this, tr("Warning"),
                                           tr("You were absent more than 15 minutes. Would you like to mark this time as worked?"),
                                           tr("Yes"), tr("No"),
                                           0, 1 ) == 0)
    {
        // Do nothing according to requirements
    }
    else
    {
        finalizeLastRecord(_sleepStartTime);
        addNewRecord(QDateTime::currentDateTimeUtc());
    }

    _workedHoursTimer->start(60*1000);
    updateWeekHours(_calendar->selectedDate());
    updateDayHours(_calendar->selectedDate());
}

int MainWindow::addNewRecord(const QDateTime& dateTime)
{
    int id = generateRecordId();
    QSqlRecord record;

    QSqlField f1("id", QVariant::Int);
    QSqlField f2("week", QVariant::Int);
    QSqlField f3("day", QVariant::UInt);
    QSqlField f4("start", QVariant::Int);
    QSqlField f5("end", QVariant::Int);

    f1.setValue(QVariant(id));
    QDateTime currentDate = QDateTime(dateTime.date());
    f2.setValue(QVariant(dateTime.date().weekNumber()));
    f3.setValue(QVariant(currentDate.toUTC().toTime_t()));
    f4.setValue(QVariant(dateTime.toUTC().toTime_t()));
    f5.setValue(QVariant(0));
    record.append(f1);
    record.append(f2);
    record.append(f3);
    record.append(f4);
    record.append(f5);

    bool res = _model->insertRecord(-1, record);

    return id;
}

void MainWindow::finalizeLastRecord(const QDateTime& dateTime)
{
    QDateTime currentDate = QDateTime(dateTime.date());
    QString filter = QString::number(currentDate.toUTC().toTime_t(), 10);
    _model->setFilter("day = '" + filter + "'");
    int count = _model->rowCount();
    for (int i = count-1; i >=0 ; i--)
    {
        QSqlRecord record =  _model->record(i);
        if (record.value("day") == currentDate.toUTC().toTime_t())
        {
            QString val = record.value("end").toString();
            if(val == "0")
            {
                record.setValue("end", dateTime.toUTC().toTime_t());
                val = record.value("end").toString();
                _model->removeRow(i);
                _model->insertRecord(-1, record);
                break;
            }
        }
    }
}

int MainWindow::generateRecordId()
{
    return ++_newRecordId;
}

void MainWindow::updateDetails(const QDate& date)
{
    QDateTime dt(date);
    QString filter = QString::number(dt.toUTC().toTime_t(), 10);
    _model->setFilter("day = '" + filter + "'");
}

void MainWindow::updateWeekHours(const QDate& date)
{
    QString hpwText = tr("Worked per week: ");
    int seconds = _daysModel->calculateHoursPerWeek(date);
    QTime time(seconds/3600, (seconds%3600)/60, (seconds%3600)%60);
    hpwText += time.toString("hh:mm:ss");
    _hoursPerWeek->setText(hpwText);
}

void MainWindow::updateDayHours(const QDate& date)
{
    QString hpdText = tr("Worked per day: ");
    int seconds = _daysModel->calculateHours(date);
    QTime time(seconds/3600, (seconds%3600)/60, (seconds%3600)%60);
    hpdText += time.toString("hh:mm:ss");
    _hoursPerDay->setText(hpdText);
}

void MainWindow::workedHoursUpdate()
{
    updateWeekHours(QDate::currentDate());
    updateDayHours(QDate::currentDate());
}

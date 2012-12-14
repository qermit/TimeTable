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
    _model->select();

    _daysModel = new DaysModel(*_model, this);

    QGroupBox *days = createDaysGroupBox();
    QGroupBox *hours = createHoursGroupBox();
    QGroupBox *details = createDetailsGroupBox();

    changeDate(QDate::currentDate());
    _newRecordId = _model->rowCount();

    connect(_model, SIGNAL(rowsInserted(QModelIndex,int,int)),
            this, SLOT(updateHeader(QModelIndex,int,int)));
    connect(_model, SIGNAL(rowsRemoved(QModelIndex,int,int)),
            this, SLOT(updateHeader(QModelIndex,int,int)));

    QGridLayout *layout = new QGridLayout;
    layout->addWidget(days, 0, 0);
    layout->addWidget(details, 0, 1, 2, 1);
    layout->addWidget(hours, 1, 0);
    layout->setColumnStretch(1, 1);
    layout->setColumnMinimumWidth(0, 300);

    QWidget *widget = new QWidget;
    widget->setLayout(layout);
    setCentralWidget(widget);
    createMenuBar();

    resize(850, 400);
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

    addNewRecord();
}

MainWindow::~MainWindow()
{
    finalizeLastRecord();

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
    updateDetails(index.data().toDate());
    _calendar->setSelectedDate(index.data().toDate());
    updateWeekHours(index.data().toDate());
}

void MainWindow::changeDate(const QDate& date)
{
    updateDetails(date);

    QModelIndex index = _daysModel->index(date.dayOfYear() - 1, 0);
    QItemSelectionModel* selModel = _hoursView->selectionModel();
    selModel->select(index, QItemSelectionModel::ClearAndSelect | QItemSelectionModel::Rows);

    updateWeekHours(date);
}

QGroupBox* MainWindow::createDaysGroupBox()
{
    _calendar = new QCalendarWidget;
    _calendar->setMinimumDate(QDate(1900, 1, 1));
    _calendar->setMaximumDate(QDate(3000, 1, 1));
    _calendar->setGridVisible(true);

    connect(_calendar, SIGNAL(clicked(const QDate&)),
            this, SLOT(changeDate(const QDate&)));

    QGroupBox *box = new QGroupBox(tr("Days"));

    QGridLayout *layout = new QGridLayout;
    layout->addWidget(_calendar, 0, 0);
    box->setLayout(layout);

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
    _detailsView->setItemDelegateForColumn(2, new DateFormatDelegate("dd.MM.yyyy", this));
    _detailsView->setItemDelegateForColumn(3, new TimeFormatDelegate("hh:mm", this));
    _detailsView->setItemDelegateForColumn(4, new TimeFormatDelegate("hh:mm", this));
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
    _detailsView->horizontalHeader()->setResizeMode(2, QHeaderView::Stretch);
    _detailsView->resizeColumnToContents(0);
    _detailsView->resizeColumnToContents(3);
    _detailsView->resizeColumnToContents(4);
}

void MainWindow::about()
{
    QMessageBox::about(this, tr("About Music Archive"),
                       tr("<p>The <b>Music Archive</b> example shows how to present "
                          "data from different data sources in the same application. "
                          "The album titles, and the corresponding artists and release dates, "
                          "are kept in a database, while each album's tracks are stored "
                          "in an XML file. </p><p>The example also shows how to add as "
                          "well as remove data from both the database and the "
                          "associated XML file using the API provided by the QtSql and "
                          "QtXml modules, respectively.</p>"));
}

void MainWindow::doSleep()
{
    finalizeLastRecord();
}

void MainWindow::doWakeup()
{
    addNewRecord();
}

int MainWindow::addNewRecord()
{
    int id = generateRecordId();
    QSqlRecord record;

    QSqlField f1("id", QVariant::Int);
    QSqlField f2("week", QVariant::Int);
    QSqlField f3("day", QVariant::UInt);
    QSqlField f4("start", QVariant::Int);
    QSqlField f5("end", QVariant::Int);

    f1.setValue(QVariant(id));
    QDateTime currentDate = QDateTime(QDate::currentDate());
    f2.setValue(QVariant(QDate::currentDate().weekNumber()));
    f3.setValue(QVariant(currentDate.toTime_t()));
    f4.setValue(QVariant(QDateTime::currentDateTime().toTime_t()));
    f5.setValue(QVariant(0));
    record.append(f1);
    record.append(f2);
    record.append(f3);
    record.append(f4);
    record.append(f5);

    _model->insertRecord(-1, record);

    return id;
}

void MainWindow::finalizeLastRecord()
{
    QDateTime currentDate = QDateTime(QDate::currentDate());
    for (int i = _model->rowCount()-1; i >=0 ; i--)
    {
        QSqlRecord record =  _model->record(i);
        if (record.value("day") == currentDate.toTime_t())
        {
            QString val = record.value("end").toString();
            if(val == "0")
            {
                record.setValue("end", QDateTime::currentDateTime().toTime_t());
                val = record.value("end").toString();
                //model->setRecord(i, record);
                _model->removeRow(i);
                _model->insertRecord(-1, record);
                break;
            }
        }
    }

    for (int i = 0; i < _model->rowCount(); i++)
    {
        QSqlRecord record =  _model->record(i);
        if (record.value("day") == currentDate.toTime_t())
        {
            QString val = record.value("end").toString();
            qDebug() <<i<<val;
        }
    }
}

int MainWindow::generateRecordId()
{
    return ++_newRecordId;
}

void MainWindow::updateWeekHours(const QDate& date)
{
    QString hpwText = tr("Worked per week: ");
    int seconds = _daysModel->calculateHoursPerWeek(date);
    hpwText += QString::fromAscii("%1:%2").arg(seconds/3600).arg(seconds/60);
    _hoursPerWeek->setText(hpwText);
}

void MainWindow::updateDetails(const QDate& date)
{
    QDateTime dt(date);
    QString filter = QString::number(dt.toTime_t(), 10);
    int count = _model->rowCount();
    _model->setFilter("day = '" + filter + "'");

    QModelIndex index = _daysModel->index(date.dayOfYear() - 1, 1);
    _hoursPerDay->setText(tr("Worked per day: ") + _daysModel->data(index).toString());
}

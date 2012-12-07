/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** You may use this file under the terms of the BSD license as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of Nokia Corporation and its Subsidiary(-ies) nor
**     the names of its contributors may be used to endorse or promote
**     products derived from this software without specific prior written
**     permission.
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
** $QT_END_LICENSE$
**
****************************************************************************/

#include "mainwindow.h"
#include "dialog.h"

#include <QtGui>
#include <QtSql>
#include <QtXml>
#include "systemwatch.h"
#include "daysmodel.h"
#include "timedelegate.h"

extern int uniqueAlbumId;
extern int uniqueArtistId;

MainWindow::MainWindow(const QString &dayTable, const QString &hourTable,
                       QFile *albumDetails, QWidget *parent)
     : QMainWindow(parent)
{
    file = albumDetails;

    model = new QSqlRelationalTableModel(this);
    model->setTable(hourTable);
    //model->setRelation(1, QSqlRelation(dayTable, "id", "day"));
    model->select();

    QGroupBox *days = createDaysGroupBox();
	QGroupBox *hours = createHoursGroupBox();
    QGroupBox *details = createDetailsGroupBox();
    
//    daysView->setCurrentIndex(0);
    uniqueAlbumId = model->rowCount();
//    uniqueArtistId = daysView->count();

    connect(model, SIGNAL(rowsInserted(QModelIndex,int,int)),
            this, SLOT(updateHeader(QModelIndex,int,int)));
    connect(model, SIGNAL(rowsRemoved(QModelIndex,int,int)),
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
    setWindowTitle(tr("Music Archive"));

	// Connect to the system monitor
	SystemWatch* sw = SystemWatch::instance();
	connect(sw, SIGNAL(sleep()), this, SLOT(doSleep()));
	connect(sw, SIGNAL(wakeup()), this, SLOT(doWakeup()));
}

void MainWindow::changeArtist(int row)
{
    if (row > 0) 
	{
        QModelIndex index = model->relationModel(1)->index(row, 1);
        model->setFilter("day = '" + index.data().toString() + '\'') ;
    } 
	else if (row == 0) 
	{
        model->setFilter(QString());
    } 
	else 
	{
        return;
    }
}

void MainWindow::changeArtist(QModelIndex index)
{
	QDateTime dt(index.data().toDate());
	uint time = dt.toTime_t();
	qDebug() << time;
	qDebug() << dt.toLocalTime().toString();
	QString filter = QString::number(time, 10);
	model->setFilter("day = '" + filter + '\'') ;
}

void MainWindow::addAlbum()
{
    Dialog *dialog = new Dialog(model, this);
    int accepted = dialog->exec();

    if (accepted == 1) {
        int lastRow = model->rowCount() - 1;
        detailsView->selectRow(lastRow);
        detailsView->scrollToBottom();
    }
}

void MainWindow::deleteAlbum()
{
    QModelIndexList selection = detailsView->selectionModel()->selectedRows(0);

    if (!selection.empty()) {
        QModelIndex idIndex = selection.at(0);
        int id = idIndex.data().toInt();
        QString title = idIndex.sibling(idIndex.row(), 1).data().toString();
        QString artist = idIndex.sibling(idIndex.row(), 2).data().toString();

        QMessageBox::StandardButton button;
        button = QMessageBox::question(this, tr("Delete Album"),
                                       QString(tr("Are you sure you want to " \
                                                  "delete '%1' by '%2'?"))
                                              .arg(title).arg(artist),
                                       QMessageBox::Yes | QMessageBox::No);

        if (button == QMessageBox::Yes) {
            removeAlbumFromFile(id);
            removeAlbumFromDatabase(idIndex);
//            decreaseAlbumCount(indexOfArtist(artist));
        }
    } else {
        QMessageBox::information(this, tr("Delete Album"),
                                 tr("Select the album you want to delete."));
    }
}

void MainWindow::removeAlbumFromFile(int id)
{
    QDomNodeList albums = albumData.elementsByTagName("album");

    for (int i = 0; i < albums.count(); i++) {
        QDomNode node = albums.item(i);
        if (node.toElement().attribute("id").toInt() == id) {
            albumData.elementsByTagName("archive").item(0).removeChild(node);
            break;
        }
    }
}

void MainWindow::removeAlbumFromDatabase(QModelIndex index)
{
    model->removeRow(index.row());
}

void MainWindow::decreaseAlbumCount(QModelIndex artistIndex)
{
    int row = artistIndex.row();
    QModelIndex albumCountIndex = artistIndex.sibling(row, 2);
    int albumCount = albumCountIndex.data().toInt();

    QSqlTableModel *artists = model->relationModel(2);

    if (albumCount == 1) {
        artists->removeRow(row);
    } else {
        artists->setData(albumCountIndex, QVariant(albumCount - 1));
    }
}

void MainWindow::readAlbumData()
{
    if (!file->open(QIODevice::ReadOnly))
        return;

    if (!albumData.setContent(file)) {
        file->close();
        return;
    }
    file->close();
}

QGroupBox* MainWindow::createDaysGroupBox()
{
	calendar = new QCalendarWidget;
	calendar->setMinimumDate(QDate(1900, 1, 1));
	calendar->setMaximumDate(QDate(3000, 1, 1));
	calendar->setGridVisible(true);

    QGroupBox *box = new QGroupBox(tr("Days"));

    QGridLayout *layout = new QGridLayout;
    layout->addWidget(calendar, 0, 0);
    box->setLayout(layout);

    return box;
}

QGroupBox* MainWindow::createDetailsGroupBox()
{
    QGroupBox *box = new QGroupBox(tr("Details"));

    detailsView = new QTableView;
    detailsView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    detailsView->setSortingEnabled(true);
    detailsView->setSelectionBehavior(QAbstractItemView::SelectRows);
    detailsView->setSelectionMode(QAbstractItemView::SingleSelection);
    detailsView->setShowGrid(false);
    detailsView->verticalHeader()->hide();
    detailsView->setAlternatingRowColors(true);
    detailsView->setModel(model);
    adjustHeader();
	detailsView->setItemDelegateForColumn(1, new DateFormatDelegate("dd.MM.yyyy", this));
	detailsView->setItemDelegateForColumn(2, new TimeFormatDelegate("hh:mm", this));
	detailsView->setItemDelegateForColumn(3, new TimeFormatDelegate("hh:mm", this));

    QLocale locale = detailsView->locale();
    locale.setNumberOptions(QLocale::OmitGroupSeparator);
    detailsView->setLocale(locale);

    connect(detailsView, SIGNAL(clicked(QModelIndex)),
            this, SLOT(showAlbumDetails(QModelIndex)));
    connect(detailsView, SIGNAL(activated(QModelIndex)),
            this, SLOT(showAlbumDetails(QModelIndex)));

    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(detailsView, 0, 0);
    box->setLayout(layout);

    return box;
}

QGroupBox* MainWindow::createHoursGroupBox()
{
	DaysModel *hModel = new DaysModel(*model, this);

	hoursView = new QTableView;
	hoursView->setEditTriggers(QAbstractItemView::NoEditTriggers);
	hoursView->setSortingEnabled(true);
	hoursView->setSelectionBehavior(QAbstractItemView::SelectRows);
	hoursView->setSelectionMode(QAbstractItemView::SingleSelection);
	hoursView->setShowGrid(false);
	hoursView->verticalHeader()->hide();
	hoursView->setAlternatingRowColors(true);
	hoursView->setModel(hModel);
	//hoursView->hideColumn(0);
	hoursView->horizontalHeader()->setResizeMode(0, QHeaderView::Stretch);
	hoursView->resizeColumnToContents(1);	
	
	QDate current = QDate::currentDate();
	for(int i=0; i<hModel->rowCount(); i++)
	{
		QModelIndex index = hModel->index(i, 0);
		if(hModel->data(index).toDate() == current)
		{
			hoursView->setCurrentIndex(index);
			break;
		}
	}

	connect(hoursView, SIGNAL(clicked(QModelIndex)),
		this, SLOT(changeArtist(QModelIndex)));
	connect(hoursView, SIGNAL(activated(QModelIndex)),
		this, SLOT(changeArtist(QModelIndex)));

	QGroupBox *box = new QGroupBox(tr("Hours"));

	QGridLayout *layout = new QGridLayout;
	layout->addWidget(hoursView, 0, 0);
	box->setLayout(layout);

	return box;
}

void MainWindow::createMenuBar()
{
    QAction *addAction = new QAction(tr("&Add album..."), this);
    QAction *deleteAction = new QAction(tr("&Delete album..."), this);
    QAction *quitAction = new QAction(tr("&Quit"), this);
    QAction *aboutAction = new QAction(tr("&About"), this);
    QAction *aboutQtAction = new QAction(tr("About &Qt"), this);

    addAction->setShortcut(tr("Ctrl+A"));
    deleteAction->setShortcut(tr("Ctrl+D"));
    quitAction->setShortcuts(QKeySequence::Quit);

    QMenu *fileMenu = menuBar()->addMenu(tr("&File"));
    fileMenu->addAction(addAction);
    fileMenu->addAction(deleteAction);
    fileMenu->addSeparator();
    fileMenu->addAction(quitAction);

    QMenu *helpMenu = menuBar()->addMenu(tr("&Help"));
    helpMenu->addAction(aboutAction);
    helpMenu->addAction(aboutQtAction);

    connect(addAction, SIGNAL(triggered(bool)), this, SLOT(addAlbum()));
    connect(deleteAction, SIGNAL(triggered(bool)), this, SLOT(deleteAlbum()));
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
    //detailsView->hideColumn(0);
    detailsView->horizontalHeader()->setResizeMode(1, QHeaderView::Stretch);
    detailsView->resizeColumnToContents(2);
    detailsView->resizeColumnToContents(3);
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
	QString log = QDateTime::currentDateTime().toString();
	qDebug() <<"sleep";
	qDebug() <<log; 
	finalizeLastRecord();
}

void MainWindow::doWakeup()
{
	QString log = QDateTime::currentDateTime().toString();
	qDebug() <<"wakeup";
	qDebug() <<log; 
	addNewRecord();
}

void MainWindow::showAlbumDetails(QModelIndex index)
{
	QSqlRecord record = model->record(index.row());
}

int MainWindow::addNewRecord()
{
	int id = generateRecordId();
	QSqlRecord record;

	QSqlField f1("id", QVariant::Int);
	QSqlField f2("day", QVariant::UInt);
	QSqlField f3("start", QVariant::Int);
	QSqlField f4("end", QVariant::Int);

	f1.setValue(QVariant(id));
	QDateTime currentDate = QDateTime(QDate::currentDate());
	f2.setValue(QVariant(currentDate.toTime_t()));
	f3.setValue(QVariant(QDateTime::currentDateTime().toTime_t()));
	f4.setValue(QVariant(0));
	record.append(f1);
	record.append(f2);
	record.append(f3);
	record.append(f4);

	model->insertRecord(-1, record);

	return id;
}

void MainWindow::finalizeLastRecord()
{
	QDateTime currentDate = QDateTime(QDate::currentDate());
	for (int i = model->rowCount()-1; i >=0 ; i--) 
	{
		QSqlRecord record =  model->record(i);
		if (record.value("day") == currentDate.toTime_t())
		{
			QString val = record.value("end").toString();
			if(val == "0")
			{
				record.setValue("end", QDateTime::currentDateTime().toTime_t());
				val = record.value("end").toString();
				//model->setRecord(i, record);
				model->removeRow(i);
				model->insertRecord(-1, record);
				break;
			}
		}
	}

	for (int i = 0; i < model->rowCount(); i++) 
	{
		QSqlRecord record =  model->record(i);
		if (record.value("day") == currentDate.toTime_t())
		{
			QString val = record.value("end").toString();
			qDebug() <<i<<val;
		}
	}
}

int MainWindow::generateRecordId()
{
	uniqueAlbumId += 1;
	return uniqueAlbumId;
}

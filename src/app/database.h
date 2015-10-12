#ifndef __DATABASE_H__
#define __DATABASE_H__

#include <QMessageBox>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>

static bool createConnection()
{
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");

    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("D:\\Devel\\timetable.db");

    if(!QFile::exists("D:\\Devel\\timetable.db")) // Database does not exist yet
    {
        if (!db.open()) {
            QMessageBox::critical(0, qApp->tr("Cannot open database"),
                                  qApp->tr("Unable to establish a database connection.\n"
                                           "This example needs SQLite support. Please read "
                                           "the Qt SQL driver documentation for information how "
                                           "to build it.\n\n"
                                           "Click Cancel to exit."), QMessageBox::Cancel);
            return false;
        }

        QSqlQuery query;

        query.exec("create table hours (id int primary key, "
                   "week int, "
                   "day uint, "
                   "start int, "
                   "end int, "
                   "sum int )");
    }
    else // Database exists. Just open it
    {
        if (!db.open()) {
            QMessageBox::critical(0, qApp->tr("Cannot open database"),
                                  qApp->tr("Unable to establish a database connection.\n"
                                           "This example needs SQLite support. Please read "
                                           "the Qt SQL driver documentation for information how "
                                           "to build it.\n\n"
                                           "Click Cancel to exit."), QMessageBox::Cancel);
            return false;
        }
    }

    return true;
}

#endif // __DATABASE_H__

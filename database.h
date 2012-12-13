#ifndef DATABASE_H
#define DATABASE_H

#include <QMessageBox>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>

static bool createConnection()
{
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    //db.setDatabaseName(":memory:");

    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("timetable.db");

    if(!QFile::exists("timetable.db")) // Database does not exist yet
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
                   "day uint, "
                   "start int, "
                   "end int )");

        /*		query.exec("insert into hours values(1, '1325365200', 9, 16)");
        query.exec("insert into hours values(2, '1325365200', 17, 18)");
        query.exec("insert into hours values(3, '1325538000', 9, 10)");
        query.exec("insert into hours values(4, '1325538000', 11, 12)");
        query.exec("insert into hours values(5, '1325538000', 9, 19)");
        query.exec("insert into hours values(6, '1325710800', 8, 15)");
        query.exec("insert into hours values(7, '1325710800', 16, 18)");
        query.exec("insert into hours values(8, '1325710800', 19, 20)");
*/
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

#endif



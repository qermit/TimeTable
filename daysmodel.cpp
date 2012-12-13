#include <QDate>
#include <QtSql>
#include "daysmodel.h"

DaysModel::DaysModel(QAbstractTableModel& base, QObject *parent) : QAbstractTableModel(parent), _base(base)
{
}

int DaysModel::rowCount(const QModelIndex & /*parent*/) const
{
    return QDate::currentDate().daysInYear();
}

int DaysModel::columnCount(const QModelIndex & /*parent*/) const
{
    return 2;
}

QVariant DaysModel::data(const QModelIndex &index, int role) const
{
    int month = 1;
    int daysCount = 0;
    int day = 0;
    for(month=1; month<=12;month++)
    {
        if(index.row() + 1 - daysCount - QDate(QDate::currentDate().year(),month,1).daysInMonth() <= 0)
        {
            day = index.row() + 1 - daysCount;
            break;
        }
        daysCount += QDate(QDate::currentDate().year(),month,1).daysInMonth();
    }

    QDate date(QDate::currentDate().year(), month, day);

    if (role == Qt::DisplayRole)
    {
        if(index.column() == 0)
        {
            return date;
        }
        else
        {
            int seconds = calculateHours(date);
            return QString::fromAscii("%1:%2").arg(seconds/3600).arg(seconds/60);
        }
    }

    return QVariant();
}

int DaysModel::calculateHours(QDate date) const
{
    int res = 0;
    for(int i=0; i<_base.rowCount(); i++)
    {
        QSqlRecord record =  ((QSqlRelationalTableModel&)_base).record(i);
        QString ddd = QDateTime::fromTime_t(record.value("day").toUInt()).date().toString("dd-MM-yyyy");
        if(QDateTime::fromTime_t(record.value("day").toUInt()).date() == date)
        {
            QString val = record.value("end").toString();
            QString dattt = date.toString("dd-MM-yyyy");
            if(record.value("end").toUInt() > 0)
            {
                uint diff = record.value("end").toUInt() - record.value("start").toUInt();
                res+=diff;
            }
        }
    }

    /*    QSqlRelationalTableModel& model = (QSqlRelationalTableModel&)_base;
    int count = model.rowCount();
    model.setFilter("day = '" + date.toString() + "'");
    model.select();
    count = model.rowCount();
    for(int i=0; i<count; i++)
    {
        QSqlRecord record =  model.record(i);
        QString ddd = QDateTime::fromTime_t(record.value("day").toUInt()).date().toString("dd-MM-yyyy");
        if(QDateTime::fromTime_t(record.value("day").toUInt()).date() == date)
        {
            QString val = record.value("end").toString();
            QString dattt = date.toString("dd-MM-yyyy");
            if(record.value("end").toUInt() > 0)
            {
                uint diff = record.value("end").toUInt() - record.value("start").toUInt();
                res+=diff;
            }
        }
    }

    model.setFilter(QString());
*/
    return res;
}

int DaysModel::calculateHoursPerWeek(QDate date) const
{
    int res = date.weekNumber();

    /*    QSqlRelationalTableModel& model = (QSqlRelationalTableModel&)_base;
    int count = model.rowCount();
    model.setFilter("day <= '1355169600'");
    model.select();
    count = model.rowCount();
    if(model.select())
    {
        while (model.query().next())
        {
            int id = model.query().value(0).toInt();
            int year = model.query().value(1).toInt();
            qDebug() << id << " " << year << '\n';
        }
    }
*/
    return res;
}

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
            QTime time(seconds/3600, (seconds%3600)/60, (seconds%3600)%60);
            return time.toString("hh:mm:ss");
        }
    }

    return QVariant();
}

int DaysModel::calculateHours(const QDate& date) const
{
    int res = 0;
    QSqlRelationalTableModel& model = (QSqlRelationalTableModel&)_base;
    QString currFilter = model.filter();
    QDateTime dt(date);
    QString newFilter = "day = '" + QString::number(dt.toUTC().toTime_t(), 10) + "'";
    model.setFilter(newFilter);
    int count = model.rowCount();
    for(int i=0; i<count; i++)
    {
        QSqlRecord record =  model.record(i);
        if(record.value("end").toUInt() > 0)
        {
            uint diff = record.value("end").toUInt() - record.value("start").toUInt();
            res+=diff;
        }
    }

    int resUncompleted = calculateHoursFromUncompletedRecord();
    res += resUncompleted;

    model.setFilter(currFilter);

    return res;
}

int DaysModel::calculateHoursPerWeek(const QDate& date) const
{
    int res = 0;
    QSqlRelationalTableModel& model = (QSqlRelationalTableModel&)_base;
    QString currFilter = model.filter();
    QString newFilter = "week = '" + QString::number(date.weekNumber(), 10) + "'";
    model.setFilter(newFilter);
    model.select();
    int count = model.rowCount();
    for(int i=0; i<count; i++)
    {
        QSqlRecord record =  model.record(i);
        if(record.value("end").toUInt() > 0)
        {
            uint diff = record.value("end").toUInt() - record.value("start").toUInt();
            res+=diff;
        }
    }

    int resUncompleted = calculateHoursFromUncompletedRecord();
    res += resUncompleted;

    model.setFilter(currFilter);
    return res;
}

int DaysModel::calculateHoursFromUncompletedRecord() const
{
    int res = 0;
    QSqlRelationalTableModel& model = (QSqlRelationalTableModel&)_base;
    int count = model.rowCount();
    for (int i = count-1; i >=0 ; i--)
    {
        QSqlRecord record =  model.record(i);
        QDateTime dt(QDate::currentDate());
        if (record.value("day") == dt.toUTC().toTime_t())
        {
            QString val = record.value("end").toString();
            if(val == "0")
            {
                uint start = record.value("start").toUInt();
                uint end = QDateTime::currentDateTimeUtc().toTime_t();
                res = end - start;
                break;
            }
        }
    }

    return res;
}

#include <QDate>
#include "timedelegate.h"

DateFormatDelegate::DateFormatDelegate (QString dateFormat, QObject *parent = 0) : 
    QStyledItemDelegate(parent), _dateFormat(dateFormat)
{
}

QString DateFormatDelegate::displayText(const QVariant & value, const QLocale & locale ) const
{
    Q_UNUSED(locale);
    QDateTime date(QDateTime::fromTime_t(value.toUInt()));
    return date.toString(_dateFormat);
}

TimeFormatDelegate::TimeFormatDelegate (QString timeFormat, QObject *parent = 0) : 
    QStyledItemDelegate(parent), _timeFormat(timeFormat)
{
}

QString TimeFormatDelegate::displayText(const QVariant & value, const QLocale & locale ) const
{
    Q_UNUSED(locale);
    QString res = QString("");
    if(value.toUInt() != 0)
    {
        QDateTime date(QDateTime::fromTime_t(value.toUInt()));
        res = date.toString(_timeFormat);
    }
    return res;
}

#include <QDate>
#include "timedelegate.h"

DateFormatDelegate::DateFormatDelegate (QString dateFormat, QObject *parent = 0) : 
QStyledItemDelegate(parent), m_dateFormat(dateFormat)
{
}

QString DateFormatDelegate::displayText(const QVariant & value, const QLocale & locale ) const
{
	Q_UNUSED(locale);
	QDateTime date(QDateTime::fromTime_t(value.toUInt()));
	return date.toString(m_dateFormat);
}



TimeFormatDelegate::TimeFormatDelegate (QString timeFormat, QObject *parent = 0) : 
QStyledItemDelegate(parent), m_timeFormat(timeFormat)
{
}

QString TimeFormatDelegate::displayText(const QVariant & value, const QLocale & locale ) const
{
	Q_UNUSED(locale);
	QDateTime date(QDateTime::fromTime_t(value.toUInt()));
	return date.toString(m_timeFormat);
}
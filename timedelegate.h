#ifndef TIMEDELEGATE_H
#define TIMEDELEGATE_H

#include <QStyledItemDelegate>

class DateFormatDelegate : public QStyledItemDelegate
{
public:
	DateFormatDelegate (QString dateFormat, QObject *parent);

	virtual QString displayText(const QVariant & value, const QLocale & locale ) const;

private:
	QString m_dateFormat;
};


class TimeFormatDelegate : public QStyledItemDelegate
{
public:
	TimeFormatDelegate (QString timeFormat, QObject *parent);

	virtual QString displayText(const QVariant & value, const QLocale & locale ) const;

private:
	QString m_timeFormat;
};

#endif // TIMEDELEGATE_H

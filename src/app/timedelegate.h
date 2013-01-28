#ifndef __TIMEDELEGATE_H__
#define __TIMEDELEGATE_H__

#include <QStyledItemDelegate>

class DateFormatDelegate : public QStyledItemDelegate
{
public:
    DateFormatDelegate (QString dateFormat, QObject *parent);
private:
    virtual QString displayText(const QVariant & value, const QLocale & locale ) const;
private:
    QString _dateFormat;
};


class TimeFormatDelegate : public QStyledItemDelegate
{
public:
    TimeFormatDelegate (QString timeFormat, QObject *parent);
private:
    virtual QString displayText(const QVariant & value, const QLocale & locale ) const;
private:
    QString _timeFormat;
};

#endif // __TIMEDELEGATE_H__

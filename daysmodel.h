#ifndef __DAYS_MODEL__
#define __DAYS_MODEL__

#include <QAbstractTableModel>

class DaysModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    DaysModel(QAbstractTableModel& base, QObject *parent);
    int rowCount(const QModelIndex &parent = QModelIndex()) const ;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    int calculateHoursPerWeek(const QDate& date) const;
private:
    int calculateHours(const QDate& date) const;
    int calculateHoursFromUncompletedRecord(const QDate& date) const;
private:
    QAbstractTableModel& _base;
};

#endif // __DAYS_MODEL__
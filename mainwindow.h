#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QDomDocument>
#include <QMainWindow>
#include <QModelIndex>
#include <qcoreevent.h>

QT_BEGIN_NAMESPACE
class QFile;
class QGroupBox;
class QLabel;
class QListWidget;
class QSqlRelationalTableModel;
class QTableView;
class QEvent;
class QCalendarWidget;
QT_END_NAMESPACE
class DaysModel;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(const QString &hoursTable, QWidget *parent = 0);

    ~MainWindow();
public slots:
    void doSleep();
    void doWakeup();

private slots:
    void about();
    void changeDate(int row);
    void changeDate(QModelIndex index);
    void changeDate(const QDate& date);
    void updateHeader(QModelIndex, int, int);
private:
    void adjustHeader();
    QGroupBox *createDetailsGroupBox();
    QGroupBox *createDaysGroupBox();
    QGroupBox *createHoursGroupBox();
    void createMenuBar();
    int generateRecordId();
    int addNewRecord();
    void finalizeLastRecord();
    void updateDetails(const QDate& date);
    void updateWeekHours(const QDate& date);

private:
    int _newRecordId;
    QTableView* _detailsView;
    QCalendarWidget* _calendar;
    QTableView* _hoursView;
    QLabel*     _hoursPerWeek;
    QSqlRelationalTableModel* _model;
    DaysModel* _daysModel;
};

#endif

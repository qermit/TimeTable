#ifndef __MAINWINDOW_H__
#define __MAINWINDOW_H__

#include <QMainWindow>
#include <QDialog>
#include <QModelIndex>
#include <qcoreevent.h>
#include <QSystemTrayIcon>
#include <qdatetime.h>
#include <QTranslator>

QT_BEGIN_NAMESPACE
class QFile;
class QGroupBox;
class QLabel;
class QListWidget;
class QSqlRelationalTableModel;
class QTableView;
class QEvent;
class QCalendarWidget;
class QDateTime;
QT_END_NAMESPACE
class DaysModel;

class MainWindow : public QDialog
{
    Q_OBJECT
public:
    MainWindow(const QString &hoursTable, QWidget *parent = 0);
    ~MainWindow();
public slots:
    void doSleep();
    void doWakeup();
    virtual void setVisible(bool visible);
private slots:
    void about();
    void changeDate(int row);
    void changeDate(QModelIndex index);
    void changeDate(const QDate& date);
    void updateHeader(QModelIndex, int, int);
    void iconActivated(QSystemTrayIcon::ActivationReason reason);
    void workedHoursUpdate();
    void exportTo();
    void doRestore();
private:
    void createActions();
    void createTrayIcon();
    void adjustHeader();
    QGroupBox *createDetailsGroupBox();
    QGroupBox *createDaysGroupBox();
    QGroupBox *createHoursGroupBox();
    void createMenuBar();
    int generateRecordId();
    int addNewRecord(const QDateTime& dateTime);
    void finalizeLastRecord(const QDateTime& dateTime);
    void updateDetails(const QDate& date);
    void updateWeekHours(const QDate& date);
    void updateDayHours(const QDate& date);
    void changeEvent(QEvent* e);
    void initializeTranslators(const QString& locale);
    bool reporterPluginsExist();
private:
    QList<QTranslator*> _transList;
    int _newRecordId;
    QTableView* _detailsView;
    QCalendarWidget* _calendar;
    QTableView* _hoursView;
    QLabel*     _hoursPerWeek;
    QLabel*     _hoursPerDay;
    QSqlRelationalTableModel* _model;
    DaysModel* _daysModel;
    QAction *_restoreAction;
    QAction *_quitAction;
    QSystemTrayIcon *trayIcon;
    QMenu *trayIconMenu;
    QTimer* _workedHoursTimer;
    QDateTime _sleepStartTime;
    QRect _geometry;
};

#endif // __MAINWINDOW_H__

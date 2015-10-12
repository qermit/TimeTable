#ifndef EXPORTDIALOG_H
#define EXPORTDIALOG_H

#include <QtGui>
#include <QtWidgets>

class NCReport;

class ExportDialog : public QDialog
{
    Q_OBJECT
public:
    ExportDialog(QWidget *parent = 0);
    ~ExportDialog();

private slots:
    void doExport();
    void setAllRecords();
    void setRecordsFromInterval();

private:
    QDialogButtonBox* createButtons();
    QGroupBox* createInputWidgets();
    bool connectDB(const QString& id);

private:
    NCReport* _reporter;
    QDateEdit* _leftDate;
    QDateEdit* _rightDate;
};


#endif // EXPORTDIALOG_H

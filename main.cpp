
#include <QtGui>

#include "database.h"
#include "mainwindow.h"



int main(int argc, char *argv[])
{
    Q_INIT_RESOURCE(timetable);

    QApplication app(argc, argv);

    if (!createConnection())
        return 1;

    //QFile *timeDetails = new QFile("timetable.xml");
    MainWindow window("hours", NULL);
    window.show();
    return app.exec();
}

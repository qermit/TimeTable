
#include <QtGui>

#include "database.h"
#include "mainwindow.h"



int main(int argc, char *argv[])
{
    Q_INIT_RESOURCE(app);

    QApplication app(argc, argv);

    QTranslator qtTranslator;
    qtTranslator.load("qt_" + QLocale::system().name(),
                      QLibraryInfo::location(QLibraryInfo::TranslationsPath));
    app.installTranslator(&qtTranslator);

    QTranslator myTranslator;
    QString filename = "timetable_" + QLocale::system().name();
    bool res = myTranslator.load(filename);
    app.installTranslator(&myTranslator);

    if (!createConnection())
        return 1;

    //QFile *timeDetails = new QFile("timetable.xml");
    MainWindow window("hours", NULL);
    window.show();
    return app.exec();
}

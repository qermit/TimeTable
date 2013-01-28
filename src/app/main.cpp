#include <QtGui>

#include "database.h"
#include "mainwindow.h"


int main(int argc, char *argv[])
{
    Q_INIT_RESOURCE(app);

    QApplication app(argc, argv);
    if (!createConnection())
        return 1;

    MainWindow window("hours", NULL);
    window.show();
    return app.exec();
}

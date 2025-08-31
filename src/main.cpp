#include <QApplication>
#include "./data/Database.h"
#include "./data/Migrations.h"
#include "MainWindow.h"

int main(int argc, char **argv)
{
    QApplication app(argc, argv);
    if (!Database::init())
        return 1;
    Migrations::run();
    MainWindow w;
    w.setWindowTitle("Pagos");
    w.resize(1100, 700);
    w.show();
    return app.exec();
}
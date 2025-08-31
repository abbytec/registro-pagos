#include <QApplication>
#include <QMessageBox>
#include <QSqlError>
#include "data/Database.h"
#include "data/Migrations.h"
#include "ui/MainWindow.h"

int main(int argc, char **argv)
{
    QApplication app(argc, argv);

    if (!Database::init())
    {
        QMessageBox::critical(nullptr, "DB",
                              "No se pudo abrir la base de datos en:\n" + Database::dbPath());
        return 1;
    }
    auto db = Database::db();
    if (!db.isValid() || !db.isOpen())
    {
        QMessageBox::critical(nullptr, "DB",
                              "DB inválida o cerrada.\nDriver: " + db.driverName() +
                                  "\nError: " + db.lastError().text());
        return 1;
    }
    if (!Migrations::run())
    {
        QMessageBox::critical(nullptr, "DB", "Fallaron las migraciones.");
        return 1;
    }

    MainWindow w;
    w.setWindowTitle("Pagos");
    w.resize(1100, 700);
    w.show();
    return app.exec();
}

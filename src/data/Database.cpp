#include "Database.h"
#include <QStandardPaths>
#include <QDir>
QSqlDatabase Database::m_db;

QString Database::dbPath()
{
    const QString base = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir().mkpath(base);
    return base + "/pagos.db";
}
bool Database::init()
{
    if (QSqlDatabase::contains("main"))
    {
        m_db = QSqlDatabase::database("main");
        return true;
    }
    m_db = QSqlDatabase::addDatabase("QSQLITE", "main");
    m_db.setDatabaseName(dbPath());
    return m_db.open();
}
QSqlDatabase Database::db() { return m_db; }
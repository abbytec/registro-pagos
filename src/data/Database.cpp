#include "Database.h"
#include <QStandardPaths>
#include <QDir>
#include <QSqlError>
#include <QDebug>

QSqlDatabase Database::m_db;

QString Database::dbPath()
{
    const QString base = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    if (base.isEmpty())
    {
        QString home = qEnvironmentVariable("HOME", QDir::homePath());
        QDir().mkpath(home + "/.pagosapp");
        return home + "/.pagosapp/pagos.db";
    }
    QDir().mkpath(base);
    return base + "/pagos.db";
}

bool Database::init()
{
    if (QSqlDatabase::contains("main"))
    {
        m_db = QSqlDatabase::database("main");
        return m_db.isOpen();
    }
    m_db = QSqlDatabase::addDatabase("QSQLITE", "main");
    m_db.setDatabaseName(dbPath());
    if (!m_db.open())
    {
        qCritical() << "DB open fail:" << m_db.lastError().text() << "path:" << dbPath();
        return false;
    }
    return true;
}

QSqlDatabase Database::db() { return m_db; }

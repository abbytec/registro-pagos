#include "Migrations.h"
#include "Database.h"
#include <QtSql>
bool Migrations::run()
{
    QSqlQuery q(Database::db());
    auto exec = [&](const char *sql)
    { if(!q.exec(sql)) qWarning() << "SQL error:" << q.lastError(); };
    exec("PRAGMA foreign_keys = ON;");
    exec("CREATE TABLE IF NOT EXISTS app_config (key TEXT PRIMARY KEY, value TEXT NOT NULL);");
    exec("CREATE TABLE IF NOT EXISTS students ("
         "id INTEGER PRIMARY KEY AUTOINCREMENT,"
         "doc TEXT UNIQUE NOT NULL,"
         "full_name TEXT NOT NULL,"
         "created_at TEXT DEFAULT (datetime('now'))"
         ");");
    exec("CREATE TABLE IF NOT EXISTS payments ("
         "id INTEGER PRIMARY KEY AUTOINCREMENT,"
         "student_id INTEGER NOT NULL,"
         "amount REAL NOT NULL,"
         "paid_at TEXT NOT NULL,"
         "method TEXT,"
         "notes TEXT,"
         "FOREIGN KEY(student_id) REFERENCES students(id) ON DELETE CASCADE"
         ");");
    exec("CREATE INDEX IF NOT EXISTS idx_students_doc ON students(doc);");
    exec("CREATE INDEX IF NOT EXISTS idx_payments_student ON payments(student_id);");
    exec("CREATE INDEX IF NOT EXISTS idx_payments_paid_at ON payments(paid_at);");
    return true;
}
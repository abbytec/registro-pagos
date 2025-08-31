#pragma once
#include <QtSql>

class Database
{
public:
    static bool init();
    static QSqlDatabase db();
    static QString dbPath();

private:
    static QSqlDatabase m_db;
};
#pragma once
#include <QString>
#include <QSqlTableModel>
class StudentRepository
{
public:
    static bool add(const QString &doc, const QString &fullName, QString *error = nullptr);
    static int idByDoc(const QString &doc);
    static QSqlTableModel *model(QObject *parent = nullptr);
};
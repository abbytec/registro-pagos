#include "StudentRepository.h"
#include "Database.h"
#include <QtSql>

bool StudentRepository::add(const QString &doc, const QString &fullName, QString *error)
{
    const QString d = doc.trimmed();
    const QString n = fullName.trimmed();

    // Validación: ambos campos son obligatorios
    if (d.isEmpty() || n.isEmpty())
    {
        if (error)
            *error = QStringLiteral("Documento y nombre son obligatorios.");
        return false;
    }

    QSqlQuery q(Database::db());
    q.prepare("INSERT INTO students(doc, full_name) VALUES(?,?)");
    q.addBindValue(d);
    q.addBindValue(n);
    if (!q.exec())
    {
        if (error)
            *error = q.lastError().text();
        return false;
    }
    return true;
}
int StudentRepository::idByDoc(const QString &doc)
{
    QSqlQuery q(Database::db());
    q.prepare("SELECT id FROM students WHERE doc=?");
    q.addBindValue(doc);
    return (q.exec() && q.next()) ? q.value(0).toInt() : -1;
}
QSqlTableModel *StudentRepository::model(QObject *parent)
{
    auto *m = new QSqlTableModel(parent, Database::db());
    m->setTable("students");
    m->setEditStrategy(QSqlTableModel::OnManualSubmit);
    m->select();
    m->setHeaderData(1, Qt::Horizontal, "Documento");
    m->setHeaderData(2, Qt::Horizontal, "Nombre");
    return m;
}

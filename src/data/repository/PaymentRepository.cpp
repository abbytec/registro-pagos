#include "PaymentRepository.h"
#include "Database.h"
#include "LicenseManager.h"
#include <QtSql>

bool PaymentRepository::add(int studentId, double amount, const QDate &date, const QString &method, const QString &notes, QString *error)
{
    if (!LicenseManager::canAddMore())
    {
        if (error)
            *error = "Se alcanzó el límite de registros.";
        return false;
    }
    QSqlQuery q(Database::db());
    q.prepare("INSERT INTO payments(student_id, amount, paid_at, method, notes) VALUES(?,?,?,?,?)");
    q.addBindValue(studentId);
    q.addBindValue(amount);
    q.addBindValue(date.toString(Qt::ISODate));
    q.addBindValue(method);
    q.addBindValue(notes);
    if (!q.exec())
    {
        if (error)
            *error = q.lastError().text();
        return false;
    }
    return true;
}
QSqlQueryModel *PaymentRepository::all(QObject *parent)
{
    auto *m = new QSqlQueryModel(parent);
    m->setQuery("SELECT p.id, s.doc, s.full_name, p.amount, p.paid_at, p.method, p.notes "
                "FROM payments p JOIN students s ON s.id=p.student_id "
                "ORDER BY p.paid_at DESC",
                Database::db());
    return m;
}
QSqlQueryModel *PaymentRepository::search(const QString &t, QObject *parent)
{
    auto *m = new QSqlQueryModel(parent);
    QSqlQuery q(Database::db());
    q.prepare("SELECT p.id, s.doc, s.full_name, p.amount, p.paid_at, p.method, p.notes "
              "FROM payments p JOIN students s ON s.id=p.student_id "
              "WHERE s.doc LIKE ? OR s.full_name LIKE ? "
              "ORDER BY p.paid_at DESC");
    const QString like = "%" + t.trimmed() + "%";
    q.addBindValue(like);
    q.addBindValue(like);
    q.exec();
    m->setQuery(std::move(q));
    return m;
}
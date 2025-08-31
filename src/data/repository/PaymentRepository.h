#pragma once
#include <QString>
#include <QDate>
#include <QSqlQueryModel>

class PaymentRepository
{
public:
    static bool add(int studentId, double amount, const QDate &date, const QString &method, const QString &notes, QString *error = nullptr);
    static QSqlQueryModel *all(QObject *parent = nullptr);
    static QSqlQueryModel *search(const QString &queryText, QObject *parent = nullptr);
};
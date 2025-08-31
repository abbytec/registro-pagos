#pragma once
#include <QString>
#include <QDate>

struct LicenseInfo
{
    int limit = 100;
    QString name;
    QDate expiry;
    bool valid = false;
};

class LicenseManager
{
public:
    static LicenseInfo current();
    static bool applyKey(const QString &key, QString *error = nullptr);
    static int allowedLimit();
    static int usedRecords();
    static bool canAddMore();
    static QString currentKey();

    // compartidas con licgen
    static QByteArray signPayload(const QByteArray &payload);
    static QByteArray base64UrlEncode(const QByteArray &in);
    static QByteArray base64UrlDecode(const QByteArray &in);

private:
    static LicenseInfo parseAndVerify(const QString &key);
    static QString readConfig(const QString &k);
    static void writeConfig(const QString &k, const QString &v);
};

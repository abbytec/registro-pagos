#include "LicenseManager.h"
#include "./data/Database.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QCryptographicHash>
#include <QtSql>

static const QByteArray SECRET = QByteArrayLiteral("panchos");

QByteArray LicenseManager::base64UrlEncode(const QByteArray &in)
{
    return in.toBase64(QByteArray::Base64UrlEncoding | QByteArray::OmitTrailingEquals);
}
QByteArray LicenseManager::base64UrlDecode(const QByteArray &in)
{
    return QByteArray::fromBase64(in, QByteArray::Base64UrlEncoding);
}
QByteArray LicenseManager::signPayload(const QByteArray &payload)
{
    QByteArray data = SECRET + "." + payload;
    return QCryptographicHash::hash(data, QCryptographicHash::Sha256).toHex();
}
static QString cfgKey = "license_key";

QString LicenseManager::readConfig(const QString &k)
{
    QSqlQuery q(Database::db());
    q.prepare("SELECT value FROM app_config WHERE key=?");
    q.addBindValue(k);
    if (q.exec() && q.next())
        return q.value(0).toString();
    return {};
}
void LicenseManager::writeConfig(const QString &k, const QString &v)
{
    QSqlQuery q(Database::db());
    q.prepare("INSERT INTO app_config(key,value) VALUES(?,?) "
              "ON CONFLICT(key) DO UPDATE SET value=excluded.value;");
    q.addBindValue(k);
    q.addBindValue(v);
    q.exec();
}
LicenseInfo LicenseManager::parseAndVerify(const QString &key)
{
    LicenseInfo info;
    const auto parts = key.split('.');
    if (parts.size() != 2)
        return info;
    const QByteArray payloadB64 = parts[0].toUtf8();
    const QByteArray sigHex = parts[1].toUtf8();
    const QByteArray payload = base64UrlDecode(payloadB64);
    if (signPayload(payload) != sigHex)
        return info;

    QJsonParseError err;
    auto doc = QJsonDocument::fromJson(payload, &err);
    if (err.error != QJsonParseError::NoError || !doc.isObject())
        return info;
    const auto o = doc.object();
    info.limit = o.value("limit").toInt(100);
    info.name = o.value("name").toString();
    const auto expiryStr = o.value("expiry").toString();
    if (!expiryStr.isEmpty())
        info.expiry = QDate::fromString(expiryStr, Qt::ISODate);
    info.valid = info.limit > 0;
    if (info.expiry.isValid() && QDate::currentDate() > info.expiry)
        info.valid = false;
    return info;
}
LicenseInfo LicenseManager::current()
{
    const auto key = readConfig(cfgKey);
    if (key.isEmpty())
        return {};
    return parseAndVerify(key);
}
bool LicenseManager::applyKey(const QString &key, QString *error)
{
    auto info = parseAndVerify(key);
    if (!info.valid)
    {
        if (error)
            *error = "Clave inválida o expirada.";
        return false;
    }
    writeConfig(cfgKey, key);
    return true;
}
int LicenseManager::allowedLimit()
{
    auto info = current();
    return info.valid ? info.limit : 100;
}
int LicenseManager::usedRecords()
{
    QSqlQuery q(Database::db());
    q.exec("SELECT COUNT(*) FROM payments");
    return q.next() ? q.value(0).toInt() : 0;
}
bool LicenseManager::canAddMore() { return usedRecords() < allowedLimit(); }
QString LicenseManager::currentKey() { return readConfig(cfgKey); }
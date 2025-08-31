// tools/LicGen.cpp
#include <QCoreApplication>
#include <QJsonDocument>
#include <QJsonObject>
#include <QTextStream>
#include "LicenseManager.h"

int main(int argc, char **argv)
{
    QCoreApplication app(argc, argv);
    QTextStream out(stdout), in(stdin);
    QString name;
    int limit;
    QString expiry;
    out << "Titular (name): ";
    out.flush();
    name = in.readLine();
    out << "Límite (p.ej. 1000): ";
    out.flush();
    limit = in.readLine().toInt();
    out << "Expira (YYYY-MM-DD) o vacío: ";
    out.flush();
    expiry = in.readLine();

    QJsonObject o;
    o["name"] = name;
    o["limit"] = limit;
    if (!expiry.trimmed().isEmpty())
        o["expiry"] = expiry.trimmed();

    QByteArray payload = QJsonDocument(o).toJson(QJsonDocument::Compact);
    QByteArray b64 = LicenseManager::base64UrlEncode(payload);
    QByteArray sig = LicenseManager::signPayload(payload);
    out << "\nClave de licencia:\n"
        << (b64 + "." + sig) << "\n";
    return 0;
}

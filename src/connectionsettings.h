#ifndef CONNECTIONSETTINGS_H
#define CONNECTIONSETTINGS_H

#include <QString>
#include <QHash>

class ConnectionSettings{

private:
    QString driver;
    QString dbname;
    QString host;
    QString user;
    QString password;
    QHash<QString, QString> esquemas;

public:
    ConnectionSettings();
    bool loadSettings(const QString &settingsFile);

    QString getUser();
    QString getPass();
    QString getHost();
    QString getDbname();
    QString getDriver();
    QHash<QString, QString> getEsquemas();
};

#endif // CONNECTIONSETTINGS_H

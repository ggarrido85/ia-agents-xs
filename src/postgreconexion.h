#ifndef POSTGRECONEXION_H
#define POSTGRECONEXION_H

#include <QtSql>
#include "connectionsettings.h"

class PostgreConexion{

private:
    QSqlDatabase database;
    QHash<QString, QString> esquemas;

    QString bindValues(QString query, const QList<QString> &bindvalues);

public:
     PostgreConexion();
    ~PostgreConexion();

    int inicializar(bool etc=false);
    QSqlQuery execute(const QString &query);
    bool execute(QString query, const QList<QString> &bindValues);
    bool open(){
        if(!this->database.open()){
            qDebug()<<"Error al abrir la conexion con la BD" << this->database.lastError().text(); 
            return 0;
        }
        return 1;
    }
    void close(){this->database.close();}
    QString getEsquema(const QString &etiqueta);
};

#endif // POSTGRECONEXION_H

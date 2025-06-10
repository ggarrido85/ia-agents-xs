#include <QDebug>
#include "postgreconexion.h"

/**
 * Constructor del sistema
 *
 * @brief PostgreConexion::PostgreConexion
 */
PostgreConexion::PostgreConexion(){
}

/**
 * Destructor
 *
 * @brief PostgreConexion::~PostgreConexion
 */
PostgreConexion::~PostgreConexion(){
}

/**
 * Funcion ara inicializar conexion a postgresql
 *
 * @brief PostgreConexion::inicializar
 * @param etc
 * @return
 */
int PostgreConexion::inicializar(bool etc){
    ConnectionSettings cs;
    bool flag;
    QString file;
    if(!etc)
      file = "connection_settings.xml";
    else
      file = "/etc/Agentes/Resources/connection_settings.xml";

    qDebug() <<("** Fichero de configuracion asignado desde:")<<file<<"**\n";
    flag = cs.loadSettings(file);

    if(!flag) return 0;

    QString driver = cs.getDriver();
    this->database = QSqlDatabase::addDatabase(driver);
    this->database.setHostName(cs.getHost());
    this->database.setDatabaseName(cs.getDbname());
    this->database.setUserName(cs.getUser());
    this->database.setPassword(cs.getPass());
    this->esquemas = cs.getEsquemas();

    qDebug()<<"Conexion a BD establecida exitosamente";
    return 1;
}

/**
 * Obtener el esquema de configiracion
 *
 * @brief PostgreConexion::getEsquema
 * @param etiqueta
 * @return
 */
QString PostgreConexion::getEsquema(const QString &etiqueta){
    return esquemas.value(etiqueta);
}

/**
 * Ejecutar consulta SQL
 *
 * @brief PostgreConexion::execute
 * @param query
 * @return
 */
QSqlQuery PostgreConexion::execute(const QString &query){
    QSqlQuery q(this->database);
    this->database.transaction();
    /*bool ok = */q.exec(query);
    this->database.commit();
    return q;
}


/**
 * Ejecutar consulta parametrizada
 *
 * @brief PostgreConexion::execute
 * @param query
 * @param bindvalues
 * @return
 */
bool PostgreConexion::execute(QString query, const QList<QString> &bindvalues){
    QSqlQuery q(this->database);
    this->database.transaction();
    bool ok = q.exec(bindValues(query, bindvalues));
    this->database.commit();
    return ok;
}


/**
 * Parametrizar
 *
 * @brief PostgreConexion::bindValues
 * @param query
 * @param bindvalues
 * @return
 */
QString PostgreConexion::bindValues(QString query, const QList<QString> &bindvalues){
    QString qq = query;
    int count = qq.count('?');
    if(bindvalues.size()!=count) return qq;
    int indexof = 0;
    int i=0;
    while( (indexof=qq.indexOf('?',indexof)) != -1){
        qq.remove(indexof,1);
        qq.insert(indexof, bindvalues.at(i));
        i++;
    }
    return qq;
}

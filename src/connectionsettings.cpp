#include <QFile>
#include <QDebug>
#include <QtXml>
#include "connectionsettings.h"

/**
 * Constructor
 *
 * @brief ConnectionSettings::ConnectionSettings
 */
ConnectionSettings::ConnectionSettings(){
}

/**
 * Cargar configuracion
 *
 * @brief ConnectionSettings::loadSettings
 * @param settingsFile
 * @return
 */
bool ConnectionSettings::loadSettings(const QString &settingsFile){
    QFile file(settingsFile);

    if (!file.open(QIODevice::ReadOnly)) {
        qCritical()<<"Critical Error: Cannot open file "<<settingsFile;
        return false;
    }

    QDomDocument domTree;

    if ( !domTree.setContent( &file ) ) {
        qCritical()<<"Critical Error: Parsing error for file "<<settingsFile;
        file.close();
        return false;
    }

    file.close();

    QDomElement root = domTree.documentElement();
    QDomElement element;

    //obtener lo parametros de la conexion
    {
        element = root.firstChildElement("connection");

        if(element.isNull()) return false;

        this->driver = element.attribute("driver");
        this->dbname = element.attribute("dbname");
        this->host = element.attribute("host");
        this->user = element.attribute("user");
        this->password = element.attribute("password");
    }

    //obtener los esquemas
    {
        element = root.firstChildElement("esquemas");
        if(element.isNull()) return false;

        QDomNode node = element.firstChild();
        QDomText text;

        QString etiqueta, esquema;
        while(!node.isNull()){
            if(node.isElement()){
                etiqueta = node.nodeName();
                text = node.firstChild().toText();
                if(!text.isNull()){
                    esquema = text.nodeValue();
                }
                else{
                    esquema = "";
                }
                this->esquemas.insert(etiqueta, esquema);
            }
            node = node.nextSibling();
        }
    }

    return true;
}

/**
 * Obtener usuario
 *
 * @brief ConnectionSettings::getUser
 * @return
 */
QString ConnectionSettings::getUser(){return user;}

/**
 * Obtener password
 *
 * @brief ConnectionSettings::getPass
 * @return
 */
QString ConnectionSettings::getPass(){return password;}

/**
 * Obtener host
 *
 * @brief ConnectionSettings::getHost
 * @return
 */
QString ConnectionSettings::getHost(){return host;}

/**
 * Obtener nombre de la base de datos
 *
 * @brief ConnectionSettings::getDbname
 * @return
 */
QString ConnectionSettings::getDbname(){return dbname;}

/**
 * Obtener conector
 *
 * @brief ConnectionSettings::getDriver
 * @return
 */
QString ConnectionSettings::getDriver(){return driver;}

/**
 * Obtener esquema
 *
 * @brief ConnectionSettings::getEsquemas
 * @return
 */
QHash<QString, QString> ConnectionSettings::getEsquemas(){return esquemas;}

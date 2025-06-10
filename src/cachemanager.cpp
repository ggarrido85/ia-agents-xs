#include <QFile>
#include <QDebug>
#include "cachemanager.h"

CacheManager::CacheManager(){
    iniciada = false;
}

CacheManager::~CacheManager(){
    qDebug()<<"Eliminada cache:  "<<cachePath;
    QFile::remove(cachePath);

}


QList<TypeObjective> CacheManager::get_objetivos(){
    return objetivos;
}

QHash<QString, QString> CacheManager::get_configuracion(){
    return configuracion;
}

int CacheManager::get_tiempoejecucion(){
    QString s;
    if( (s=configuracion.value("tiempoejecucion")) != "" )  return s.toInt();
    else return DEFAULT_EXEC_TIME;
}

double CacheManager::get_tolerancia(){
    return configuracion.value("tolerancia").toDouble();
}

int CacheManager::get_semejantes(){
    return configuracion.value("semejantes").toInt();
}

QString CacheManager::get_ConsultaActualizarClasificacion( int who ){
    if( who == 0 )
        return configuracion.value("consultaactclasificacion");
    else
        return configuracion.value("consultaactclasificacion_2");
}

QString CacheManager::get_ConsultaActualizaRepporteDia( ){
     return configuracion.value("consultaactreporte");
}

/**
 * Consulta para actualizar datos de la tabla historica
 *
 * @brief CacheManager::get_ConsultaHistorico
 * @return
 */
QString CacheManager::get_ConsultaHistorico( ){
     return configuracion.value("consulta_historico");
}

QString CacheManager::get_ConsultaObtenerDatos(){
    return configuracion.value("consultaobtenerdatos");
}

QString CacheManager::get_ficherosalida(){
    return configuracion.value("ficherosalida");
}

TypeObjective CacheManager::get_tipoobjetivoAt(int index){
    return objetivos.at(index);
}

int CacheManager::loadDataFromXmlCacheFile(){
    qDebug()<<"Cargando los datos en memoria...";

    QFile file(cachePath);

    if (!file.open(QIODevice::ReadOnly)) {
        qCritical()<<"Critical Error: No se puede abrir el archivo "<<cachePath;
        return ERROR_LOADING_CACHE;
    }

    QDomDocument doc;

    if ( !doc.setContent( &file ) ) {
        qCritical()<<"Critical Error: No es posible parsear el archivo "<<cachePath;
        file.close();
        return false;
    }

    file.close();

    QDomElement root = doc.documentElement();
    QDomNode node;

    node = root.firstChild();
    bool oneWasProc = false;
    while(!node.isNull()){
        if(node.isElement()){
            if(node.nodeName()=="configuracion"){
                QDomElement confElem = node.toElement();

                procesarConfiguracion(confElem);

                if(oneWasProc) break; else oneWasProc=true;
            } else if(node.nodeName()=="tiposobjetivos"){
                QDomElement tipObjElem = node.toElement();

                procesarTiposObjetivos(tipObjElem);
    
                if(oneWasProc) break; else oneWasProc=true;
            }
        }
        node = node.nextSibling();
    }

    qDebug()<<">>Datos cargados en memoria exitosamente.";
    return CACHE_LOADED;
}

void CacheManager::procesarConfiguracion(const QDomElement &confElem){
    QString atributo, valor;
    QDomText text;
    QDomNode node = confElem.firstChild();
    while(!node.isNull()){
        if(node.isElement()){
            atributo = node.nodeName();
            text = node.firstChild().toText();
            if(!text.isNull()) valor=text.nodeValue(); else valor="";
            configuracion.insert(atributo, valor);
        }
        node = node.nextSibling();
    }
}

void CacheManager::procesarTiposObjetivos(const QDomElement &tipObjElem){
    QString atributo, valor;
    QDomText text;
    QDomNode nodeSon;
    QDomNode node = tipObjElem.firstChild();
    while(!node.isNull()){
        if(node.isElement() && node.nodeName()=="tipoobjetivo"){

            TypeObjective t;

            nodeSon = node.firstChild();
            while(!nodeSon.isNull()){
                if(nodeSon.isElement()){
                    atributo = nodeSon.nodeName();
                    text = nodeSon.firstChild().toText();
                    if(!text.isNull()) valor=text.nodeValue(); else valor="";

                    t.atributos.insert(atributo, valor);

                }
                nodeSon = nodeSon.nextSibling();
            }

            objetivos.append(t);

        }
        node = node.nextSibling();
    }
}

void CacheManager::set_chequeCodigo(int i, ChequeoCodigo cc){
    objetivos[i].check = cc;
}

ChequeoCodigo CacheManager::get_chequeCodigo(int i){
    return objetivos[i].check;
}

QString CacheManager::get_atributo_objetivo(int k, QString atributo){
    return objetivos[k].get_valor(atributo);
}

int CacheManager::generateCache(PostgreConexion &postgreConx){
    qDebug()<<"Generando la cache y cargando los datos en memoria...";

    QString schema = postgreConx.getEsquema("ia");
    QString query;
    QSqlQuery qResult;

    bool ok = true;

    try{
        //XML
        QDomDocument doc;
        QDomNode header = doc.createProcessingInstruction("xml", "version=\"1.0\" encoding=\"UTF-8\"");
        doc.insertBefore(header, doc.firstChild());

        QDomElement root = doc.createElement("seobjetivos");
        doc.appendChild(root);

        QDomElement element;
        QDomElement childElement;
        QDomElement grandsonElement;

        element = doc.createElement("configuracion");
        root.appendChild(element);

        QString temporal, atributo, valor;

        //obteniendo los elementos de configuracion
        {
            configuracion.clear();

            query = "SELECT denominacion,valor FROM "+schema+".conf_atributosconfig";
            qResult = postgreConx.execute(query);

            while(qResult.next()){
                temporal = qResult.value(0).toString();
                atributo = temporal;
                childElement = doc.createElement(temporal); //xml

                temporal = qResult.value(1).toString();
                valor = temporal;
                childElement.appendChild(doc.createTextNode(temporal));//xml

                configuracion.insert(atributo, valor);
                element.appendChild(childElement);//xml
            }
        }

        qResult.clear();

        element = doc.createElement("tiposobjetivos");
        root.appendChild(element);

        //obteniendo los datos de los tipos de objetivos
        {
            objetivos.clear();

            query = configuracion.value("consultatipoobjetivo");
            if(query == ""){
                qCritical()<<"Falta el atributo de configuracion 'consultatipoobjetivo'";
                return ERROR_GENERATING_CACHE;
            }

            qResult = postgreConx.execute(query);
            if(!qResult.isActive()){
                qCritical()<<"Error al obtener los tipos de objetivos";
                return ERROR_GENERATING_CACHE;
            }

            QSqlRecord record;

            while(qResult.next()){
                TypeObjective t_objetivo;
                childElement = doc.createElement("tipoobjetivo"); //xml

                record = qResult.record();
                for(int i=0; i<record.count(); i++){
                    temporal = record.fieldName(i);
                    atributo = temporal;
                    valor = record.value(i).toString();

                    grandsonElement = doc.createElement(atributo);//xml
                    grandsonElement.appendChild(doc.createTextNode(valor));//xml
                    childElement.appendChild(grandsonElement);//xml

                    t_objetivo.atributos.insert(atributo, valor);
                }

                objetivos.append(t_objetivo);
                element.appendChild(childElement); //xml
            }
        }

        QFile io(cachePath);

        if(!io.open(QIODevice::WriteOnly | QIODevice::Text)){
            qCritical()<<">>Error al escribir en el fichero de la cache:"<<cachePath<<".";
            ok = false;
        }
        else if(!io.isWritable()){
            qCritical()<<"Error. El archivo de la cache no permite escritura:"<<cachePath<<".";
            io.close();
            ok = false;
        }
        else{

            io.setPermissions(io.permissions() | QFile::ReadOther | QFile::WriteOther);

            QByteArray str;
            str.append(doc.toString());
            if(io.write(str)!=str.count()){
                qCritical()<<"Error. Contenido de la cache incompleto:"<<cachePath<<".";
                ok = false;
            }
            io.close();
        }

    }
    catch(std::exception e){
        qCritical()<<"Error generando la cache: "<<e.what(); return ERROR_GENERATING_CACHE;
    }


    if(ok)     qDebug()<<">>Cache generada exitosamente. Datos cargados en memoria.";
    else       qDebug()<<">>Datos cargados en memoria.";

    return CACHE_UPDATED;
}

int CacheManager::init_updateCache(PostgreConexion &postgreConx, QString filtroObjetivos){
    int respuesta = CACHE_OK;
    try{

        if(!iniciada){
            qDebug()<<"Iniciando la cache...";

            QString query = "SELECT valor FROM "+postgreConx.getEsquema("ia")+".conf_atributosconfig WHERE denominacion='ficherocache'";
            QSqlQuery result = postgreConx.execute(query);

            if(!result.isActive()){
                qCritical()<<"Critical Error: Fallo la consulta de obtener el directorio de la cache.";
                return ERROR_INIT_UPDATE_CACHE;
            }
            result.next();
            cachePath = result.value(0).toString() + "_" + filtroObjetivos;
            // Crear fichero de configuracion para el manejor web
            crearFicheroConfig(result.value(0).toString());

            result.finish();

            QFile f(cachePath);
            if(!f.open(QIODevice::ReadOnly)){
                qDebug()<<"Cache no encontrada en: "<<cachePath;
                respuesta = generateCache(postgreConx);
            }
            else{
                qDebug()<<"Cache encontrada en: "<<cachePath;
                respuesta = loadDataFromXmlCacheFile();
            }
            f.close();
            iniciada = true;
        }
        else{
            qDebug()<<"Comprobando actualizacion de la cache...";
            QFile f(cachePath);
            if(!f.open(QIODevice::ReadOnly)){
                qDebug()<<"Cache no encontrada en: "<<cachePath;
                objetivos.clear();
                configuracion.clear();
                respuesta = generateCache(postgreConx);
            }
            else{
                qDebug()<<">>La cache se encuentra actualizada.";
            }
            f.close();
        }
    }
    catch(std::exception e){
        qCritical()<<"Error al iniciar/actualizar la cache: "<<e.what(); return ERROR_INIT_UPDATE_CACHE;
    }

    return respuesta;
}

/**
 * Crear fichero de configuracion para el manejo web
 *
 * @brief CacheManager::crearFicheroConfig
 * @param cacheDir
 */
void CacheManager::crearFicheroConfig (QString cacheFile) {
    QString dir = cacheFile.replace(cacheFile.indexOf("cache_seobjetivos.xml"),21,"");
    //dir = cacheFile;
    QString fileConfig = "../web/config/config.ini";
    QFile io(fileConfig);

    if(!io.open(QIODevice::WriteOnly | QIODevice::Text)){
        qCritical()<<">>Error al escribir en el fichero de la configuracion:"<<fileConfig<<".";
    }
    else if(!io.isWritable()){
        qCritical()<<"Error. El archivo de la configuracion no permite escritura:"<<fileConfig<<".";
        io.close();

    }
    else{

        io.setPermissions(io.permissions() | QFile::ReadOther | QFile::WriteOther);
        QString configuracion = " clasificacionFile =\"" + dir + "\";";
        QByteArray str;
        str.append(configuracion);
        if(io.write(str)!=str.count()){
            qCritical()<<"Error. Contenido de la cache incompleto:"<<fileConfig<<".";
        }
        io.close();
    }

}



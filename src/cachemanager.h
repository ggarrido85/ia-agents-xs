#ifndef CACHEMANAGER_H
#define CACHEMANAGER_H

#include <QDomElement>
#include "typeobjective.h"
#include "postgreconexion.h"

class CacheManager{

private:
    QHash<QString, QString> configuracion;
    QList<TypeObjective> objetivos;

    QString cachePath;
    bool iniciada;

    /* ############ METODOS PRIVADOS ############# */

      /* Carga los datos de la cache en memoria */
    int loadDataFromXmlCacheFile();

       /* Carga las variables de configuracion */
    void procesarConfiguracion(const QDomElement &confElem);

           /* Carga los tipos de objetivos */
    void procesarTiposObjetivos(const QDomElement &tipObjElem);

    /*
       Genera la cache en caso de que no exista el archivo cache
       especificado en el fichero de configuracion
   */
    int generateCache(PostgreConexion &postgreConx);

    void crearFicheroConfig(QString);
    /* ########################################### */

public:

    /* ###############  CONSTANTES ############### */
    static const int CACHE_OK = 1;
    static const int CACHE_UPDATED = 2;
    static const int CACHE_LOADED = 3;
    static const int ERROR_INIT_UPDATE_CACHE = 4;
    static const int ERROR_LOADING_CACHE = 5;
    static const int ERROR_GENERATING_CACHE = 6;
    static const int DEFAULT_EXEC_TIME = 3;
    /* ########################################### */

    ~CacheManager();
    CacheManager();

    QList<TypeObjective> get_objetivos();
    QHash<QString, QString> get_configuracion();
    int get_tiempoejecucion();
    double get_tolerancia();
    int get_semejantes();
    QString get_ConsultaActualizarClasificacion(int who = 0);
    QString get_ConsultaObtenerDatos();
    QString get_ConsultaActualizaRepporteDia();
    QString get_ConsultaHistorico();


    QString get_ficherosalida();
    TypeObjective get_tipoobjetivoAt(int index);

    int size_tipoobjetivo(){
        return objetivos.size();
    }

    void set_chequeCodigo(int, ChequeoCodigo);
    ChequeoCodigo get_chequeCodigo(int i);

    /*
      Metodo que inicia/actualiza los datos de la cache. En caso de no haber
      sido actualizada los datos se mantienen igual.En caso contrario se genera
      nuevamente la cache y se cargan los datos en memoria.
    */
    int init_updateCache(PostgreConexion &postgreConx,QString filtroObjetivos);

    QString get_atributo_objetivo(int, QString);


};

#endif // CACHEMANAGER_H

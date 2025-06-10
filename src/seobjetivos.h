#ifndef SEOBJETIVOS_H
#define SEOBJETIVOS_H

#include "postgreconexion.h"
#include "dataobjective.h"
#include "cachemanager.h"
#include "polaca.h"

#define MIN(x,y) (x < y ? (x) : (y))

typedef struct{
    DataObjective* objetivo;        // Avion
    int tipo;                       /*
                                       Tipo de objetivo para el cual se le evaluara la polaca. Es el indice correspondiente
                                       al tipo de objetivo dentro del arreglo de TypeObjective de la cache.
                                    */
    double distancia;               // Resultado de evaluar la polaca del tipo de objetivo en el avion.
}registro_objetivo;


class SEObjetivos{

private:

    CacheManager cache;
    Polaca p;
    QList< QList<elemento_polaca> >  polacas;
    QString filtrosObjetivos;
    int iteracion;

    void getSemejantes(QList<registro_objetivo> &temporal, int cantSemejantes);


public:
    PostgreConexion pg;
    SEObjetivos();

    int init(bool etc);
    int checkCacheStatus_ConstructPostfijas(QString);
    int clasificar(bool,QString);

    void clasificar_prueba(QList<DataObjective> &aviones, QList<registro_objetivo> salida[], bool flag);
    void generarJson(QList<registro_objetivo> salida[], int size, QString filtroObjetivos);

    int getSleepTime();
    int size_cache();

};

#endif // SEOBJETIVOS_H

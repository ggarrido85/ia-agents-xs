#include "seobjetivos.h"

SEObjetivos::SEObjetivos(){
    iteracion = 1;
}

int SEObjetivos::getSleepTime(){
    return cache.get_tiempoejecucion();
}

int SEObjetivos::size_cache(){
    return cache.size_tipoobjetivo();
}

void SEObjetivos::getSemejantes(QList<registro_objetivo> &temporal, int cantSemejantes){
    QList<registro_objetivo> result;
    double dist = 2;
    int index = 0;
    for(int i=0; i<cantSemejantes; i++){
        for(int j=0; j<temporal.size(); j++){
            if(temporal.at(j).distancia < dist){
                dist = temporal.at(j).distancia;
                index = j;
            }
        }

        result.append(temporal.at(index));
        temporal.erase(temporal.begin()+index);
        dist = 2;
    }
    temporal = result;
}

int SEObjetivos::init(bool etc){
    return pg.inicializar(etc);
}

int SEObjetivos::checkCacheStatus_ConstructPostfijas( QString filtroObjetivos){
    filtrosObjetivos = filtroObjetivos;
    int cacheStatus = cache.init_updateCache(pg,filtroObjetivos);
    int result = 0;

    switch(cacheStatus){
    case CacheManager::CACHE_OK:
        qDebug()<<"- Vector postfija de los tipos de objetivos ya ha sido generado.";
        result = 1;
        break;

    case CacheManager::CACHE_LOADED:
    case CacheManager::CACHE_UPDATED:
        qDebug()<<"- Generando vector postfija de los tipos de objetivos...";
        polacas.clear();
        int n = cache.get_objetivos().size();

        QString expresion;
        Tokenizer tokenizer;
        TypeObjective* actual;

        QList<TypeObjective> objetivos = cache.get_objetivos();
        for(int i=0; i<n; i++){
            actual = &(objetivos[i]);
            expresion = actual->get_valor("expresion");
            QList<elemento_polaca> postfija;
            tokenizer.parsePostfijaString(expresion, p.getOperadores(), postfija);
            polacas.append(postfija);

            QStringList lista = expresion.split(",");
            ChequeoCodigo cc(lista);
            cache.set_chequeCodigo(i, cc);
        }
        result = 1;
        qDebug()<<"- Vector postfija de los tipos de objetivos generado exitosamente.";
        break;
    }
    return result;
}

/**
 * Funcion de evaluacion
 *
 * @brief SEObjetivos::clasificar
 * @param flag
 * @return
 */
int SEObjetivos::clasificar(bool flag, QString filtroObjetivos){

    qDebug()<<"- Realizando la clasificacion...";

	// Buscar los datos (Buscar hechos)
    QSqlQuery queryResult;
    QString queryAux = cache.get_ConsultaObtenerDatos();
    // Cambiar valores de parametro en timestamp
    qint64 tiempoActual = QDateTime::currentMSecsSinceEpoch()/1000 ;
    queryAux.replace("params_time", QString::number( tiempoActual ));
    if(filtroObjetivos == "")
        filtroObjetivos = "0,1,2,3,4,5,6,7,8,9";
    queryAux.replace("params_filterid", filtroObjetivos);

    // Imprimir la consulta de obtencion de datos
    //qDebug()<<"- -  "<<queryAux<<"----\n";
    
    // Cantidad de resultados maximos a devolver
    int n_semejante = cache.get_semejantes();

    queryResult = pg.execute(queryAux);

    int n_objetivos = queryResult.size();

    if(n_objetivos == 0 || n_objetivos == -1){
        qDebug()<<"- - No hay objetivos que clasificar.";
        return 0;
    }

    qDebug()<<"- - Hay"<<n_objetivos<<"localizaciones sin clasificar.";

    int n = polacas.size();

    QString idobjetivo, idtrama, codigo, tipoobjetivo;
    QList<registro_objetivo> temporal;
    QList<DataObjective> datos;
    QList<registro_objetivo> salida[n+1];
    // Consulta SQL para actualizar en la taba de objetivos
    QString sqlConsultaAct_2 = cache.get_ConsultaActualizarClasificacion(2);
    QString query = cache.get_ConsultaActualizarClasificacion();
    QString sqlReporte = cache.get_ConsultaActualizaRepporteDia();
    QString sqlHistorico = cache.get_ConsultaHistorico();

    /*
       'salida' es un arreglo de QList<registro_objetivo> que almacenara en la posicion 'i' la lista de aviones que se clasificaron
       como el TypeObjective correspondiente a la posicion 'i' del QList 'objetivos' de la clase 'CacheManager'. 'salida[n]' se
       correspondera con la lista de los aviones que no pudieron ser clasificados en ninguno de los tipos de objetivos especificados.
    */

    double distancia;

    /********** MIO *********/
    QString idtrama_old = "";
    int clasif_trama, no_clasif_trama,total=0;
    /************************/

    QSqlRecord record;
    QSqlField field;
    while(queryResult.next()){
        DataObjective dataObjective;
        record = queryResult.record();
        int n_items = record.count();
        for(int i = 0; i < n_items; i++) {
            field = record.field(i);
            dataObjective.atributos[field.name()] = field.value().toString();
        }
        datos.append(dataObjective);
    }

    DataObjective* actual;
    for(int i=0; i<n_objetivos; i++){              //Recorre uno a uno los aviones del espacio aereo
        actual = &(datos[i]);

        idobjetivo = actual->atributos["idobjetivo"];
        idtrama = actual->atributos["idtrama"];
        codigo = actual->atributos["codigo"];

        if(idtrama_old != idtrama){
            if(!idtrama_old.isEmpty()){
                total += clasif_trama;
                qDebug()<< "- - -" << clasif_trama << " objetivos fueron clasificados en tipos definidos";
                qDebug()<< "- - -" << no_clasif_trama << " objetivos no pudieron ser asociados a tipos definidos";
            }
            QDateTime timestamp;
            timestamp.setTime_t(idtrama.toInt());
            qDebug()<<"- - Clasificando objetivos en la trama: "<<  timestamp.toString(Qt::SystemLocaleShortDate);
            idtrama_old = idtrama;
            clasif_trama = 0;
            no_clasif_trama = 0;
        }

        // Evaluo al objetivo para cada uno de los tipos de objetivos
        for(int j=0; j<n; j++){

            // Esto es para no aplicarle la evalucion en polaca a un objetivo que su codigo no se encuentre entre los codigos del posible tipo de objetivo en que se intenta clasificar
            //if(flag || cache.get_objetivos()[j].check.chequear(codigo.toInt())){

                distancia = p.comunEval(polacas.at(j), *actual, pg);
                // qDebug()<<"- - Distancia "<<distancia<<" ."<<"  Codigo: "<<codigo;
                // Solo valores de distancia entre [0,tolerancia)
                if(distancia>=0 && distancia<=cache.get_tolerancia()){
                    registro_objetivo clasificado;
                    clasificado.objetivo = actual;
                    clasificado.tipo = j;
                    clasificado.distancia = distancia;
                    temporal.append(clasificado); 
                    // Optimizacion: Si la distancia es 0 verificar que no pase la cantidad de semejantes
                    if( distancia == 0 && n_semejante <= temporal.size() ) {
                        // Mandar a salir del ciclo
                        j = n;
                    }
                }
            //}
        }

        // Si 'temporal' esta vacio fue que el avion no cumplio con la regla de ninguno de los tipos de objetivos especificados
        if(temporal.empty()){

            if( query != ""  ) {
                // Update dat_localizacion en BD
                QList<QString> bindvalues;
                bindvalues.append("NPC");       // NPC: No Podido Clasificar
                bindvalues.append(idobjetivo);
                bindvalues.append(idtrama);
                pg.execute(query, bindvalues);    //Actualiza BD
            }

            QString updateIaNPC = "UPDATE mod_monitoreo.dat_localizacionterr  set  clasificacionesia =  '-1'  WHERE idobjetivo = '?' AND tiempo = '?' ";

            if( updateIaNPC != "") {
                QList<QString> bindupdateIaNPC;
                //bindupdateIa.append("-1");
                bindupdateIaNPC.append(idobjetivo);
                bindupdateIaNPC.append(idtrama);

                // Actualizar reporte
                bool qr = pg.execute( updateIaNPC , bindupdateIaNPC);    //Actualiza BD
                if(!qr){
                    qCritical()<<"Error al ejecutar la funcion en BD."<<updateIaNPC<<bindupdateIaNPC ;
                }
            }

            registro_objetivo no_clasificado;
            no_clasificado.objetivo = actual;
            no_clasificado.tipo = n;
            no_clasificado.distancia = 0;

            salida[n].append(no_clasificado);
            no_clasif_trama++;

//VERIFICACION
/*
            int longitud = salida[n].size();
            qDebug() << "NPC";
            for(int k=0; k<longitud; k++){
                qDebug() << salida[n].at(k).objetivo->atributos["idobjetivo"] + ", " + salida[n].at(k).objetivo->atributos["codigo"];
            }
*/
//############

        }
        else{

            // Obteniendo semejantes
            getSemejantes(temporal, MIN(temporal.size(), cache.get_semejantes()));
/*
            Despues del llamado a esta funcion 'temporal' sale solamente con el mas semejante, el de menor
            valor de distancia. La cantidad de semejantes a dejar se especifica en el fichero cache, que es 1.
            Si la cantidad de semejantes especificada fuera mayor que 1, y a su vez mayor que la cantidad de
            elementos que contiene 'temporal' entonces todos los elementos de 'temporal' se conservarian para
            el posterior analisis.
*/
            // Listado de parametros para la consulta de actualizacion
            QList<QString> bindvalues;
            QString clasificacionesia;
            for(int k=0, temporalSize = temporal.size(); k < temporalSize; k++){

                //if(k==0)
                    clasificacionesia.append(cache.get_atributo_objetivo(temporal[k].tipo, "idtipoobjetivo") + ",");
                /*else
                    clasificacionesia.append( "," + cache.get_atributo_objetivo(temporal[k].tipo, "idtipoobjetivo")  );*/


                // Si es la posicon 0 actualizar el tiempo real la trama o localizacion
                if( k == 0 && query != ""  ) {
                    bindvalues.clear();
                    bindvalues.append(cache.get_atributo_objetivo(temporal[k].tipo, "denominacion"));
                    bindvalues.append(idobjetivo);
                    bindvalues.append(idtrama);
                    pg.execute(query, bindvalues);    //Actualiza BD
                }

                // Si no hay reporte

                // Registrar en el sistema de salida
                salida[temporal[k].tipo].append(temporal[k]);

//VERIFICACION
/*
                int longitud = salida[temporal[k].tipo].size();
                int t = temporal[k].tipo;
                qDebug() << tipoobjetivo;
                for(int h=0; h<longitud; h++){
                    qDebug() << salida[t].at(h).objetivo->atributos["idobjetivo"] + ", " + salida[n].at(h).objetivo->atributos["codigo"];
                }
*/
//############

            }

            QString updateIa = "UPDATE mod_monitoreo.dat_localizacionterr  set  clasificacionesia =  '?'  WHERE idobjetivo = '?' AND tiempo = '?' ";

            if( updateIa != "") {
                QList<QString> bindupdateIa;
                bindupdateIa.append(clasificacionesia);
                bindupdateIa.append(idobjetivo);
                bindupdateIa.append(idtrama);

                // Actualizar reporte
                bool qr = pg.execute( updateIa , bindupdateIa);    //Actualiza BD
                if(!qr){
                    qCritical()<<"Error al ejecutar la funcion en BD."<<updateIa<<bindupdateIa ;
                }
            }

            clasif_trama++;
            temporal.clear();

        }

    }
    total += clasif_trama;
    // Resultado de la clasificacion de la ultima trama.
    qDebug()<< "- - -" << clasif_trama << " objetivos fueron clasificados en tipos definidos";
    qDebug()<< "- - -" << no_clasif_trama << " objetivos no pudieron ser asociados a tipos definidos";


    // Update dat_avion en BD

    qDebug()<< "- Preparando historico -" << " \n";
    if( sqlHistorico != "" ) {
        QList<QString> bindvalues;
        QList<registro_objetivo>* p_lista;
        registro_objetivo* p_registro;

        QString cadena,idObj = "";
        QString parametrosHistorico = "";
        // TODO verificicar si  n corresponde con el tamaño de salida
        for(int i=0; i<=n; i++){

            p_lista = &(salida[i]);
            int tamanoLista = p_lista->size();
            if( tamanoLista > 0 ) {
                // La denominacion "NPC" va al final de la lista de los clasificados
                if(i!=n) {
                    cadena = cache.get_objetivos()[i].get_valor("denominacion");
                    idObj = cache.get_objetivos()[i].get_valor("idtipoobjetivo") ;
                }
                else {
                    idObj = "";
                    cadena = "NPC";
                }
                 if(  idObj != "" )
                    parametrosHistorico.append(  idObj  );
            }
            for( int j = 0; j < tamanoLista ; j++ ){

                p_registro = &(p_lista->operator [](j));
                parametrosHistorico.append( "-" + p_registro->objetivo->atributos["idobjetivo"] );
                parametrosHistorico.append( "-" + p_registro->objetivo->atributos["idtrama"] );

                // Solo ejecuta si existe valor para la consulta
                if( sqlConsultaAct_2 != "" ) {

                    // Limpiar los valores a insertar
                    bindvalues.clear();

                    // Anadir elementos a la cadena
                    bindvalues.append(cadena);
                    cadena = p_registro->objetivo->atributos["idobjetivo"];
                    bindvalues.append(cadena);
                    pg.execute(sqlConsultaAct_2, bindvalues);      //Actualiza BD
                }
            }
            // verificar que fuera la ultima
            if( tamanoLista > 0 && i < n ) {
                parametrosHistorico.append(  ",");
            }
        }
        //parametrosHistorico.append(  "1");

        parametrosHistorico = "'{" +  parametrosHistorico  + "}'";
        // Ejecutar consulta de historico
        bindvalues.clear();
        sqlHistorico =  sqlHistorico.replace("?",  parametrosHistorico );
        sqlHistorico =  sqlHistorico.replace("params_time",  QString::number( tiempoActual ) );
        qDebug()<< "- Ejecutando historico: -"<< sqlHistorico  << " \n";
        bindvalues.append( "" );
        pg.execute( sqlHistorico , bindvalues);
    }


    generarJson(salida, n, filtroObjetivos);
    qDebug()<< "- -" << total << " objetivos fueron clasificados en total";
    qDebug()<<"- - Clasificacion realizada exitosamente.";
    total = 0;
    if(iteracion == INT_MAX) iteracion = 1;
    else iteracion++;

    return 0;
}

/**
 * Funcion para crear el json de salida a escribir en el sistema de archivos
 *
 * @brief SEObjetivos::generarJson
 * @param salida
 * @param size
 */
void SEObjetivos::generarJson(QList<registro_objetivo> salida[], int size, QString filtroObjetivos){

    registro_objetivo* actual;
    int n,m;

    int indicador = -1;
    int posicion = 0;
    while(posicion < size){
        if(salida[posicion].size() > 0) indicador = posicion;
        posicion ++;
    }

    m = salida[size].size();
    bool flag = (m > 0);

    QString cadena = QString("{ \"iteracion\" : \"%1\", ").arg(QString::number(iteracion)); //Codigo de Camilo
    cadena += "\"result\" : [ ";


/* #################################  TIPO DE OBJETIVO  ######################### */
    for(int i=0; i<size; i++){
        if(salida[i].size() > 0){

            cadena += "{ ";
            cadena += QString("\"audio\" : ") + "\"" + cache.get_atributo_objetivo(i, "audio") + "\", ";;

            cadena += "\"clasificados\" : [ ";
            n = salida[i].size();

            /* #######################  CLASIFICADOS  ################### */
            for(int j=0; j<n-1; j++){

                actual = &( (salida[i])[j] );

                cadena += "{ ";
                cadena += QString("\"codigo\" : ") + "\"" + actual->objetivo->atributos["codigo"] + "\", ";
                cadena += QString("\"idobjetivo\" : ") + "\"" + actual->objetivo->atributos["idobjetivo"] + "\", ";
                cadena += QString("\"idtrama\" : ") + "\"" + actual->objetivo->atributos["idtrama"] + "\", ";
                cadena += QString("\"semejanza\" : ") + "\"" + QString::number(actual->distancia) + "\" ";
                cadena += "}, ";
            }

            actual = &( (salida[i])[n-1] );
            cadena += "{ ";
            cadena += QString("\"codigo\" : ") + "\"" + actual->objetivo->atributos["codigo"] + "\", ";
            cadena += QString("\"idobjetivo\" : ") + "\"" + actual->objetivo->atributos["idobjetivo"] + "\", ";
            cadena += QString("\"idtrama\" : ") + "\"" + actual->objetivo->atributos["idtrama"] + "\", ";
            cadena += QString("\"semejanza\" : ") + "\"" + QString::number(actual->distancia) + "\" ";
            cadena += "} ";

            cadena += "], ";
            /* ################## FIN CLASIFICADOS ##################### */

            cadena += QString("\"denominacion\" : ") + "\"" + cache.get_atributo_objetivo(i, "denominacion") + "\", ";
            cadena += QString("\"imagen\" : ") + "\"" + cache.get_atributo_objetivo(i, "imagen") + "\", ";
            cadena += QString("\"mensaje\" : ") + "\"" + cache.get_atributo_objetivo(i, "mensaje") + "\" ";
            cadena += "}";

            if(flag || i != indicador)   cadena += ", ";

        }

    }
/* ##############################  FIN TIPO DE OBJETIVO  ########################### */




/* ##############################         NPC            ########################### */
    if (flag){
        cadena += "{ ";
        cadena += QString("\"audio\" : ") + "\"default_audio\", ";

        cadena += "\"clasificados\" : [ ";

        /* ######################  CLASIFICADOS NPC ################# */
        for(int j=0; j<m-1; j++){

            actual = &( (salida[size])[j] );

            cadena += "{ ";
            cadena += QString("\"codigo\" : ") + "\"" + actual->objetivo->atributos["codigo"] + "\", ";
            cadena += QString("\"idobjetivo\" : ") + "\"" + actual->objetivo->atributos["idobjetivo"] + "\", ";
            cadena += QString("\"idtrama\" : ") + "\"" + actual->objetivo->atributos["idtrama"] + "\", ";
            cadena += QString("\"semejanza\" : ") + "\"" + QString::number(actual->distancia) + "\" ";
            cadena += "}, ";
        }

        actual = &( (salida[size])[m-1] );
        cadena += "{ ";
        cadena += QString("\"codigo\" : ") + "\"" + actual->objetivo->atributos["codigo"] + "\", ";
        cadena += QString("\"idobjetivo\" : ") + "\"" + actual->objetivo->atributos["idobjetivo"] + "\", ";
        cadena += QString("\"idtrama\" : ") + "\"" + actual->objetivo->atributos["idtrama"] + "\", ";
        cadena += QString("\"semejanza\" : ") + "\"" + QString::number(actual->distancia) + "\" ";
        cadena += "} ";

        cadena += "], ";
        /* ################# FIN CLASIFICADOS NPC ################### */

        cadena += QString("\"denominacion\" : ") + "\"NPC\", ";
        cadena += QString("\"imagen\" : ") + "\"default_imagen\", ";
        cadena += QString("\"mensaje\" : ") + "\"Conocimiento insuficiente para clasificar estos objetivos\" ";
        cadena += "} ";
    }
/* ##############################      FIN DE NPC        ############################ */

    cadena += "] }";

    QString fileName = cache.get_ficherosalida() + "_" + filtroObjetivos;
    QFile jsonQFile( fileName );

    if (jsonQFile.open(QIODevice::ReadWrite|QIODevice::Truncate)) {
        QTextStream printer(&jsonQFile);
        printer.setCodec("UTF-8");
        printer << cadena;
        jsonQFile.close();
        QFile::setPermissions(fileName,(QFile::WriteOther|QFile::ReadOther)|
                                       (QFile::ReadGroup|QFile::WriteGroup)|
                                       (QFile::ReadUser|QFile::WriteUser));
    }
}

void SEObjetivos::clasificar_prueba(QList<DataObjective> &aviones, QList<registro_objetivo> salida[], bool flag){
    QString idobjetivo, idtrama, codigo;
    QString idtrama_old = "";
    int clasif_trama, no_clasif_trama;
    double distancia;
    QList<registro_objetivo> temporal;

    // Cantidad de resultados maximos a devolver
    int n_semejante = cache.get_semejantes();

    int n_objetivos = aviones.size();
    int n = polacas.size();  /*
                               Coincide con la cantidad de tipos de objetivos que se encuentran especificados en el fichero cache.
                               Es equivalente a declarar: n = cache.size_tipoobjetivo()
                             */

    DataObjective* actual;
    for(int i=0; i<n_objetivos; i++){              //Recorre uno a uno los aviones del espacio aereo
        actual = &(aviones[i]);

        idobjetivo = actual->atributos["idobjetivo"];
        idtrama = actual->atributos["idtrama"];
        codigo = actual->atributos["codigo"];
        if(idtrama_old != idtrama){
            if(!idtrama_old.isEmpty()){
                qDebug()<< "   >>> " << clasif_trama << " objetivos fueron clasificados en tipos definidos";
                qDebug()<< "   >>> " << no_clasif_trama << " objetivos no pudieron ser asociados a tipos definidos";
            }
            qDebug()<<">>>Clasificando objetivos en la trama: "<<idtrama;
            idtrama_old = idtrama;
            clasif_trama = 0;
            no_clasif_trama = 0;
        }

        // Evaluo al avion para cada uno de los tipos de objetivos
        for(int j=0; j<n; j++){

            // Esto es para no aplicarle la evalucion en polaca a un objetivo que su codigo no se encuentre entre los codigos del posible tipo de objetivo en que se intenta clasificar
            if(flag || cache.get_objetivos()[j].check.chequear(codigo.toInt())){

                distancia = p.comunEval(polacas.at(j), *actual, pg);

                // Solo valores de distancia entre [0,tolerancia)
                if(distancia>=0 && distancia<=cache.get_tolerancia()){
                    registro_objetivo clasificado;
                    clasificado.objetivo = actual;
                    clasificado.tipo = j;
                    clasificado.distancia = distancia;
                    temporal.append(clasificado);
                    // Optimizacion: Si la distancia es 0 verificar que no pase la cantidad de semejantes
                    if( distancia == 0 && n_semejante <= temporal.size() ) {
                        // Mandar a salir del ciclo
                        j = n;
                    }

                }
            }
        }


        // Si 'temporal' esta vacio fue que el avion no cumplio con la regla de ninguno de los tipos de objetivos especificados
        if(temporal.empty()){

            registro_objetivo no_clasificado;
            no_clasificado.objetivo = actual;
            no_clasificado.tipo = n;
            no_clasificado.distancia = 0;

            salida[n].append(no_clasificado);
            no_clasif_trama++;

//VERIFICACION
/*
            int longitud = salida[n].size();
            qDebug() << "NPC";
            for(int k=0; k<longitud; k++){
                qDebug() << salida[n].at(k).objetivo->atributos["idobjetivo"] + ", " + salida[n].at(k).objetivo->atributos["codigo"];
            }
*/
//############

        }
        else{

            // Obteniendo semejantes
            getSemejantes(temporal, MIN(temporal.size(), cache.get_semejantes()));
/*
            Despues del llamado a esta funcion 'temporal' sale solamente con el mas semejante, el de menor
            valor de distancia. La cantidad de semejantes a dejar se especifica en el fichero cache, que es 1.
            Si la cantidad de semejantes especificada fuera mayor que 1, y a su vez mayor que la cantidad de
            elementos que contiene 'temporal' entonces todos los elementos de 'temporal' se conservarian para
            el posterior analisis.
*/

            for(int k=0; k<temporal.size(); k++){
                salida[temporal[k].tipo].append(temporal[k]);

//VERIFICACION
/*
                int longitud = salida[temporal[k].tipo].size();
                int t = temporal[k].tipo;
                qDebug() << tipoobjetivo;
                for(int h=0; h<longitud; h++){
                    qDebug() << salida[t].at(h).objetivo->atributos["idobjetivo"] + ", " + salida[n].at(h).objetivo->atributos["codigo"];
                }
*/
//############

            }
            clasif_trama++;
            temporal.clear();

        }

    }

    // Resultado de la clasificacion de la ultima trama.
    qDebug()<< "   >>> " << clasif_trama << " objetivos fueron clasificados en tipos definidos";
    qDebug()<< "   >>> " << no_clasif_trama << " objetivos no pudieron ser asociados a tipos definidos";

}

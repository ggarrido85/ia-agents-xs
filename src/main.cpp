#include "seobjetivos.h"
//#include "stdio.h"
#include "unistd.h"
#define QVERSION "2.3.1 Agentes"
#define DEFAULT_CONFIG 0  // to etc/Agentes/Resources

/**
 * @brief main
 * @param argc
 * @param argv
 * @return
 */
int main(int argc, char *argv[]){

    qDebug() <<("** VERSION:")<<QVERSION<<"**\n";

    bool flag = false;
    // Decidir si se busca de etc o del mismo lugar que esta el binario
    bool flagEtc = DEFAULT_CONFIG;

    QString filtroObjetivos = "";

    // Buscar parametros de configuracion
    for( int j = 1; j < argc; j++ ){
        QString parametro = argv[j];
        QRegExp rx("(\\=)");
        QStringList query =parametro.split(rx);

        QString variable = query.at(0);
        QString value = query.at(1);
        qDebug() <<("-- Aplicando variable: ")<<variable<<"="<<value<<"**\n";
        if( variable == "etc" ) {
            qDebug() <<("-- Aplicando ETC: ");
            if(value == "1")
                flagEtc=true;
            else
                if(value == "0")
                    flagEtc=false;
        }
        else {
            if( variable  == "var"   ) {
                filtroObjetivos = value;
            }
        }
    }

    SEObjetivos se;

    while(!se.init(flagEtc)){
        qDebug()<<"- Esperando realizar otra pasada: durmiendo por 5 segundos...";
        sleep(5);
    }

    int sleepTime;
    while(true){
        se.pg.open();
        if(se.checkCacheStatus_ConstructPostfijas(filtroObjetivos))   se.clasificar(flag,filtroObjetivos);

        sleepTime = se.getSleepTime();

        qDebug()<<"- Durmiendo por "<<sleepTime<<" segundos ...\n";

        se.pg.close();
        sleep(sleepTime);

    }

    return 0;
}

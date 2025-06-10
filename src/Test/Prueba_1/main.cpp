#include <QDebug>
#include "seobjetivos.h"

/**                                         EXPLICACION DE LA PRUEBA
  El espaceo aereo se construye con 5 objetivos (DataObjective a1, a2, a3, a4, a5).
  El fichero cache_seobjetivos.xml contiene la especificaciones de los tipos de objetivos a clasificar.
  Cada tipo de objetivo tiene asociado una cadena polaca que lo describe y que se encuentra especificada
  en el fichero cache_seobjetivos.xml. La direccion donde se aloja este fichero xml es:
       /var/www/GIS/aplicativos/daafar/web/control_aereo/views/js/fleetControl/data/
  Se requiere tambien un fichero de configuracion con los parametros de conexion a la base de datos.
  Este fichero se llama connection_settings.xml y se encuentra alojado en el directorio:
       /etc/SEObjetivos/Resources/

  La logica de la prueba consiste en:
    - Se crea un objeto de tipo SEObjetivos nombrado "se"
    - El objeto "se" se inicia con la llamada al metodo "se.init()" que basicamente carga la configuracion del
      fichero connection_settings.xml.
    - Se llama al metodo "se.checkCacheStatus_ConstructPostfijas()" que crea o actualiza la cache, mediante la
      lectura del fichero cache_seobjetivos.xml.
    - Existe una estructrura creada en el programa llamada registro_objetivo. La misma encapsula los datos de
      salida asociados a un objetivo aereo.
    - El siguiente paso es llamar al metodo "se.clasificar_prueba(aviones, salida, false)". El mismo recibe de
      parametro de entrada el espacio aereo mediante un QList<DataObjective>. Recibe tambien un parametro de
      salida de tipo arreglo de QList<registro_objetivo> donde se encapsula la salida del metodo de clasificacion.
      El tercer parametro es un parametro de entrada booleano que indica si la clasificacion se realizara con
      previo chequeo de codigo o no. Valor "false" indica que se realiza con previo chequeo de codigo. El codigo
      es un atributo de los objetivos aereos que se tiene en consideracion para su proceso de clasificacion en
      tipos de objetivos.
    - Una vez ejecutado el metodo de clasificacion en tipos de objetivos, se llama al metodo "se.generarJson(salida,n)"
      que escribe en un fichero json los resultados obtenidos del metodo de clasificacion.

  Los resultados del proceso de clasificacion en tipo de objetivos se muestran en un fichero clasificacion.json que
  se genera dentro del directorio:
      /var/www/GIS/aplicativos/daafar/web/control_aereo/views/js/fleetControl/data/

  Para ejecutar el archivo main.cpp de la prueba se debe adicionar el parametro CONFIG+=config_test a los parametros del
  comando qmake que se especifican en la seccion de Projects del Qt. Para ejecutar el main.cpp de la aplicacion principal
  este parametro se debe cambiar a CONFIG+=config_principal y borrar el fichero main.o del directorio donde se almacenan
  los archivos que se generan en el proceso de compilacion. Esto ultimo se precisa para que el proyecto se vuelva a compilar
  y se genere un nuevo ejecutable, ahora con el main.cpp de la aplicacion principal.

*/

int main(){    

    DataObjective a1, a2, a3, a4, a5;

    a1.atributos.insert("idobjetivo", "00"); a1.atributos.insert("idtrama", "1"); a1.atributos.insert("codigo", "0701");
    a1.atributos.insert("latitud", "20.50"); a1.atributos.insert("longitud", "-80.50");
    a1.atributos.insert("altura", "25000"); a1.atributos.insert("velocidad", "600");

    a2.atributos.insert("idobjetivo", "01"); a2.atributos.insert("idtrama", "1"); a2.atributos.insert("codigo", "0702");
    a2.atributos.insert("latitud", "25.50"); a2.atributos.insert("longitud", "-82.50");
    a2.atributos.insert("altura", "15000"); a2.atributos.insert("velocidad", "800");

    a3.atributos.insert("idobjetivo", "02"); a3.atributos.insert("idtrama", "1"); a3.atributos.insert("codigo", "1001");
    a3.atributos.insert("latitud", "24.20"); a3.atributos.insert("longitud", "-78.50");
    a3.atributos.insert("altura", "40000"); a3.atributos.insert("velocidad", "850");

    a4.atributos.insert("idobjetivo", "03"); a4.atributos.insert("idtrama", "2"); a4.atributos.insert("codigo", "1002");
    a4.atributos.insert("latitud", "27.50"); a4.atributos.insert("longitud", "-76.50");
    a4.atributos.insert("altura", "55000"); a4.atributos.insert("velocidad", "900");

    a5.atributos.insert("idobjetivo", "04"); a5.atributos.insert("idtrama", "2"); a5.atributos.insert("codigo", "2500");
    a5.atributos.insert("latitud", "20.50"); a5.atributos.insert("longitud", "-76.50");
    a5.atributos.insert("altura", "30000"); a5.atributos.insert("velocidad", "300");

    QList<DataObjective> aviones;
    aviones.append(a1); aviones.append(a2); aviones.append(a3); aviones.append(a4); aviones.append(a5);

    SEObjetivos se;
    if(!se.init())  qDebug()<<"Error de conexion a la base de datos";

    se.checkCacheStatus_ConstructPostfijas();  /* Actualizando la cache */

    int n = se.size_cache();
    QList<registro_objetivo> salida[n+1];

    se.clasificar_prueba(aviones, salida, false);
    se.generarJson(salida,n);    

    return 0;
}

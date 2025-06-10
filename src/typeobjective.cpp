#include "typeobjective.h"

TypeObjective::TypeObjective(){

}

QString TypeObjective::get_valor(QString key){
    return atributos.value(key);
}

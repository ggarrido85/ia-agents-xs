#include "chequeocodigo.h"

ChequeoCodigo::ChequeoCodigo(){
}

ChequeoCodigo::ChequeoCodigo(QStringList expresion){
    int n = expresion.size();
    int i = 0;
    while(i<n){
        if(expresion.at(i) == "codigo"){
            registro_codigo r;
            r.menor = expresion.at(i+1).toInt();
            r.mayor = expresion.at(i+4).toInt();
            codigos.append(r);
            i+=6;
        }
        else{
            i++;
        }
    }
}

bool ChequeoCodigo ::chequear(int codigo){
    for(int i=0; i<codigos.size(); i++){
        if( codigo >= codigos.at(i).menor  &&  codigo <= codigos.at(i).mayor) return true;
    }
    return false;
}

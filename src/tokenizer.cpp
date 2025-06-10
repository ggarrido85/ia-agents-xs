#include "tokenizer.h"
#include <QDebug>

Tokenizer::Tokenizer(){
}

void Tokenizer::parsePostfijaString(const QString &st, const QList<operador> &ops, QList<elemento_polaca> &result){
    int ini = 0;
    QString cadena;
    int numero;

    for(int i=0; i<st.length(); i++){
        if(st.at(i) == ','){
            elemento_polaca ep;
            cadena = st.mid(ini, i-ini);
            if(cadena.length() == 1 && (numero=findOpInVector(cadena.at(0), ops)) >= 0){
                ep.variant.setValue(numero);
                ep.usertype = OPERADOR;
            }
            else if(Utils::esNumero(cadena)){
                ep.variant.setValue(cadena.toDouble());
                ep.usertype = NUMERICO;
            }
            else{
                ep.variant.setValue(cadena);
                if(cadena.startsWith("f_")){
                    ep.usertype = FUNCION;
                }
                else{
                    ep.usertype = VARIABLE;
                }
            }
            ini = i+1;
            result.push_back(ep);

        }
        else if(i == st.length()-1){
            elemento_polaca ep;
            cadena = st.mid(ini, st.length()-ini);
            if(cadena.length() == 1 && (numero=findOpInVector(cadena.at(0), ops)) >= 0){
                ep.variant.setValue(numero);
                ep.usertype = OPERADOR;
            }
            else if(Utils::esNumero(cadena)){
                ep.variant.setValue(cadena.toDouble());
                ep.usertype = NUMERICO;
            }
            else{
                ep.variant.setValue(cadena);
                if(cadena.startsWith("f_")){
                    ep.usertype = FUNCION;
                }
                else{
                    ep.usertype = VARIABLE;
                }
            }
            result.push_back(ep);
        }
    }
}

int Tokenizer::findOpInVector(QChar op, const QList<operador> &ops){
    if(op == '(')         return -2;
    else if(op == ')')    return -3;

    int i = 0;
    int n = ops.size();
    bool searching = true;

    while (i<n && searching){
        if(op == ops.at(i).op)   searching = false;
        else  i++;
    }

    if(searching)       return -1;
    return i;
}

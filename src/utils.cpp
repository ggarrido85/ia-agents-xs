#include "utils.h"

Utils::Utils(){
}

bool Utils::esNumero(const QString &st){
    int i = 1;
    bool esNumero = true;
    int length = st.length();

    if(!st.at(0).isDigit()) return false;

    while(i<length && esNumero){
        if(!st.at(i).isDigit() && st.at(i) != '.'){
            esNumero = false;
        }
        else {
            i++;
        }
    }
    return esNumero;
}

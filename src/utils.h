#ifndef UTILS_H
#define UTILS_H

#include <QVariant>

enum Type {NUMERICO,VARIABLE,OPERADOR,PARDER,PARIZQ,FUNCION};

/* Estructura que define un operador */
typedef struct{
    char op;
    int pa;
    int afecta;
}operador;

/* Estructura que define un elemento de la cadena polaca */
typedef struct{
    QVariant variant;
    Type usertype;
}elemento_polaca;

class Utils{

public:
    Utils();
    static bool esNumero(const QString &st);
};

#endif // UTILS_H

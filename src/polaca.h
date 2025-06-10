#ifndef POLACA_H
#define POLACA_H

#include <QList>
#include "dataobjective.h"
#include "tokenizer.h"
#include "postgreconexion.h"

class Polaca{

private:

    QList<operador> _operadores;          // Vector con los operadores

public:
    Polaca();

    QList<operador> getOperadores();

    /**
       Metodo de evaluacion de la polaca en un objetivo aereo
    */
    double comunEval(const QList<elemento_polaca> &postfija, const DataObjective &avion, PostgreConexion &postgreConx);
};

#endif // POLACA_H

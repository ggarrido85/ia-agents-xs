#ifndef TYPEOBJECTIVE_H
#define TYPEOBJECTIVE_H

#include <QHash>
#include "chequeocodigo.h"

class TypeObjective{

public:
    QHash<QString, QString> atributos;
    ChequeoCodigo check;

    TypeObjective();
    QString get_valor(QString);
};

#endif // TYPEOBJECTIVE_H

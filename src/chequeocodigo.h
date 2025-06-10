#ifndef CHEQUEOCODIGO_H
#define CHEQUEOCODIGO_H

#include <QStringList>

typedef struct{
    int menor;
    int mayor;
}registro_codigo;


class ChequeoCodigo{

public:
    ChequeoCodigo();
    ChequeoCodigo(QStringList expresion);
    bool chequear(int codigo);

private:
   QList<registro_codigo> codigos;
};

#endif // CHEQUEOCODIGO_H

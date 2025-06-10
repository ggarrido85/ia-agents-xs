#ifndef TOKENIZER_H
#define TOKENIZER_H

#include "utils.h"
#include <QList>

class Tokenizer{

private:
    int findOpInVector(QChar op, const QList<operador> &ops);

public:
    Tokenizer();
    void parsePostfijaString(const QString &st, const QList<operador> &ops, QList<elemento_polaca> &result);
};

#endif // TOKENIZER_H

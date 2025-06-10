#include <QDebug>
#include "polaca.h"

Polaca::Polaca(){
    operador op1;   op1.op = '+';     op1.pa = 1;      op1.afecta = 2;  _operadores.push_back(op1);
    operador op2;   op2.op = '-';     op2.pa = 1;      op2.afecta = 2;  _operadores.push_back(op2);
    operador op3;   op3.op = '*';     op3.pa = 2;      op3.afecta = 2;  _operadores.push_back(op3);
    operador op4;   op4.op = '/';     op4.pa = 2;      op4.afecta = 2;  _operadores.push_back(op4);
    operador op5;   op5.op = '^';     op5.pa = 3;      op5.afecta = 2;  _operadores.push_back(op5);
    operador op6;   op6.op = '?';     op6.pa = 6;      op6.afecta = 3;  _operadores.push_back(op6);
    operador op7;   op7.op = ':';     op7.pa = 7;      op7.afecta = 1;  _operadores.push_back(op7);
    operador op8;   op8.op = '|';     op8.pa = 4;      op8.afecta = 2;  _operadores.push_back(op8);
    operador op9;   op9.op = '&';     op9.pa = 4;      op9.afecta = 2;  _operadores.push_back(op9);
    operador op10;  op10.op = '=';    op10.pa = 5;     op10.afecta = 2; _operadores.push_back(op10);
    operador op11;  op11.op = '>';    op11.pa = 5;     op11.afecta = 2; _operadores.push_back(op11);
    operador op12;  op12.op = '<';    op12.pa = 5;     op12.afecta = 2; _operadores.push_back(op12);
    operador op13;  op13.op = '{';    op13.pa = 5;     op13.afecta = 2; _operadores.push_back(op13);
    operador op14;  op14.op = '}';    op14.pa = 5;     op14.afecta = 2; _operadores.push_back(op14);
    operador op15;  op15.op = '~';    op15.pa = 7;     op15.afecta = 1; _operadores.push_back(op15);
    operador op16;  op16.op = ';';    op16.pa = 6;     op16.afecta = 2; _operadores.push_back(op16);
}

double Polaca::comunEval(const QList<elemento_polaca> &postfija, const DataObjective &avion, PostgreConexion &postgreConx){
    QList<double> pilaEvaluacion;
    elemento_polaca ep;
    double term1, term2, tmp, toConvert;
    int n = postfija.size(),toConvertNoDecimal;
    bool eval = false;

    QString _argv, numstr, var;
    QSqlQuery query;

    for(int i=0; i<n; i++){
        ep = postfija.at(i);
        switch(ep.usertype){
        case NUMERICO:
            pilaEvaluacion.push_back(ep.variant.toDouble());
            break;
        case VARIABLE:
            /* busco el valor de la variable en el objeto DataObjective y lo meto en la pila de evaluacion */
            var = ep.variant.toString();
            if(!avion.atributos.contains(var)){
                qCritical()<<"Atributo"<<var<<"no existe en el avion.";
                return -1;
            }
            else{
                tmp = avion.atributos[var].toDouble();
                pilaEvaluacion.push_back(tmp);
            }
            break;
        case FUNCION:
            /* cuando se encuentra una funcion debe haber al menos un valor que seria el numero de args de la funcion*/
            if(pilaEvaluacion.size() == 0){
                qCritical()<<"ERROR: Insufficient operands on the stack of evaluation. Rule poorly formed.";
                return -1;
            }
            /* tmp recibe la cantidad de argumentos de la funcion */
            tmp = pilaEvaluacion.back();
            pilaEvaluacion.pop_back();
            if(tmp>0){
                if(pilaEvaluacion.size()<tmp){
                    qCritical()<<"ERROR: Insufficient operands on the stack of evaluation. Rule poorly formed.";
                    return -1;
                }
                _argv = "";
                numstr = "";
                //numstr.setNum((long)pilaEvaluacion.back(),(int)12);
                //numstr = QString::number(pilaEvaluacion.back());//setNum((long)pilaEvaluacion.back(),(int)12);
                // Llevar a entero o decimal segun convenga convertir el string
                toConvert = pilaEvaluacion.back();
                toConvertNoDecimal = toConvert;
                // Si son diferentes quedarme con el decimal
                if(toConvert != toConvertNoDecimal)
                    numstr.setNum(toConvert,'f');
                else
                    numstr.setNum(toConvertNoDecimal);


                pilaEvaluacion.pop_back();
                _argv.append(numstr);
                for( ; tmp>1; tmp--){
                    _argv.insert(0, ',');
                    // Llevar a entero o decimal segun convenga convertir el string
                    toConvert = pilaEvaluacion.back();
                    toConvertNoDecimal = toConvert;
                    // Si son diferentes quedarme con el decimal
                    if(toConvert != toConvertNoDecimal)
                        numstr.setNum(toConvert,'f');
                    else
                        numstr.setNum(toConvertNoDecimal);

                    pilaEvaluacion.pop_back();
                    _argv.insert(0, numstr);
                }
            }
            _argv.append(')');
            _argv.insert(0, '(');
            _argv.insert(0, ep.variant.toString());
            numstr = "SELECT ";
            numstr.append(postgreConx.getEsquema("ia"));
            numstr.append('.');
            numstr.append(_argv);
            //qDebug()<<"- Ejecutando funcion de BD "<< numstr<<"...\n";
            query = postgreConx.execute(numstr);
            if(!query.isActive()){
                qCritical()<<"Error al ejecutar la funcion en BD." << query.lastError().text();
                return -1;
            }
            query.next();
            pilaEvaluacion.push_back(query.value(0).toDouble());
            query.clear();
            break;
        case OPERADOR:
            if(pilaEvaluacion.size()<_operadores.at(ep.variant.toInt()).afecta){
                qCritical()<<"ERROR: Insufficient operands on the stack of evaluation. Rule poorly formed.";
                return -1;
            }
            switch(_operadores.at(ep.variant.toInt()).op){
            case '+':
                term2 = pilaEvaluacion.back();
                pilaEvaluacion.pop_back();
                term1 = pilaEvaluacion.back();
                pilaEvaluacion.pop_back();
                pilaEvaluacion.push_back(term1 + term2);
                break;
            case '-':
                term2 = pilaEvaluacion.back();
                pilaEvaluacion.pop_back();
                term1 = pilaEvaluacion.back();
                pilaEvaluacion.pop_back();
                pilaEvaluacion.push_back(term1 - term2);
                break;
            case '*':
                term2 = pilaEvaluacion.back();
                pilaEvaluacion.pop_back();
                term1 = pilaEvaluacion.back();
                pilaEvaluacion.pop_back();
                pilaEvaluacion.push_back(term1 * term2);
                break;
            case '/':
                term2 = pilaEvaluacion.back();
                pilaEvaluacion.pop_back();
                term1 = pilaEvaluacion.back();
                pilaEvaluacion.pop_back();
                pilaEvaluacion.push_back(term1 / term2);
                break;
            case '^':
                term2 = pilaEvaluacion.back();
                pilaEvaluacion.pop_back();
                term1 = pilaEvaluacion.back();
                pilaEvaluacion.pop_back();
                tmp = term1;
                for(int i=1; i<term2; i++)
                    tmp *= term1;
                pilaEvaluacion.push_back(tmp);
                break;
            case '?':
                term2 = pilaEvaluacion.back();
                pilaEvaluacion.pop_back();
                term1 = pilaEvaluacion.back();
                pilaEvaluacion.pop_back();
                tmp = pilaEvaluacion.back();
                pilaEvaluacion.pop_back();
                if(tmp == 1)
                    pilaEvaluacion.push_back(term1);
                else
                    pilaEvaluacion.push_back(term2);
                break;
            case '|':
                term2 = pilaEvaluacion.back();
                pilaEvaluacion.pop_back();
                term1 = pilaEvaluacion.back();
                pilaEvaluacion.pop_back();
                pilaEvaluacion.push_back( ((int)term1) | ((int)term2) );
                break;
            case '&':
                term2 = pilaEvaluacion.back();
                pilaEvaluacion.pop_back();
                term1 = pilaEvaluacion.back();
                pilaEvaluacion.pop_back();
                pilaEvaluacion.push_back( ((int)term1) & ((int)term2) );
                break;
            case '=':
                term2 = pilaEvaluacion.back();
                pilaEvaluacion.pop_back();
                term1 = pilaEvaluacion.back();
                pilaEvaluacion.pop_back();
                eval = (term1 == term2);
                if(!eval)
                    return -1;
                pilaEvaluacion.push_back(eval);
                break;
            case '>':
                term2 = pilaEvaluacion.back();
                pilaEvaluacion.pop_back();
                term1 = pilaEvaluacion.back();
                pilaEvaluacion.pop_back();
                eval = (term1 > term2);
                if(!eval)
                    return -1;
                pilaEvaluacion.push_back(eval);
                break;
            case '<':
                term2 = pilaEvaluacion.back();
                pilaEvaluacion.pop_back();
                term1 = pilaEvaluacion.back();
                pilaEvaluacion.pop_back();
                eval = (term1 < term2);
                if(!eval)
                    return -1;
                pilaEvaluacion.push_back(eval);
                break;
            case '{':
                term2 = pilaEvaluacion.back();
                pilaEvaluacion.pop_back();
                term1 = pilaEvaluacion.back();
                pilaEvaluacion.pop_back();
                eval = (term1 <= term2);
                if(!eval)
                    return -1;
                pilaEvaluacion.push_back(eval);
                break;
            case '}':
                term2 = pilaEvaluacion.back();
                pilaEvaluacion.pop_back();
                term1 = pilaEvaluacion.back();
                pilaEvaluacion.pop_back();
                eval = (term1 >= term2);
                if(!eval)
                    return -1;
                pilaEvaluacion.push_back(eval);
                break;
            case '~':
                term1 = pilaEvaluacion.back();
                pilaEvaluacion.pop_back();
                pilaEvaluacion.push_back((-1)*term1);
                break;
            }
            break;
        }
    }
    if(pilaEvaluacion.size() != 1){
        qCritical()<<"ERROR: Too many items on the stack at the end of the evaluation. Rule poorly formed.";
        return -1;
    }
    return pilaEvaluacion.back();
}

QList<operador> Polaca::getOperadores(){
    return _operadores;
}


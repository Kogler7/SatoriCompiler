/**
 * @file gram/opg.h
 * @author Zhenjie Wei (2024108@bjtu.edu.cn)
 * @brief Operator Precedence Grammar
 * @date 2023-05-12
 *
 * @copyright Copyright (c) 2023
 *
 */

#include "basic.h"

class OperatorPrecedenceGrammar : public Grammar
{
    symset calcFirstVTOf(symbol t);
    symset calcLastVTOf(symbol t);
    void calcFirstVT();
    void calcLastVT();
    void calcOPT();

public:
    map<symbol, symset> firstVT;
    map<symbol, symset> lastVT;
    map<symbol, map<symbol, int>> opt; // operator priority table
    OperatorPrecedenceGrammar() : Grammar(){};
    OperatorPrecedenceGrammar(const Grammar &g) : Grammar(g)
    {
        calcFirstVT();
        calcLastVT();
        calcOPT();
    }
    OperatorPrecedenceGrammar(const OperatorPrecedenceGrammar &g) : Grammar(g)
    {
        firstVT = g.firstVT;
        lastVT = g.lastVT;
    }
    void printFirstVT();
    void printLastVT();
    void printOPT();
};
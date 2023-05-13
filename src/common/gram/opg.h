/**
 * @file gram/opg.h
 * @author Zhenjie Wei (2024108@bjtu.edu.cn)
 * @brief Operator Precedence Grammar
 * @date 2023-05-12
 *
 * @copyright Copyright (c) 2023
 *
 */

#pragma once

#include "basic.h"

enum OP
{
    NL = -2,
    LT = -1,
    EQ = 0,
    GT = 1
};

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
        // 添加产生式S->#S#
        product p(symStart, {SYM_END, symStart, SYM_END});
        products.push_back(p);
        rules[symStart].insert(p.second);
        calcOPT();
    }
    OperatorPrecedenceGrammar(const OperatorPrecedenceGrammar &g) : Grammar(g)
    {
        firstVT = g.firstVT;
        lastVT = g.lastVT;
        opt = g.opt;
    }
    void printFirstVT();
    void printLastVT();
    void printOPT();
};
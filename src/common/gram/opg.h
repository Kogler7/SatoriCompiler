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
    symset_t calcFirstVTOf(symbol_t t);
    symset_t calcLastVTOf(symbol_t t);
    void calcFirstVT();
    void calcLastVT();
    void calcOPT();

public:
    map<symbol_t, symset_t> firstVT;
    map<symbol_t, symset_t> lastVT;
    table_t<symbol_t, symbol_t, int> opt; // operator precedence table
    OperatorPrecedenceGrammar() : Grammar(){};
    OperatorPrecedenceGrammar(const Grammar &g) : Grammar(g)
    {
        calcFirstVT();
        calcLastVT();
        // 添加产生式S->#S#
        product_t p(symStart, {SYM_END, symStart, SYM_END});
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
    void printFirstVT() const;
    void printLastVT() const;
    void printOPT() const;
};
/**
 * @file gram/basic.h
 * @author Zhenjie Wei (2024108@bjtu.edu.cn)
 * @brief Predictive Grammar
 * @date 2023-05-12
 *
 * @copyright Copyright (c) 2023
 *
 */

#pragma once

#include "basic.h"

class PredictiveGrammar : public Grammar
{
    symset calcFirstOf(symbol t);
    symset calcFirstOf(symstr s);
    symset calcFollowOf(symbol t);
    symset calcSelectOf(product p);
    void calcFirst();
    void calcFollow();
    void calcSelect();

public:
    PredictiveGrammar() : Grammar(){};
    PredictiveGrammar(const Grammar &g) : Grammar(g)
    {
        calcFirst();
        calcFollow();
        calcSelect();
    }
    PredictiveGrammar(const PredictiveGrammar &g) : Grammar(g)
    {
        first = g.first;
        firstS = g.firstS;
        follow = g.follow;
        select = g.select;
    }
    map<symbol, symset> first;
    map<symstr, symset> firstS;
    map<symbol, symset> follow;
    map<product, symset> select;
    void printFirst();
    void printFollow();
    void printFirstS();
    void printSelect();
    bool isLL1Grammar();
};
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
protected:
    bool hasLeftRecursion = false;
    symset_t calcFirstOf(symbol_t t);
    symset_t calcFirstOf(symstr_t s);
    symset_t calcFollowOf(symbol_t t);
    symset_t calcSelectOf(product_t p);
    void calcFirst();
    void calcFollow();
    void calcSelect();
    void calcFirstWithLeftRecursion();

public:
    map<symbol_t, symset_t> first;
    map<symstr_t, symset_t> firstS;
    map<symbol_t, symset_t> follow;
    map<product_t, symset_t> select;
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
    void printFirst() const;
    void printFollow() const;
    void printFirstS() const;
    void printSelect() const;
    bool isLL1Grammar() const;
};
/**
 * @file gram/slr1.h
 * @author Zhenjie Wei (2024108@bjtu.edu.cn)
 * @brief SLR(1) Grammar
 * @date 2023-05-16
 *
 * @copyright Copyright (c) 2023
 *
 */

#pragma once

#include <variant>
#include "lrg.h"
#include "utils/log.h"

class SLR1Grammar : public LRGrammar
{
    bool checkSLR1();
    void calcSLR1Table();

public:
    SLR1Grammar() : LRGrammar(){};
    SLR1Grammar(const Grammar &g) : LRGrammar(g)
    {
        assert(checkSLR1(), "Not SLR(1) grammar!");
        calcSLR1Table();
    }
    SLR1Grammar(const SLR1Grammar &g) : LRGrammar(g)
    {
        slr1Table = g.slr1Table;
    }
    map<coord_t, action_t> slr1Table;
    void printSLR1Table();
};
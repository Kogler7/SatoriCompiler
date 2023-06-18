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

#include "lrg.h"
#include "utils/log.h"

#include <variant>

class SLR1Grammar : public LRGrammar
{
    void calcSLR1Table();

public:
    bool checkSLR1();
    SLR1Grammar() : LRGrammar(){};
    SLR1Grammar(const Grammar &g) : LRGrammar(g)
    {
        calcSLR1Table();
    }
    SLR1Grammar(const SLR1Grammar &g) : LRGrammar(g)
    {
        slr1Table = g.slr1Table;
    }
    table_t<state_id_t, symbol_t, action_t> slr1Table;
    void printSLR1Table() const;
    void printSLR1TableField(coord_t<state_id_t, symbol_t> c) const;
    void printSLR1TableOfState(state_id_t s) const;
    void printSLR1Table(coord_t<state_id_t, symbol_t> c) const;
    void printLargeSLR1Table() const;
};
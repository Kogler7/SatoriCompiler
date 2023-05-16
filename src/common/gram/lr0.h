/**
 * @file gram/lr0.h
 * @author Zhenjie Wei (2024108@bjtu.edu.cn)
 * @brief LR(0) Grammar
 * @date 2023-05-16
 *
 * @copyright Copyright (c) 2023
 *
 */

#pragma once

#include <variant>
#include "common/fa.h"
#include "predict.h"

using namespace std;

typedef reference_wrapper<product> product_ref;
typedef variant<bool, product_ref, state_id> action;
typedef pair<product_ref, size_t> lr0_item;

class LR0Grammar : public PredictiveGrammar
{
    void calcLR0Closure();
    void calcLR0Goto();
    void calcLR0Items();

public:
    LR0Grammar() : PredictiveGrammar(){};
    LR0Grammar(const Grammar &g) : PredictiveGrammar(g)
    {
        calcLR0Closure();
        calcLR0Goto();
        calcLR0Items();
    }
    LR0Grammar(const LR0Grammar &g) : PredictiveGrammar(g)
    {
        items = g.items;
        gotoTable = g.gotoTable;
        actTable = g.actTable;
    }
    vector<lr0_item> items;
    map<state_id, map<symbol, action>> gotoTable;
    map<state_id, map<symbol, action>> actTable;
    void printItems();
    void printGotoTable();
    void printActTable();
};
/**
 * @file spt/parser.h
 * @author Zhenjie Wei (2024108@bjtu.edu.cn)
 * @brief Stack-based Parsing with Predictive Table
 * @date 2023-04-27
 *
 * @copyright Copyright (c) 2023
 *
 */

#include "common/tree/cst.h"
#include "common/gram/predict.h"

class StackPredictiveTableParser
{
    PredictiveGrammar grammar;
    cst_tree_ptr_t tree;
    map<symbol_t, map<symbol_t, symstr_t>> predict;
    void calcPredictTable();

public:
    StackPredictiveTableParser(PredictiveGrammar g) : grammar(g)
    {
        tree = cst_tree_t::createNode(TERMINAL, SYM_END, 0, 0);
        calcPredictTable();
    }
    void printPredictTable();
    bool parse(vector<token> input);
    cst_tree_ptr_t getTree() { return tree; }
};

typedef StackPredictiveTableParser SPTParser;
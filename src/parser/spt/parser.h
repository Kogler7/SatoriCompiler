/**
 * @file spt/parser.h
 * @author Zhenjie Wei (2024108@bjtu.edu.cn)
 * @brief Stack-based Parsing with Predictive Table
 * @date 2023-04-27
 *
 * @copyright Copyright (c) 2023
 *
 */

#include "common/cst.h"
#include "common/gram.h"

class StackPredictiveTableParser
{
public:
    Grammar grammar;
    cst_tree_ptr tree;
    map<term, map<term, vector<term>>> predict;

    StackPredictiveTableParser(Grammar g) : grammar(g)
    {
        tree = cst_tree::createNode(TERMINAL, SYM_END, 0, 0);
    }
    void calcPredictTable();
    void printPredictTable();
    bool parse(vector<token> input);
};

typedef StackPredictiveTableParser SPTParser;
/**
 * @file spt/parser.h
 * @author Zhenjie Wei (2024108@bjtu.edu.cn)
 * @brief Stack-based Parsing with Predictive Table
 * @date 2023-04-27
 *
 * @copyright Copyright (c) 2023
 *
 */

#include "common/tree/pst.h"
#include "common/gram/predict.h"

class StackPredictiveTableParser
{
    PredictiveGrammar grammar;
    pst_tree_ptr_t cst;
    table_t<symbol_t, symbol_t, symstr_t> predict;
    void calcPredictTable();

public:
    StackPredictiveTableParser(PredictiveGrammar g) : grammar(g)
    {
        cst = pst_tree_t::createNode(TERMINAL, SYM_END, 0, 0);
        calcPredictTable();
    }
    void printPredictTable() const;
    bool parse(std::vector<token> input);
    pst_tree_ptr_t getCST() const { return cst; }
};

using SPTParser = StackPredictiveTableParser;
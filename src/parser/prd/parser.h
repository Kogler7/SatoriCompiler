/**
 * @file prd/parser.h
 * @author Zhenjie Wei (2024108@bjtu.edu.cn)
 * @brief Predictive Recursive Descent Parsing
 * @date 2023-04-30
 *
 * @copyright Copyright (c) 2023
 *
 */

#include "common/tree/cst.h"
#include "common/gram/predict.h"
#include "common/token.h"
#include "utils/view/tok_view.h"

class PredictiveRecursiveDescentParser
{
    PredictiveGrammar grammar;
    cst_tree_ptr_t tree;
    bool parseNonTerm(TokenViewer &viewer, symbol_t sym, cst_node_ptr_t node);

public:
    PredictiveRecursiveDescentParser(PredictiveGrammar &grammar) : grammar(grammar)
    {
        tree = cst_tree_t::createNode(TERMINAL, SYM_END, 0, 0);
    }
    bool parse(vector<token> &input);
    cst_tree_ptr_t getTree() { return tree; }
};

typedef PredictiveRecursiveDescentParser PRDParser;
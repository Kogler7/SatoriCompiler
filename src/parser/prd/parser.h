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
#include "utils/tok_view.h"

class PredictiveRecursiveDescentParser
{
    PredictiveGrammar grammar;
    cst_tree_ptr tree;
    bool parseNonTerm(TokenViewer &viewer, symbol sym, cst_node_ptr node);

public:
    PredictiveRecursiveDescentParser(PredictiveGrammar &grammar) : grammar(grammar)
    {
        tree = cst_tree::createNode(TERMINAL, SYM_END, 0, 0);
    }
    bool parse(vector<token> &input);
    cst_tree_ptr getTree() { return tree; }
};

typedef PredictiveRecursiveDescentParser PRDParser;
/**
 * @file prd/parser.h
 * @author Zhenjie Wei (2024108@bjtu.edu.cn)
 * @brief Predictive Recursive Descent Parsing
 * @date 2023-04-30
 *
 * @copyright Copyright (c) 2023
 *
 */

#include "common/cst.h"
#include "common/gram.h"
#include "common/token.h"
#include "utils/tok_view.h"

class PredictiveRecursiveDescentParser
{
    Grammar grammar;
    cst_tree_ptr tree;
    bool parseNonTerm(TokenViewer &viewer, symbol t);

public:
    PredictiveRecursiveDescentParser(Grammar &grammar) : grammar(grammar)
    {
        tree = cst_tree::createNode(TERMINAL, SYM_END, 0, 0);
    }
    bool parse(vector<token> &input);
    cst_tree_ptr getTree() { return tree; }
};

typedef PredictiveRecursiveDescentParser PRDParser;
/**
 * @file prd/parser.h
 * @author Zhenjie Wei (2024108@bjtu.edu.cn)
 * @brief Predictive Recursive Descent Parsing
 * @date 2023-04-30
 *
 * @copyright Copyright (c) 2023
 *
 */

#include "common/tree/pst.h"
#include "common/gram/predict.h"
#include "common/token.h"
#include "utils/view/tok_view.h"

class PredictiveRecursiveDescentParser
{
    PredictiveGrammar grammar;
    pst_tree_ptr_t cst;
    bool parseNonTerm(TokenViewer &viewer, symbol_t sym, pst_node_ptr_t node);

public:
    PredictiveRecursiveDescentParser(PredictiveGrammar &grammar) : grammar(grammar)
    {
        cst = pst_tree_t::createNode(TERMINAL, SYM_END, 0, 0);
    }
    bool parse(std::vector<token> &input);
    pst_tree_ptr_t getCST() { return cst; }
};

using PRDParser = PredictiveRecursiveDescentParser;
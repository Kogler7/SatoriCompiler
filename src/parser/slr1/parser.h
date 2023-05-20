/**
 * @file slr1/parser.h
 * @author Zhenjie Wei (2024108@bjtu.edu.cn)
 * @brief Simple LR(1) Parser
 * @date 2023-05-16
 *
 * @copyright Copyright (c) 2023
 *
 */

#include "common/tree/cst.h"
#include "common/gram/slr1.h"

class SimpleLR1Parser
{
    SLR1Grammar grammar;
    cst_tree_ptr_t tree;
    pair<string, string> descAction(const action_t &act);
public:
    SimpleLR1Parser(SLR1Grammar &grammar) : grammar(grammar)
    {
        tree = cst_tree_t::createNode(TERMINAL, SYM_END, 0, 0);
    }
    bool parse(vector<token> &input);
    cst_tree_ptr_t getTree() { return tree; }
};

typedef SimpleLR1Parser SLR1Parser;
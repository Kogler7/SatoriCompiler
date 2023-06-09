/**
 * @file elr/parser.h
 * @author Zhenjie Wei (2024108@bjtu.edu.cn)
 * @brief Extended SLR(1) Parser
 * @date 2023-06-05
 *
 * @copyright Copyright (c) 2023
 *
 */

#include "common/tree/pst.h"
#include "common/gram/slr1.h"
#include "utils/view/ctx_view.h"

class ExtendedSimpleLR1Parser
{
    SLR1Grammar grammar;
    pst_tree_ptr_t tree;
    pair<string, string> descAction(const action_t &act) const;
public:
    ExtendedSimpleLR1Parser(SLR1Grammar &grammar) : grammar(grammar)
    {
        tree = pst_tree_t::createNode(TERMINAL, SYM_END, 0, 0);
    }
    bool parse(vector<token> &input, const ContextViewer &code);
    pst_tree_ptr_t getTree() { return tree; }
};

typedef ExtendedSimpleLR1Parser ESLR1Parser;
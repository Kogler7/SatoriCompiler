/**
 * @file slr1/parser.h
 * @author Zhenjie Wei (2024108@bjtu.edu.cn)
 * @brief Simple LR(1) Parser
 * @date 2023-05-16
 *
 * @copyright Copyright (c) 2023
 *
 */

#include "common/tree/pst.h"
#include "common/gram/slr1.h"
#include "utils/view/ctx_view.h"

class SimpleLR1Parser
{
    SLR1Grammar grammar;
    pst_tree_ptr_t cst;
    pair<string, string> descAction(const action_t &act) const;
public:
    SimpleLR1Parser(SLR1Grammar &grammar) : grammar(grammar)
    {
        cst = pst_tree_t::createNode(TERMINAL, SYM_END, 0, 0);
    }
    bool parse(vector<token> &input, const ContextViewer &code);
    pst_tree_ptr_t getCST() { return cst; }
};

typedef SimpleLR1Parser SLR1Parser;
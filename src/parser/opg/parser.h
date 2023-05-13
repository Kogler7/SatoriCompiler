/**
 * @file opg/parser.h
 * @author Zhenjie Wei (2024108@bjtu.edu.cn)
 * @brief Operator Precedence Grammar Parser
 * @date 2023-05-12
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#include "common/gram/opg.h"
#include "common/tree/cst.h"

class OperatorPrecedenceGrammarParser
{
public:
    OperatorPrecedenceGrammar grammar;
    cst_tree_ptr tree;
    OperatorPrecedenceGrammarParser(OperatorPrecedenceGrammar &grammar) : grammar(grammar)
    {
        tree = cst_tree::createNode(TERMINAL, SYM_END, 0, 0);
    }
    bool parse(vector<token> &input);
    cst_tree_ptr getTree() { return tree; }
};
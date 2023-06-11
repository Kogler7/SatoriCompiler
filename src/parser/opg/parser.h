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
#include "common/tree/pst.h"

class OperatorPrecedenceGrammarParser
{
public:
    OperatorPrecedenceGrammar grammar;
    pst_tree_ptr_t cst;
    OperatorPrecedenceGrammarParser(OperatorPrecedenceGrammar &grammar) : grammar(grammar)
    {
        cst = pst_tree_t::createNode(TERMINAL, SYM_END, 0, 0);
    }
    bool parse(std::vector<token> &input);
    pst_tree_ptr_t getCST() { return cst; }
};
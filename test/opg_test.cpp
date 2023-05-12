/**
 * @file opg_test.cpp
 * @author Zhenjie Wei (2024108@bjtu.edu.cn)
 * @brief Operator Precedence Grammar Test
 * @date 2023-05-13
 *
 * @copyright Copyright (c) 2023
 *
 */

#include "test.h"
#include "common/gram/opg.h"
#include "lexer/lexer.h"
#include "parser/ebnf.h"
#include "parser/opg/parser.h"
#include "utils/log.h"

void opgTest()
{
    EBNFParser ebnfParser("./assets/syntax.lex");
    Grammar g = ebnfParser.parse("./assets/lab4.stx");
    OperatorPrecedenceGrammar G = OperatorPrecedenceGrammar(g);
    G.printRules();
    G.extractLeftCommonFactor();
    G.printRules();
    G.printTerminals();
    G.printNonTerms();
    G.printFirstVT();
    G.printLastVT();
    G.printOPT();
}
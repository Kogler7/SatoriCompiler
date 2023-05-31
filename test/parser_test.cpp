/**
 * @file parser_test.cpp
 * @author Zhenjie Wei (2024108@bjtu.edu.cn)
 * @brief Test Parser
 * @date 2023-05-06
 *
 * @copyright Copyright (c) 2023
 *
 */

#include "test.h"
#include "lexer/lexer.h"
#include "parser/ebnf.h"

void parserTest()
{
    EBNFParser ebnfParser("./assets/lex/syntax.lex");
    // Grammar G = ebnfParser.parse("./assets/lab3.stx");
    Grammar G = ebnfParser.parse("./assets/stx/select.stx");
    G.printRules();
    G.extractLeftCommonFactor();
    G.printRules();
    G.printTerminals();
    G.printNonTerms();
}
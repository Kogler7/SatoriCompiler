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
    EBNFParser ebnfParser("./assets/syntax.lex");
    Grammar G = ebnfParser.parse("./assets/lab2.stx");
    G.printRules();
    G.printTerminals();
    G.printNonTerms();
    G.calcFirst();
    G.calcFollow();
    G.printFirst();
    G.printFollow();
}
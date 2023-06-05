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
#include "parser/syntax.h"

void parserTest()
{
    SyntaxParser syntax("./assets/lex/syntax.lex");
    // Grammar G = syntax.parse("./assets/lab3.stx");
    Grammar G = syntax.parse("./assets/stx/select.stx");
    G.printRules();
    G.extractLeftCommonFactor();
    G.printRules();
    G.printTerminals();
    G.printNonTerms();
}
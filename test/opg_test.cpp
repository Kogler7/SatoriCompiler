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
#include "parser/syntax.h"
#include "parser/opg/parser.h"
#include "utils/log.h"

void opgTest()
{
    SyntaxParser syntax("./assets/lex/syntax.lex");
    Grammar g = syntax.parse("./assets/stx/lab4.stx");
    OperatorPrecedenceGrammar G = OperatorPrecedenceGrammar(g);
    G.printRules();
    G.extractLeftCommonFactor();
    G.printRules();
    G.printTerminals();
    G.printNonTerms();
    G.printFirstVT();
    G.printLastVT();
    G.printOPT();
    OperatorPrecedenceGrammarParser parser(G);
    Lexer lexer("./assets/lex/lab3.lex");
    vector<token> tokens = lexer.tokenizeFile("./assets/src/lab4.txt");
    Lexer::printTokens(tokens);
    tokens = G.transferTokens(tokens);
    Lexer::printTokens(tokens);
    parser.parse(tokens);
    parser.getTree()->print();
}
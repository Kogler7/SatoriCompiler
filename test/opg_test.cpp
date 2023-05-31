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
    EBNFParser ebnfParser("./assets/lex/syntax.lex");
    Grammar g = ebnfParser.parse("./assets/stx/lab4.stx");
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
    vector<token> tokens = lexer.tokenize("./assets/src/lab4.txt");
    lexer.printTokens(tokens);
    tokens = G.transferTokens(tokens);
    lexer.printTokens(tokens);
    parser.parse(tokens);
    parser.getTree()->print();
}
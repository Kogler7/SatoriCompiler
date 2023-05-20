/**
 * @file slr1_test.cpp
 * @author Zhenjie Wei (2024108@bjtu.edu.cn)
 * @brief Test SLR1 Parser
 * @date 2023-05-16
 *
 * @copyright Copyright (c) 2023
 *
 */

#include "test.h"
#include "lexer/lexer.h"
#include "parser/ebnf.h"
#include "parser/slr1/parser.h"
#include "utils/log.h"

void slr1Test()
{
    EBNFParser ebnfParser("./assets/syntax.lex");
    Grammar g = ebnfParser.parse("./assets/lab5r.stx");
    SLR1Grammar G = SLR1Grammar(g);
    G.printRules();
    G.extractLeftCommonFactor();
    G.printRules();
    G.printTerminals();
    G.printNonTerms();
    G.printFollow();
    SLR1Parser slr1(G);
    Lexer lexer("./assets/lab3.lex");
    auto tokens = lexer.tokenize("./assets/lab5.txt");
    lexer.printTokens(tokens);
    tokens = G.transferTokens(tokens);
    lexer.printTokens(tokens);
    G.printItems();
    G.printClusters();
    G.printGoTrans();
    G.printSLR1Table();
    info << "result: \n"
         << slr1.parse(tokens) << endl;
    slr1.getTree()->print();
}
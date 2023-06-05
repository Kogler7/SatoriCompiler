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
#include "parser/syntax.h"
#include "parser/slr1/parser.h"
#include "utils/log.h"

void slr1Test()
{
    SyntaxParser syntax("./assets/lex/syntax.lex");
    Grammar g = syntax.parse("./assets/stx/lab5r.stx");
    // Grammar g = syntax.parse("./assets/tmp/wjy.stx");
    SLR1Grammar G = SLR1Grammar(g);
    G.printRules();
    G.extractLeftCommonFactor();
    G.printRules();
    G.printTerminals();
    G.printNonTerms();
    G.printFollow();
    SLR1Parser slr1(G);
    Lexer lexer("./assets/lex/lab3.lex");
    Viewer code = Viewer::fromFile("./assets/src/lab5.txt");
    auto tokens = lexer.tokenize(code);
    Lexer::printTokens(tokens);
    tokens = G.transferTokens(tokens);
    Lexer::printTokens(tokens);
    G.printItems();
    G.printClusters();
    G.printGoTrans();
    G.printSLR1Table();
    info << "result: \n"
         << slr1.parse(tokens, code) << endl;
    slr1.getTree()->print();
}
/**
 * @file psl_test.cpp
 * @author Zhenjie Wei (2024108@bjtu.edu.cn)
 * @brief Test PSL
 * @date 2023-07-13
 *
 * @copyright Copyright (c) 2023
 *
 */

#include "test.h"
#include "lexer/lexer.h"
#include "parser/syntax.h"
#include "parser/eslr/parser.h"
#include "utils/log.h"

using namespace std;

void PSLTest()
{
    SyntaxParser syntax("./assets/lex/syntax.lex");
    // Grammar g = syntax.parse("./assets/stx/func.stx");
    Grammar g = syntax.parse("./assets/stx/psl.stx");
    g.printRules();
    g.printTerminals();
    g.printNonTerms();
    // g.printSemanticMarks();
    SLR1Grammar G = SLR1Grammar(g);
    G.printFollow();
    G.printItems();
    // G.printClusters();
    // G.printGoTrans();
    // G.printSLR1Table();
    // assert(G.checkSLR1(), "Not SLR(1) grammar!");
    G.checkSLR1();
    ESLR1Parser eslr1(G);
    // Lexer lexer("./assets/lex/func.lex");
    // Viewer code = Viewer::fromFile("./assets/src/func.txt");
    Lexer lexer("./assets/lex/psl.lex");
    // Viewer code = Viewer::fromFile("./assets/src/test.psl");
    Viewer code = Viewer::fromFile("./assets/src/roft.psl");
    auto tokens = lexer.tokenize(code);
    Lexer::printTokens(tokens);
    tokens = G.transferTokens(tokens);
    Lexer::printTokens(tokens);
    info << "result: \n"
         << eslr1.parse(tokens, code) << endl;
    eslr1.getCST()->print();
    eslr1.reduceCST()->print();
    eslr1.refactorRST()->print();
}
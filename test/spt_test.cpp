/**
 * @file spt_test.cpp
 * @author Zhenjie Wei (2024108@bjtu.edu.cn)
 * @brief Test Stack Predictive Table Parser
 * @date 2023-05-06
 *
 * @copyright Copyright (c) 2023
 *
 */

#include "test.h"
#include "lexer/lexer.h"
#include "parser/syntax.h"
#include "parser/spt/parser.h"
#include "utils/log.h"

void sptTest()
{
    SyntaxParser syntax("./assets/lex/syntax.lex");
    Grammar g = syntax.parse("./assets/stx/lab3.stx");
    // Grammar g = syntax.parse("./assets/wjy.stx");
    PredictiveGrammar G = PredictiveGrammar(g);
    G.printRules();
    G.extractLeftCommonFactor();
    G.printRules();
    G.printTerminals();
    G.printNonTerms();
    G.printFirst();
    G.printFollow();
    G.printFirstS();
    G.printSelect();
    cout << G.isLL1Grammar() << endl;
    StackPredictiveTableParser stp(G);
    stp.printPredictTable();
    Lexer lexer("./assets/lex/lab3.lex");
    auto tokens = lexer.tokenizeFile("./assets/src/lab3.txt");
    Lexer::printTokens(tokens);
    tokens = G.transferTokens(tokens);
    Lexer::printTokens(tokens);
    info << "result: \n" << stp.parse(tokens) << endl;
    stp.getCST()->print();
}
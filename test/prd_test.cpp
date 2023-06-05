/**
 * @file prd_test.cpp
 * @author Zhenjie Wei (2024108@bjtu.edu.cn)
 * @brief Test Predictive Recursive Descent Parser
 * @date 2023-05-11
 *
 * @copyright Copyright (c) 2023
 *
 */

#include "test.h"
#include "common/gram/predict.h"
#include "lexer/lexer.h"
#include "parser/syntax.h"
#include "parser/prd/parser.h"
#include "utils/log.h"

void prdTest()
{
    SyntaxParser syntax("./assets/lex/syntax.lex");
    Grammar g = syntax.parse("./assets/stx/rlcf.stx");
    g.printRules();
    g.extractLeftCommonFactor();
    g.printRules();
    // g.eliminateLeftRecursion();
    g.printTerminals();
    g.printNonTerms();
    PredictiveGrammar G = PredictiveGrammar(g);
    G.printFirst();
    G.printFollow();
    G.printFirstS();
    G.printSelect();
    cout << G.isLL1Grammar() << endl;
    PredictiveRecursiveDescentParser prd(G);
    Lexer lexer("./assets/lex/lab3.lex");
    auto tokens = lexer.tokenizeFile("./assets/src/lab3.txt");
    Lexer::printTokens(tokens);
    tokens = G.transferTokens(tokens);
    Lexer::printTokens(tokens);
    info << "result: \n"
         << prd.parse(tokens) << endl;
    prd.getTree()->print();
}
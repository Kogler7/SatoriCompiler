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
#include "parser/ebnf.h"
#include "parser/prd/parser.h"
#include "utils/log.h"

void prdTest()
{
    EBNFParser ebnfParser("./assets/lex/syntax.lex");
    Grammar g = ebnfParser.parse("./assets/stx/rlcf.stx");
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
    PredictiveRecursiveDescentParser prd(G);
    Lexer lexer("./assets/lex/lab3.lex");
    auto tokens = lexer.tokenize("./assets/src/lab3.txt");
    lexer.printTokens(tokens);
    tokens = G.transferTokens(tokens);
    lexer.printTokens(tokens);
    info << "result: \n"
         << prd.parse(tokens) << endl;
    prd.getTree()->print();
}
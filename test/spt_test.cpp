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
#include "parser/ebnf.h"
#include "parser/spt/parser.h"
#include "utils/log.h"

void sptTest()
{
    EBNFParser ebnfParser("./assets/syntax.lex");
    Grammar g = ebnfParser.parse("./assets/lab3.stx");
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
    Lexer lexer("./assets/lab3.lex");
    auto tokens = lexer.tokenize("./assets/lab3.txt");
    lexer.printTokens(tokens);
    tokens = G.transferTokens(tokens);
    lexer.printTokens(tokens);
    info << "result: \n" << stp.parse(tokens) << endl;
    stp.getTree()->print();
}
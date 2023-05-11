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
    Grammar G = ebnfParser.parse("./assets/lab3.stx");
    G.printRules();
    G.extractLeftCommonFactor();
    G.printRules();
    G.printTerminals();
    G.printNonTerms();
    G.calcFirst();
    G.calcFollow();
    G.calcSelect();
    G.printFirst();
    G.printFollow();
    G.printFirstP();
    G.printSelect();
    cout << G.isLL1Grammar() << endl;
    StackPredictiveTableParser pta(G);
    pta.calcPredictTable();
    pta.printPredictTable();
    Lexer lexer("./assets/lab3.lex");
    lexer.readSrcFile("./assets/lab3.txt");
    auto tokens = lexer.tokenize();
    lexer.printTokens();
    tokens = G.transferTokens(tokens);
    lexer.printTokens(tokens);
    info << "result: \n" << pta.parse(tokens);
}
/**
 * @file parser.cpp
 * @author Zhenjie Wei (2024108@bjtu.edu.cn)
 * @brief Parser
 * @date 2023-04-22
 *
 * @copyright Copyright (c) 2023
 *
 */

#include "lexer/lexer.h"
#include "parser.h"
#include "ebnf.h"

using namespace std;

void parserMain()
{
    // Lexer cppLexer("./assets/cpp.lex");
    // cppLexer.readCodeFile("./assets/error.cpp");
    // cppLexer.tokenize();
    // cppLexer.printTokens();
    EBNFParser ebnfParser("./assets/syntax.lex");
    Grammar G = ebnfParser.parse("./assets/lab2.stx");
    ebnfParser.getLexer().printTokens();
    G.printRules();
    G.printTerminals();
    G.printNonTerms();
    G.calcFirst();
    G.calcFollow();
    G.printFirst();
    G.printFollow();
}
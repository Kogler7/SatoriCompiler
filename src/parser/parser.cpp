/**
 * @file parser.cpp
 * @author Zhenjie Wei (2024108@bjtu.edu.cn)
 * @brief Parser
 * @date 2023-04-22
 *
 * @copyright Copyright (c) 2023
 *
 */

#include "../lexer/lexer.h"
#include "parser.h"

using namespace std;

void parserMain()
{
    // Lexer cppLexer("./assets/cpp.lex");
    // cppLexer.readCodeFile("./assets/error.cpp");
    // cppLexer.tokenize();
    // cppLexer.printTokens();
    Lexer ebnfLexer("./assets/ebnf.lex");
    ebnfLexer.readSrcFile("./assets/syntax.ebnf");
    ebnfLexer.tokenize();
    ebnfLexer.printTokens();
}
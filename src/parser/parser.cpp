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

using namespace std;

void test()
{
    Lexer cppLexer("cpp.lex");
    cppLexer.readCodeFile("test.c");
    cppLexer.tokenize();
    cppLexer.printTokens();
}
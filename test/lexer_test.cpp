/**
 * @file lexer_test.cpp
 * @author Zhenjie Wei (2024108@bjtu.edu.cn)
 * @brief Test Lexer
 * @date 2023-05-06
 *
 * @copyright Copyright (c) 2023
 *
 */

#include "test.h"
#include "lexer/lexer.h"

void lexerTest()
{
    Lexer lexer("./assets/cpp.lex");
    // lexer.readSrcFile("./assets/code.cpp");
    auto tokens = lexer.tokenize("./assets/error.cpp");
    lexer.printTokens(tokens);
}
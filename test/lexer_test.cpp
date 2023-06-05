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
    Lexer lexer("./assets/lex/cpp.lex");
    // Viewer codeViewer = Viewer::fromFile("./assets/src/code.cpp");
    Viewer codeViewer = Viewer::fromFile("./assets/src/error.cpp");
    auto tokens = lexer.tokenize(codeViewer);
    Lexer::printTokens(tokens);
}
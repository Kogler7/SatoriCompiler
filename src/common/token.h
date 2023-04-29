/**
 * @file token.h
 * @author Zhenjie Wei (2024108@bjtu.edu.cn)
 * @brief Token Definition
 * @date 2023-04-27
 *
 * @copyright Copyright (c) 2023
 *
 */

#pragma once
#include <string>

using namespace std;
typedef unsigned int TokenType;

/**
 * @brief Token
 */
struct token
{
    TokenType type;
    string value;
    int line;
    int col;
    token() : type(0), value(""), line(0), col(0) {}
    token(TokenType type, string value, int line, int col) : type(type), value(value), line(line), col(col) {}
};

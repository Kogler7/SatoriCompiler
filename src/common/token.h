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
    bool reserved;
    unsigned int rvId;
    token(TokenType type, string value, bool reserved = false, unsigned int rvId = 0) : type(type), value(value), reserved(reserved), rvId(rvId) {}
};

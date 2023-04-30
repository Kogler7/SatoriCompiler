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
#include <memory>

using namespace std;

typedef shared_ptr<string> token_type;

#define type_less owner_less<token_type>
#define make_type(x) make_shared<string>(x)

using namespace std;

/**
 * @brief Token
 */
struct token
{
    token_type type;
    string value;
    int line;
    int col;
    token(token_type type, string value, int line, int col) : type(type), value(value), line(line), col(col) {}
};

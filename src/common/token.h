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
#include <map>
#include <string>
#include <memory>

using namespace std;

typedef shared_ptr<string> token_type_t;
#define token_iter_t vector<token>::iterator
#define token_const_iter_t vector<token>::const_iterator

#define type_less owner_less<token_type_t>
#define make_tok_type(x) make_shared<string>(x)
#define find_tok_type(x) (tokTypeMap.find(x) != tokTypeMap.end())
#define set_tok_type(x, y) tokTypeMap[x] = y
#define get_tok_type(x) tokTypeMap[x]

extern map<string, token_type_t> tokTypeMap;

/**
 * @brief Token
 */
struct token
{
    token_type_t type;
    string value;
    size_t line;
    size_t col;
    token(token_type_t type, string value) : type(type), value(value), line(0), col(0) {}
    token(token_type_t type, string value, int line, int col) : type(type), value(value), line(line), col(col) {}
};
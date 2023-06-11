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

using token_type_t = std::shared_ptr<std::string>;
#define token_iter_t std::vector<token>::iterator
#define token_const_iter_t std::vector<token>::const_iterator

#define type_less std::owner_less<token_type_t>
#define make_tok_type(x) std::make_shared<std::string>(x)
#define find_tok_type(x) (tokTypeMap.find(x) != tokTypeMap.end())
#define set_tok_type(x, y) tokTypeMap[x] = y
#define get_tok_type(x) tokTypeMap[x]

extern std::map<std::string, token_type_t> tokTypeMap;

/**
 * @brief Token
 */
struct token
{
    token_type_t type;
    std::string value;
    size_t line;
    size_t col;
    token(token_type_t type, std::string value) : type(type), value(value), line(0), col(0) {}
    token(token_type_t type, std::string value, int line, int col) : type(type), value(value), line(line), col(col) {}
};
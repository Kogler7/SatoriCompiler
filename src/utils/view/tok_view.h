/**
 * @file utils/token_viewer.h
 * @author Zhenjie Wei (2024108@bjtu.edu.cn)
 * @brief Lightweight Token Viewer
 * @date 2023-04-27
 *
 * @copyright Copyright (c) 2023
 *
 */

#pragma once

#include "common/token.h"
#include "utils/log.h"

#include <vector>

class TokenViewer
{
    std::vector<token> &tokens;
    size_t index;

public:
    TokenViewer(std::vector<token> &tokens) : tokens(tokens), index(0) {}
    void operator=(TokenViewer &v)
    {
        tokens = v.tokens;
        index = v.index;
    }
    token &operator[](size_t i) const
    {
        assert(i >= 0 && i < tokens.size());
        return tokens[i];
    }
    size_t pos() const
    {
        return index;
    }
    size_t size() const
    {
        return tokens.size();
    }
    bool ends() const
    {
        return index >= tokens.size();
    }
    token &current() const
    {
        return (*this)[index];
    }
    void advance()
    {
        index++;
    }
    std::vector<token> rest() const
    {
        std::vector<token> ret;
        for (size_t i = index; i < tokens.size(); i++)
            ret.push_back(tokens[i]);
        return ret;
    }
    std::vector<std::string> restTypes() const
    {
        std::vector<std::string> ret;
        for (size_t i = index; i < tokens.size(); i++)
            ret.push_back(*(tokens[i].type));
        return ret;
    }
};

inline std::string descTokVecFrom(const std::vector<token> &v, int i, int limit = 10)
{
    std::stringstream ss;
    for (int j = i; j < v.size() && j < i + limit; j++)
    {
        ss << v[j].value;
        if (j != v.size() - 1)
            ss << " ";
    }
    return ss.str();
}
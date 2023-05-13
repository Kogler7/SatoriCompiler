/**
 * @file utils/tok_view.h
 * @author Zhenjie Wei (2024108@bjtu.edu.cn)
 * @brief Lightweight Token Viewer
 * @date 2023-04-27
 *
 * @copyright Copyright (c) 2023
 *
 */

#include "common/token.h"
#include "vector"
#include "log.h"

using namespace std;

class TokenViewer
{
    vector<token> &tokens;
    size_t index;

public:
    TokenViewer(vector<token> &tokens) : tokens(tokens), index(0) {}
    void operator=(TokenViewer &v)
    {
        tokens = v.tokens;
        index = v.index;
    }
    token &operator[](size_t i)
    {
        assert(i >= 0 && i < tokens.size());
        return tokens[i];
    }
    size_t pos()
    {
        return index;
    }
    size_t size()
    {
        return tokens.size();
    }
    bool ends()
    {
        return index >= tokens.size();
    }
    token &current()
    {
        return (*this)[index];
    }
    void advance()
    {
        index++;
    }
    vector<token> rest()
    {
        vector<token> ret;
        for (size_t i = index; i < tokens.size(); i++)
            ret.push_back(tokens[i]);
        return ret;
    }
    vector<string> restTypes()
    {
        vector<string> ret;
        for (size_t i = index; i < tokens.size(); i++)
            ret.push_back(*(tokens[i].type));
        return ret;
    }
};
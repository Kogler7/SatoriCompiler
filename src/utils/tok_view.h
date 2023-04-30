/**
 * @file tok_view.h
 * @author Zhenjie Wei (2024108@bjtu.edu.cn)
 * @brief Lightweight Token Viewer
 * @date 2023-04-27
 *
 * @copyright Copyright (c) 2023
 *
 */

#include "common/token.h"
#include "vector"

using namespace std;

class TokenViewer
{
    vector<token> tokens;
    int index;

public:
    TokenViewer(vector<token> tokens) : tokens(tokens), index(0) {}
    void operator=(TokenViewer &v)
    {
        tokens = v.tokens;
        index = v.index;
    }
    token operator[](int i)
    {
        if (i < 0 || i >= tokens.size())
        {
            return token();
        }
        return tokens[i];
    }
    int size()
    {
        return tokens.size();
    }
    bool ends()
    {
        return index >= tokens.size();
    }
    token current()
    {
        return (*this)[index];
    }
    void advance()
    {
        index++;
    }
};
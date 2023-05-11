/**
 * @file prd/parser.cpp
 * @author Zhenjie Wei (2024108@bjtu.edu.cn)
 * @brief Predictive Recursive Descent Parsing
 * @date 2023-04-22
 *
 * @copyright Copyright (c) 2023
 *
 */

#include "parser.h"
#include "utils/log.h"

#define _find(s, t) (s.find(t) != s.end())
#define _cur_tok(v) *(v.current().type)

bool PredictiveRecursiveDescentParser::parseNonTerm(TokenViewer &viewer, symbol t)
{
    for (auto &right : grammar.rules[t])
    {
        if (right.size() == 0)
        {
            // 空产生式，即epsilon
            symset &follow = grammar.follow[t];
            if (_find(follow, _cur_tok(viewer)))
            {
                // 分析成功
                return true;
            }
        }
        else
        {
            symset &first = grammar.firstS[right];
            if (_find(first, _cur_tok(viewer)))
            {
                // 根据first集预测选择唯一一个产生式，若分析失败不再考察其他产生式
                for (auto symIt = ++first.begin(); symIt != first.end(); symIt++)
                {
                    viewer.advance();
                    if (_find(grammar.terminals, *symIt))
                    {
                        // 终结符
                        if (_cur_tok(viewer) != *symIt)
                        {
                            error << "PRDParser: parseNonTerm: terminal not match."
                                  << format(" Expected: $, got: $.", *symIt, _cur_tok(viewer));
                            return false; // 分析失败
                        }
                    }
                    else
                    {
                        // 非终结符
                        if (!parseNonTerm(viewer, *symIt))
                        {
                            error << "PRDParser: parseNonTerm: non-terminal not match."
                                  << format(" Expected: $, got: $.", *symIt, _cur_tok(viewer));
                            return false; // 分析失败
                        }
                    }
                }
                return true;
            }
        }
    }
    error << "PRDParser: parseNonTerm: no production matched.";
    return false;
}

bool PredictiveRecursiveDescentParser::parse(vector<token> &input)
{
    TokenViewer viewer(input);
    if (parseNonTerm(viewer, grammar.symStart))
    {
        info << "PRDParser: parse succeed.";
        return true;
    }
    else
    {
        error << "PRDParser: parse failed.";
        return false;
    }
}
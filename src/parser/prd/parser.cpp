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

bool PredictiveRecursiveDescentParser::parseNonTerm(TokenViewer &viewer, term t)
{
    for (auto &right : grammar.rules[t])
    {
        TokenViewer tmpViewer = viewer;
        if (right.size() == 0)
        {
            // 空产生式，即epsilon
            set<term> &follow = grammar.follow[t];
            if (_find(follow, _cur_tok(tmpViewer)))
            {
                // 预测成功
                viewer = tmpViewer;
                return true;
            }
        }
        else
        {
            production p = make_pair(t, right);
            set<term> &first = grammar.firstP[p];
            if (_find(first, _cur_tok(tmpViewer)))
            {
                // 预测成功
                for (auto symIt = ++first.begin(); symIt != first.end(); symIt++)
                {
                    tmpViewer.advance();
                    if (_find(grammar.terminals, *symIt))
                    {
                        // 终结符
                        if (_cur_tok(tmpViewer) != *symIt)
                        {
                            error << "PRDParser: parseNonTerm: terminal not match."
                                  << format(" Expected: $, got: $.", *symIt, _cur_tok(tmpViewer));
                            return false; // 预测失败
                        }
                    }
                    else
                    {
                        // 非终结符
                        if (!parseNonTerm(tmpViewer, *symIt))
                        {
                            error << "PRDParser: parseNonTerm: non-terminal not match."
                                  << format(" Expected: $, got: $.", *symIt, _cur_tok(tmpViewer));
                            return false; // 预测失败
                        }
                    }
                }
                viewer = tmpViewer;
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
    if (parseNonTerm(viewer, grammar.startTerm))
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
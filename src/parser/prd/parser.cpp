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

#define DEBUG_LEVEL 0

#define _find(s, t) (s.find(t) != s.end())
#define _cur_tok(v) *(v.current().type)

bool PredictiveRecursiveDescentParser::parseNonTerm(TokenViewer &viewer, symbol sym, cst_node_ptr node)
{
    for (auto &right : grammar.rules[sym])
    {
        if (right.size() == 0)
        {
            // 空产生式，即epsilon
            debug(0) << format("Viewing null production: $ -> $\n", sym, str2str(right));
            symset &follow = grammar.follow[sym];
            debug(0) << format("Follow set: $.\n", set2str(follow));
            if (_find(follow, _cur_tok(viewer)))
            {
                // 分析成功
                token &tok = viewer.current();
                *(node) << cst_tree::createNode(TERMINAL, EPSILON, tok.line, tok.col);
                return true;
            }
        }
        else
        {
            symset &first = grammar.firstS[right];
            if (_find(first, _cur_tok(viewer)))
            {
                debug(0) << format("Viewing production: $ -> $\n", sym, str2str(right));
                debug(0) << format("First set: $.\n", set2str(first));
                // 根据first集预测选择唯一一个产生式，若分析失败不再考察其他产生式
                for (auto symIt = right.begin(); symIt != right.end(); symIt++)
                {
                    token &tok = viewer.current();
                    debug(0) << format("Viewer currently focuses on: $.\n", tok.value);
                    if (_find(grammar.terminals, *symIt))
                    {
                        // 终结符
                        if (_cur_tok(viewer) != *symIt)
                        {
                            error << "PRDParser: parseNonTerm: terminal not match."
                                  << format(" Expected: $, got: $.\n", *symIt, _cur_tok(viewer));
                            return false; // 分析失败
                        }
                        *(node) << cst_tree::createNode(TERMINAL, viewer.current().value, tok.line, tok.col);
                        info << format("PRDParser: parseNonTerm: terminal matched: $.\n", *symIt);
                        viewer.advance();
                        debug(0) << format("Viewer advanced to: $.\n", viewer.current().value);
                    }
                    else
                    {
                        // 非终结符
                        cst_node_ptr child = cst_tree::createNode(NON_TERM, *symIt, tok.line, tok.col);
                        *(node) << child;
                        info << format("PRDParser: parseNonTerm: goto nonTerm: $.\n", *symIt);
                        if (!parseNonTerm(viewer, *symIt, child))
                        {
                            error << "PRDParser: parseNonTerm: non-terminal not match."
                                  << format(" Expected: $, got: $.\n", *symIt, _cur_tok(viewer));
                            return false; // 分析失败
                        }
                    }
                }
                return true;
            }
        }
    }
    error << "PRDParser: parseNonTerm: no production matched." << endl;
    return false;
}

bool PredictiveRecursiveDescentParser::parse(vector<token> &input)
{
    input.push_back(token(make_shared<symbol>(SYM_END), SYM_END, 0, 0));
    TokenViewer viewer(input);
    cst_tree_ptr root = cst_tree::createNode(NON_TERM, grammar.symStart, 0, 0);
    if (parseNonTerm(viewer, grammar.symStart, root))
    {
        info << "PRDParser: parse succeed." << endl;
        tree = root;
        tree->print();
        return true;
    }
    else
    {
        error << "PRDParser: parse failed." << endl;
        tree = root;
        tree->print();
        return false;
    }
}
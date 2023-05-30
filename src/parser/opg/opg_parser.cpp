/**
 * @file opg/parser.cpp
 * @author Zhenjie Wei (2024108@bjtu.edu.cn)
 * @brief Operator Precedence Grammar Parser
 * @date 2023-04-22
 *
 * @copyright Copyright (c) 2023
 *
 */

#include "utils/log.h"
#include "utils/stl.h"
#include "utils/table.h"
#include "utils/tok_view.h"
#include "parser.h"
#include <stack>

#define DEBUG_LEVEL 0

using namespace table;

#define _isVT(t) _find(grammar.terminals, t)
#define _isVN(t) _find(grammar.nonTerms, t)
#define _lt(t1, t2) (grammar.opt[mkcrd(t1, t2)] == OP::LT)
#define _eq(t1, t2) (grammar.opt[mkcrd(t1, t2)] == OP::EQ)
#define _gt(t1, t2) (grammar.opt[mkcrd(t1, t2)] == OP::GT)

typedef std::pair<symbol_t, symstr_t> product_t;

symbol_t findLeft(const symstr_t &right, const Grammar &grammar)
{
    const vector<product_t> &products = grammar.products;
    for (auto &p : products)
    {
        if (p.second.size() != right.size())
        {
            continue;
        }
        bool flag = true;
        for (size_t i = 0; i < p.second.size(); i++)
        {
            if (_isVT(p.second[i]))
            {
                if (p.second[i] != right[i])
                {
                    flag = false;
                    break;
                }
            }
            else
            {
                if (_isVT(right[i]))
                {
                    flag = false;
                    break;
                }
            }
        }
        if (flag)
        {
            return p.first;
        }
    }
    return "";
}

bool OperatorPrecedenceGrammarParser::parse(vector<token> &input)
{
    info << "OPGParser: parsing..." << endl;
    input.push_back(token(make_shared<symbol_t>(SYM_END), SYM_END, 0, 0));
    TokenViewer viewer(input);
    vector<symbol_t> stk;
    stk.push_back(SYM_END);
    tb_head | "Stack" | "Priority" | "Input" | MD_TAB | "Action";
    set_col | AL_LFT | AL_CTR | AL_RGT | AL_RGT | AL_LFT;
    while (!stk.empty() && !viewer.ends())
    {
        assert(stk.size() >= 1, "OPGParser: invalid stack.");
        int cursor = stk.size() - 1;
        symbol_t top = stk.back();
        symbol_t cur = *(viewer.current().type);
        new_row | compact(stk);
        if (top == grammar.symStart && cur == SYM_END)
        {
            tb_line(-1);
            tb_cont | TB_TAB | TB_TAB | MD_TAB | "Accepted" = table::FORE_GRE;
            cout << tb_view();
            info << "OPGParser: parsing succeeded." << endl;
            return true;
        }
        if (_isVN(top))
        {
            assert(stk.size() >= 2, "OPGParser: invalid stack.");
            cursor--;
            top = stk[cursor];
        }
        if (!_gt(top, cur))
        {
            tb_cont | top + (grammar.opt[mkcrd(top, cur)] == OP::LT ? "<" : "=") + cur;
            tb_cont | compact(viewer.restTypes());
            tb_cont | "Shift" | cur;
            stk.push_back(cur);
            viewer.advance();
        }
        else
        {
            cursor--;
            while (cursor >= 0)
            {
                symbol_t now = stk[cursor];
                if (!_isVT(now))
                {
                    cursor--;
                    continue;
                }
                if (_lt(now, top))
                {
                    symstr_t right(stk.begin() + cursor + 1, stk.end());
                    tb_cont | now + "<" + top + ">" + cur | compact(viewer.restTypes());
                    stk.erase(stk.begin() + cursor + 1, stk.end());
                    symbol_t left = findLeft(right, grammar);
                    tb_cont | "Reduce" | left + "->" + compact(right);
                    if (left == "")
                    {
                        error << "Product " << now << "->" << str2str(right) << " not found." << endl;
                        cout << tb_view();
                        return false;
                    }
                    stk.push_back(left);
                    break;
                }
                top = now;
                cursor--;
            }
        }
    }
    cout << tb_view();
    info << "OPGParser: parsing failed." << endl;
    return false;
}
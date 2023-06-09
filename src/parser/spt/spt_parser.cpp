/**
 * @file spt/parser.cpp
 * @author Zhenjie Wei (2024108@bjtu.edu.cn)
 * @brief Stack-based Parsing with Predictive Table
 * @date 2023-04-27
 *
 * @copyright Copyright (c) 2023
 *
 */

#include "parser.h"
#include "utils/stl.h"
#include "utils/log.h"
#include "utils/table.h"
#include "utils/view/tok_view.h"
#include <stack>

using namespace std;
using namespace table;

void StackPredictiveTableParser::calcPredictTable()
{
    for (auto p : grammar.products)
    {
        auto first = grammar.firstS[p.second];
        for (auto t : first)
        {
            if (t != EPSILON)
            {
                predict[mkcrd(p.first, t)] = p.second;
            }
        }
        if (_find(first, EPSILON))
        {
            auto follow = grammar.follow[p.first];
            for (auto t : follow)
            {
                if (_find(grammar.terminals, t))
                    predict[mkcrd(p.first, t)] = p.second;
            }
        }
    }
}

void StackPredictiveTableParser::printPredictTable() const
{
    info << "Predictive Table: " << std::endl;
    tb_head | "Non-Term";
    for (auto t : grammar.terminals)
    {
        if (t != EPSILON)
        {
            tb_cont | t;
            tb_cont = AL_CTR;
        }
    }
    for (auto nonTerm : grammar.nonTerms)
    {
        new_row | nonTerm;
        for (auto terminal : grammar.terminals)
        {
            if (terminal != EPSILON)
            {
                auto crd = mkcrd(nonTerm, terminal);
                if (predict.find(crd) != predict.end())
                {
                    std::string production = nonTerm + " -> " + compact(predict.at(crd));
                    tb_cont | production;
                }
                else
                {
                    tb_cont | TB_GAP;
                }
            }
        }
        tb_line();
    }
    cout << tb_view() << std::endl;
}

#define top_sym(s) ((s.top())->data.symbol)

string descStack(stack<pst_node_ptr_t> s)
{
    stringstream ss;
    symstr_t v;
    while (!s.empty())
    {
        v.push_back(top_sym(s));
        s.pop();
    }
    reverse(v.begin(), v.end());
    ss << vec2str(v);
    return ss.str();
}

bool StackPredictiveTableParser::parse(vector<token> input)
{
    stack<pst_node_ptr_t> s;
    input.push_back(token(make_shared<symbol_t>(SYM_END), SYM_END, 0, 0));
    TokenViewer viewer(input);
    s.push(pst_tree_t::createNode(TERMINAL, SYM_END, 0, 0));
    pst_node_ptr_t startNode = pst_tree_t::createNode(NON_TERM, grammar.symStart, 0, 0);
    s.push(startNode);
    tb_head | "Analyze Stack" | "Remaining Input" | "Action";
    set_col | AL_LFT | AL_RGT | AL_RGT;
    new_row | descStack(s) | descTokVecFrom(input, 0) | "Initial";
    while (top_sym(s) != SYM_END && !viewer.ends())
    {
        token &cur = viewer.current();
        size_t idx = viewer.pos();
        string actionDesc;
        symbol_t curSym = top_sym(s);
        symbol_t curType = *(cur.type);
        assert(_find(grammar.terminals, curType));
        if (curSym == curType)
        {
            pst_node_ptr_t topNode = s.top();
            topNode->data.symbol = cur.value;
            topNode->data.line = cur.line;
            topNode->data.col = cur.col;
            s.pop();
            viewer.advance();
            actionDesc = "Matched " + cur.value;
        }
        else if (_find(grammar.nonTerms, curSym))
        {
            auto crd = mkcrd(curSym, curType);
            if (predict.find(crd) != predict.end())
            {
                symstr_t &right = predict[crd];
                pst_node_ptr_t topNode = s.top();
                s.pop();
                vector<pst_node_ptr_t> children;
                for (auto it1 = right.rbegin(); it1 != right.rend(); it1++)
                {
                    node_type type = _find(grammar.terminals, *it1) ? TERMINAL : NON_TERM;
                    pst_node_ptr_t newNode = pst_tree_t::createNode(type, *it1, 0, 0);
                    children.push_back(newNode);
                    s.push(newNode);
                }
                // 保证树节点的顺序
                for (auto it1 = children.rbegin(); it1 != children.rend(); it1++)
                {
                    *(topNode) << *it1;
                }
                if (children.size() == 0)
                {
                    // 空串
                    *(topNode) << pst_tree_t::createNode(TERMINAL, EPSILON, 0, 0);
                }
                actionDesc = curType + " -> " + compact(right);
            }
            else
            {
                error << format(
                    "StackPredictiveTableParser: Unexpected token: $ at <$, $>.\n",
                    cur.value, cur.line, cur.col);
                cout << tb_view();
                return false;
            }
        }
        else
        {
            error << format(
                "StackPredictiveTableParser: Unexpected token: $ at <$, $>.\n",
                cur.value, cur.line, cur.col);
            cout << tb_view();
            return false;
        }
        new_row | descStack(s) | descTokVecFrom(input, viewer.pos()) | actionDesc;
    }
    tb_line();
    new_row | TB_TAB | MD_TAB | "Accepted";
    info << "Analyze finished." << std::endl;
    cout << tb_view();
    info << "Parse Tree: " << std::endl;
    cst = startNode;
    return true;
}
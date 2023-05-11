/**
 * @file pta.cpp
 * @author Zhenjie Wei (2024108@bjtu.edu.cn)
 * @brief Predictive Table Analyzer
 * @date 2023-04-27
 *
 * @copyright Copyright (c) 2023
 *
 */

#include "pta.h"
#include "utils/log.h"
#include "utils/table.h"
#include "utils/tok_view.h"
#include <iomanip>
#include <stack>

using namespace std;

#define _find(s, t) (s.find(t) != s.end())

void PredictiveTableAnalyzer::calcPredictTable()
{
    for (auto pdt : grammar.products)
    {
        auto first = grammar.firstP[pdt];
        for (auto t : first)
        {
            if (t != EPSILON)
            {
                predict[pdt.first][t] = pdt.second;
            }
        }
        if (_find(first, EPSILON))
        {
            auto follow = grammar.follow[pdt.first];
            for (auto t : follow)
            {
                if (_find(grammar.terminals, t))
                    predict[pdt.first][t] = pdt.second;
            }
        }
    }
}

void PredictiveTableAnalyzer::printPredictTable()
{
    info << "Predictive Table: " << std::endl;
    tb_head | "Non-Term";
    for (auto t : grammar.terminals)
    {
        if (t != EPSILON)
        {
            tb_cont | t;
            set_cur_col(AL_MID);
        }
    }
    for (auto nonTerm : grammar.nonTerms)
    {
        set_row | nonTerm;
        for (auto terminal : grammar.terminals)
        {
            if (terminal != EPSILON)
            {
                auto it = predict[nonTerm].find(terminal);
                if (it != predict[nonTerm].end())
                {
                    std::string production = nonTerm + " -> " + compact(it->second);
                    tb_cont | production;
                }
                else
                {
                    tb_cont | TB_GAP;
                }
            }
        }
        tb_line;
    }
    cout << tb_view << std::endl;
}

#define top_sym(s) ((s.top())->data.symbol)

string descStack(stack<cst_node_ptr> s)
{
    stringstream ss;
    vector<term> v;
    while (!s.empty())
    {
        v.push_back(top_sym(s));
        s.pop();
    }
    reverse(v.begin(), v.end());
    ss << vec2str(v);
    return ss.str();
}

string descVecFrom(vector<token> v, int i)
{
    stringstream ss;
    for (int j = i; j < v.size(); j++)
    {
        ss << v[j].value << " ";
    }
    return ss.str();
}

bool PredictiveTableAnalyzer::analyze(vector<token> input)
{
    stack<cst_node_ptr> s;
    input.push_back(token(make_shared<term>(SYM_END), SYM_END, 0, 0));
    TokenViewer viewer(input);
    s.push(cst_tree::createNode(TERMINAL, SYM_END, 0, 0));
    cst_node_ptr startNode = cst_tree::createNode(NON_TERM, grammar.startTerm, 0, 0);
    s.push(startNode);
    tb_head | "Analyze Stack" | "Remaining Input" | "Action";
    set_col | AL_LFT | AL_RGT | AL_RGT;
    set_row | descStack(s) | descVecFrom(input, 0) | "Initial";
    while (top_sym(s) != SYM_END && !viewer.ends())
    {
        token &cur = viewer.current();
        size_t idx = viewer.pos();
        string actionDesc;
        term curSym = top_sym(s);
        term curType = *(cur.type);
        assert(_find(grammar.terminals, curType));
        if (curSym == curType)
        {
            cst_node_ptr topNode = s.top();
            topNode->data.symbol = cur.value;
            topNode->data.line = cur.line;
            topNode->data.col = cur.col;
            s.pop();
            viewer.advance();
            stringstream ss;
            ss << "Matched " << cur.value;
            actionDesc = ss.str();
        }
        else if (_find(grammar.nonTerms, curSym))
        {
            auto it = predict[curSym].find(curType);
            if (it != predict[curSym].end())
            {
                cst_node_ptr topNode = s.top();
                s.pop();
                for (auto it1 = it->second.rbegin(); it1 != it->second.rend(); it1++)
                {
                    node_type type = _find(grammar.terminals, *it1) ? TERMINAL : NON_TERM;
                    cst_node_ptr newNode = cst_tree::createNode(type, *it1, 0, 0);
                    *(topNode) << newNode;
                    s.push(newNode);
                }
                stringstream ss;
                ss << it->first << "->" << compact(it->second);
                actionDesc = ss.str();
            }
            else
            {
                error << "PredictiveTableAnalyzer: "
                      << "Unexpected token: " << cur.value << " at " << cur.line
                      << ":" << cur.col << ": " << std::endl;
                return false;
            }
        }
        else
        {
            error << "PredictiveTableAnalyzer: "
                  << "Unexpected token: " << cur.value << " at " << cur.line
                  << ":" << cur.col << ": " << std::endl;
            return false;
        }
        set_row | descStack(s) | descVecFrom(input, viewer.pos()) | actionDesc;
    }
    tb_line;
    set_row | TB_TAB | MD_TAB | "Accepted";
    info << "Analyze finished." << std::endl;
    cout << tb_view;
    info << "Parse Tree: " << std::endl;
    startNode->print();
    return true;
}
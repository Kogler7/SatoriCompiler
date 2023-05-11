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
        set_row;
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
                    tb_cont | "";
                }
            }
        }
        tb_line;
    }
    cout << tb_view << std::endl;
}

string descStack(stack<term> s)
{
    stringstream ss;
    vector<term> v;
    while (!s.empty())
    {
        v.push_back(s.top());
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
    stack<term> s;
    input.push_back(token(make_shared<term>(SYM_END), SYM_END, 0, 0));
    TokenViewer viewer(input);
    s.push(SYM_END);
    s.push(grammar.startTerm);
    tb_head | "Stack" | "Input" | "Action";
    set_col | AL_LFT | AL_RGT | AL_RGT;
    while (s.top() != SYM_END && !viewer.ends())
    {
        token &cur = viewer.current();
        size_t idx = viewer.pos();
        string actionDesc;
        assert(_find(grammar.terminals, *(cur.type)));
        if (s.top() == *(cur.type))
        {
            s.pop();
            viewer.advance();
            stringstream ss;
            ss << "Matched " << cur.value;
            actionDesc = ss.str();
        }
        else if (_find(grammar.nonTerms, s.top()))
        {
            auto it = predict[s.top()].find(*(cur.type));
            if (it != predict[s.top()].end())
            {
                s.pop();
                for (auto it1 = it->second.rbegin(); it1 != it->second.rend(); it1++)
                {
                    s.push(*it1);
                }
                stringstream ss;
                ss << it->first << "->" << compact(it->second);
                actionDesc = ss.str();
            }
            else
            {
                error << "Error: " << cur.line << ":" << cur.col << ": "
                      << "Unexpected token: " << cur.type << std::endl;
                return false;
            }
        }
        else
        {
            error << "Error: " << cur.line << ":" << cur.col << ": "
                  << "Unexpected token: " << cur.type << std::endl;
            return false;
        }
        set_row | descStack(s) | descVecFrom(input, idx) | actionDesc;
    }
    set_row | descStack(s) | descVecFrom(input, viewer.pos()) | "Accepted";
    info << "Analyze finished." << std::endl << tb_view;
    return true;
}
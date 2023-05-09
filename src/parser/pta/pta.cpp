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

    // Table border
    const std::string borderLine(10 + 20 * grammar.terminals.size() + grammar.nonTerms.size(), '-');

    // 表头
    std::cout << borderLine << std::endl;
    std::cout << "| " << std::setw(8) << std::left << "Non-Term"
              << " |";
    for (auto t : grammar.terminals)
    {
        if (t != EPSILON)
        {
            std::cout << " " << std::setw(18) << std::left << t << " |";
        }
    }
    std::cout << std::endl;
    std::cout << borderLine << std::endl;

    // 行
    for (auto nonTerm : grammar.nonTerms)
    {
        std::cout << "| " << std::setw(8) << std::left << nonTerm << " |";

        for (auto terminal : grammar.terminals)
        {
            if (terminal != EPSILON)
            {
                auto it = predict[nonTerm].find(terminal);
                if (it != predict[nonTerm].end())
                {
                    std::string production = nonTerm + " -> " + vec2str(it->second);
                    std::cout << " " << std::setw(18) << std::left << production << " |";
                }
                else
                {
                    std::cout << " " << std::setw(18) << std::left << " "
                              << " |";
                }
            }
        }
        std::cout << std::endl;
        std::cout << borderLine << std::endl;
    }
}

void printStack(stack<term> s)
{
    std::cout << setw(30) << std::left;
    vector<term> v;
    while (!s.empty())
    {
        v.push_back(s.top());
        s.pop();
    }
    reverse(v.begin(), v.end());
    cout << vec2str(v);
}

void printVecFrom(vector<token> v, int i)
{
    stringstream ss;
    for (int j = i; j < v.size(); j++)
    {
        ss << v[j].value << " ";
    }
    cout << ss.str();
}

bool PredictiveTableAnalyzer::analyze(vector<token> input)
{
    stack<term> s;
    input.push_back(token(make_shared<term>(SYM_END), SYM_END, 0, 0));
    s.push(SYM_END);
    s.push(grammar.startTerm);
    int i = 0;
    while (!s.empty() && s.top() != SYM_END && i < input.size())
    {
        std::cout << setw(8) << std::left;
        printStack(s);
        std::cout << " | ";
        std::cout << setw(60) << std::right;
        printVecFrom(input, i);
        assert(_find(grammar.terminals, *(input[i].type)));
        if (s.top() == *(input[i].type))
        {
            s.pop();
            i++;
            cout << setw(4) << right << " $ ";
            stringstream ss;
            ss << "match " << input[i - 1].value;
            cout << setw(40) << right << ss.str();
            cout << endl;
        }
        else if (_find(grammar.nonTerms, s.top()))
        {
            auto it = predict[s.top()].find(*(input[i].type));
            if (it != predict[s.top()].end())
            {
                s.pop();
                for (auto it1 = it->second.rbegin(); it1 != it->second.rend(); it1++)
                {
                    s.push(*it1);
                }
                cout << setw(4) << right << " $ ";
                stringstream ss;
                ss << it->first << "->" << vec2str(it->second);
                cout << setw(40) << right << ss.str();
                cout << endl;
            }
            else
            {
                error << "Error: " << input[i].line << ":" << input[i].col << ": "
                      << "Unexpected token: " << input[i].type << std::endl;
                return false;
            }
        }
        else
        {
            error << "Error: " << input[i].line << ":" << input[i].col << ": "
                  << "Unexpected token: " << input[i].type << std::endl;
            return false;
        }
    }
    std::cout << setw(8) << std::left;
    printStack(s);
    std::cout << " | ";
    std::cout << setw(60) << std::right;
    printVecFrom(input, i);
    std::cout << std::endl;
    return true;
}
/**
 * @file gram.cpp
 * @author Zhenjie Wei (2024108@bjtu.edu.cn)
 * @brief Grammar
 * @date 2023-04-29
 *
 * @copyright Copyright (c) 2023
 *
 */

#include "gram.h"
#include "../utils/log.h"

#define _find(x, y) (x.find(y) != x.end())

#define DEBUG_LEVEL 1

Grammar::Grammar(term start, set<term> terms, set<term> non_terms, map<term, set<vector<term>>> rules)
{
    startTerm = start;
    terminals = terms;
    nonTerms = non_terms;
    this->rules = rules;
}

void Grammar::operator=(const Grammar &g)
{
    startTerm = g.startTerm;
    terminals = g.terminals;
    nonTerms = g.nonTerms;
    rules = g.rules;
}

void Grammar::printRules()
{
    info << "EBNFParser: Rules:" << endl;
    for (auto it = rules.begin(); it != rules.end(); it++)
    {
        for (auto it2 = it->second.begin(); it2 != it->second.end(); it2++)
        {
            cout << it->first << " ::= ";
            for (auto it3 = it2->begin(); it3 != it2->end(); it3++)
            {
                cout << *it3 << " ";
            }
            cout << endl;
        }
    }
}

set<term> Grammar::calcFirstOf(term t)
{
    debug(0) << "Calculating First(" << t << ")" << endl;
    if (first[t].size() > 0)
    {
        debug(1) << "First(" << t << ") has been calculated before" << endl;
        return first[t]; // 已经计算过
    }
    // 对于每个产生式，计算first集
    for (auto &right : rules[t])
    {
        // 空产生式，加入epsilon
        if (right.size() == 0)
        {
            first[t].insert(EPSILON);
            debug(1) << "First(" << t << ") <- {epsilon}" << endl;
        }
        else
        {
            // 对于产生式右部的每个符号
            bool allHaveEpsilon = true;
            for (auto &symbol : right)
            {
                if (_find(terminals, symbol))
                {
                    // 终结符，直接加入first集
                    first[t].insert(symbol);
                    debug(1) << "First(" << t << ") <- {" << symbol << "}" << endl;
                    allHaveEpsilon = false;
                    break; // 终结符后面的符号不再计算
                }
                else
                {
                    set<term> resFirst = calcFirstOf(symbol);
                    set<term> tmpFirst = resFirst;
                    tmpFirst.erase(EPSILON);
                    first[t].insert(tmpFirst.begin(), tmpFirst.end());
                    for (auto &f : tmpFirst)
                    {
                        debug(1) << "First(" << t << ") <- {" << f << "}" << endl;
                    }
                    if (!_find(resFirst, EPSILON))
                    {
                        // 该符号的first集不含epsilon，后面的符号不再计算
                        allHaveEpsilon = false;
                        break;
                    }
                }
            }
            if (allHaveEpsilon)
            {
                first[t].insert(EPSILON);
                debug(1) << "First(" << t << ") <- {epsilon}" << endl;
            }
        }
    }
    return first[t];
}
set<term> Grammar::calcFollowOf(term t)
{
    debug(0) << "Calculating Follow(" << t << ")" << endl;
    if (follow[t].size() > 0)
    {
        debug(1) << "Follow(" << t << ") has been calculated before" << endl;
        return follow[t]; // 已经计算过
    }
    if (t == startTerm)
    {
        follow[t].insert(SYM_END);
        debug(1) << "Follow(" << t << ") <- {#}" << endl;
    }
    for (auto &rule : rules)
    {
        for (auto &right : rule.second)
        {
            auto symIt = right.begin();
            symIt = find(symIt, right.end(), t);
            while (symIt != right.end())
            {
                if (symIt == right.end() - 1)
                {
                    // A -> aB
                    if (rule.first != t)
                    {
                        set<term> resFollow = calcFollowOf(rule.first);
                        follow[t].insert(resFollow.begin(), resFollow.end());
                        for (auto &f : resFollow)
                        {
                            debug(1) << "Follow(" << t << ") <-> {" << f << "} <A -> aB>" << endl;
                        }
                    }
                }
                else if (_find(terminals, *(symIt + 1)))
                {
                    // A -> aBb
                    follow[t].insert(*(symIt + 1));
                    debug(1) << "Follow(" << t << ") <- {" << *(symIt + 1) << "} <A -> aBb>" << endl;
                }
                else
                {
                    // A -> aBC
                    set<term> resFirst;
                    for (auto tmpIt = symIt + 1; tmpIt != right.end(); tmpIt++)
                    {
                        resFirst = calcFirstOf(*tmpIt);
                        set<term> tmpFirst = resFirst;
                        tmpFirst.erase(EPSILON);
                        follow[t].insert(tmpFirst.begin(), tmpFirst.end());
                        for (auto &f : tmpFirst)
                        {
                            debug(1) << "Follow(" << t << ") = {" << f << "} <A -> aBC>" << endl;
                        }
                        if (!_find(resFirst, EPSILON))
                        {
                            break;
                        }
                    }
                    if (resFirst.size() == 0 || _find(resFirst, EPSILON))
                    {
                        // A -> aBC, First(C) 含有epsilon
                        if (rule.first != t)
                        {
                            set<term> resFollow = calcFollowOf(rule.first);
                            follow[t].insert(resFollow.begin(), resFollow.end());
                            for (auto &f : resFollow)
                            {
                                debug(1) << "Follow(" << t << ") = {" << f << "} <A -> aBC(e)>" << endl;
                            }
                        }
                    }
                }
                symIt = find(symIt + 1, right.end(), t);
            }
        }
    }
    return follow[t];
}

void Grammar::calcFirst()
{
    info << "Calculating First..." << endl;
    for (auto &it : nonTerms)
    {
        calcFirstOf(it);
    }
}

void Grammar::calcFollow()
{
    info << "Calculating Follow..." << endl;
    for (auto &it : nonTerms)
    {
        calcFollowOf(it);
    }
}

void Grammar::printFirst()
{
    info << "First:" << endl;
    for (auto it = nonTerms.begin(); it != nonTerms.end(); it++)
    {
        cout << *it << " : ";
        for (auto it2 = first[*it].begin(); it2 != first[*it].end(); it2++)
        {
            cout << *it2 << " ";
        }
        cout << endl;
    }
}

void Grammar::printFollow()
{
    info << "Follow:" << endl;
    for (auto it = nonTerms.begin(); it != nonTerms.end(); it++)
    {
        cout << *it << " : ";
        for (auto it2 = follow[*it].begin(); it2 != follow[*it].end(); it2++)
        {
            cout << *it2 << " ";
        }
        cout << endl;
    }
}

void Grammar::printTerminals()
{
    info << "Terminals:" << endl;
    cout << "{ ";
    for (auto it = terminals.begin(); it != terminals.end(); it++)
    {
        cout << *it << ", ";
    }
    cout << "}" << endl;
}
void Grammar::printNonTerms()
{
    info << "Non-terminals:" << endl;
    cout << "{ ";
    for (auto it = nonTerms.begin(); it != nonTerms.end(); it++)
    {
        cout << *it << ", ";
    }
    cout << "}" << endl;
}
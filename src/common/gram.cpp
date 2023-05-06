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
#include "utils/log.h"

#define _find(x, y) (x.find(y) != x.end())

#define DEBUG_LEVEL 1

Grammar::Grammar(term start, set<term> terms, set<term> non_terms, map<term, set<vector<term>>> rules, map<token_type, term> tok2term)
{
    startTerm = start;
    terminals = terms;
    nonTerms = non_terms;
    this->rules = rules;
    this->tok2term = tok2term;
}

void Grammar::operator=(const Grammar &g)
{
    startTerm = g.startTerm;
    terminals = g.terminals;
    nonTerms = g.nonTerms;
    rules = g.rules;
}

void Grammar::eliminateLeftRecursion() // 消除左递归
{
    info << "Grammar: Eliminating Left Recursion" << endl;
    for (auto &A : nonTerms)
    {
        debug(0) << "Processing Non-Terminal " << A << endl;
        // 对于每个产生式A->α|β
        for (auto alpha : rules[A])
        {
            debug(1) << "Processing Production " << A << "->";
            for (auto &s : alpha)
            {
                debugU(1) << s << " ";
            }
            debug(1) << endl;
            if (alpha[0] == A) // 直接左递归
            {
                debug(1) << "Direct Left Recursion" << endl;
                // 新建一个非终结符A'，将A'->βA'加入产生式
                term newNonTerm = A + "'";
                nonTerms.insert(newNonTerm);
                rules[newNonTerm].insert(alpha);
                // 将A->α|β改为A->αA'|ε
                rules[A].erase(alpha);
                alpha.erase(alpha.begin());
                alpha.push_back(newNonTerm);
                rules[A].insert(alpha);
            }
            else if (_find(nonTerms, alpha[0])) // 间接左递归
            {
                debug(1) << "Indirect Left Recursion" << endl;
                // 新建一个非终结符A'，将A'->βA'加入产生式
                term newNonTerm = A + "'";
                nonTerms.insert(newNonTerm);
                rules[newNonTerm].insert(alpha);
                // 将A->α|β改为A->αA'
                rules[A].erase(alpha);
                alpha.push_back(newNonTerm);
                rules[A].insert(alpha);
            }
        }
    }
}

void Grammar::extractLeftCommonFactor()
{
    info << "Grammar: Extracting Left Common Factor" << endl;
    for (auto &A : nonTerms)
    {
        debug(0) << "Processing Non-Terminal " << A << endl;
        // 对于每个产生式A->α|β
        for (auto alpha : rules[A])
        {
            debug(1) << "Processing Production " << A << "->";
            for (auto &s : alpha)
            {
                debugU(1) << s << " ";
            }
            debug(1) << endl;
            // 对于每个产生式A->α|β
            for (auto beta : rules[A])
            {
                if (alpha == beta)
                    continue;
                debug(2) << "Processing Production " << A << "->";
                for (auto &s : beta)
                {
                    debugU(2) << s << " ";
                }
                debug(2) << endl;
                // 找到最长公共前缀
                int i = 0;
                while (i < alpha.size() && i < beta.size() && alpha[i] == beta[i])
                    i++;
                if (i == 0)
                    continue;
                debug(2) << "Longest Common Prefix: ";
                for (int j = 0; j < i; j++)
                {
                    debugU(2) << alpha[j] << " ";
                }
                debug(2) << endl;
                // 新建一个非终结符A'，将A'->βA'加入产生式
                term newNonTerm = A + "'";
                nonTerms.insert(newNonTerm);
                rules[newNonTerm].insert(beta);
                // 将A->α|β改为A->αA'
                rules[A].erase(beta);
                beta.erase(beta.begin(), beta.begin() + i);
                beta.insert(beta.begin(), newNonTerm);
                rules[A].insert(beta);
            }
        }
    }
}

bool Grammar::isLL1Grammar() // 判断是否为LL(1)文法
{
    info << "Grammar: Checking LL(1)" << endl;
    // 判断是否有冲突
    for (auto &A : nonTerms)
    {
        set<term> selectSet;
        for (auto &alpha : rules[A])
        {
            selectSet.insert(select[A][alpha].begin(), select[A][alpha].end());
        }
        if (selectSet.size() != rules[A].size())
        {
            error << "Grammar: Not LL(1)" << endl;
            return false;
        }
    }
    info << "Grammar: LL(1)" << endl;
    return true;
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

set<term> Grammar::calcSelectOf(term t, vector<term> rule)
{
    debug(0) << "Calculating Select(" << t << " -> ";
    for (auto &it : rule)
    {
        debug(0) << it << " ";
    }
    debug(0) << ")" << endl;
    set<term> resSelect;
    set<term> resFirst;
    resFirst = calcFirstOf(rule[0]);
    set<term> tmpFirst = resFirst;
    tmpFirst.erase(EPSILON);
    resSelect.insert(tmpFirst.begin(), tmpFirst.end());
    for (auto &f : tmpFirst)
    {
        debug(1) << "Select(" << t << " -> ";
        for (auto &it : rule)
        {
            debug(1) << it << " ";
        }
        debug(1) << ") <- {" << f << "}" << endl;
    }
    if (_find(resFirst, EPSILON))
    {
        // First(A) 含有epsilon
        set<term> resFollow = calcFollowOf(t);
        resSelect.insert(resFollow.begin(), resFollow.end());
        for (auto &f : resFollow)
        {
            debug(1) << "Select(" << t << " -> ";
            for (auto &it : rule)
            {
                debug(1) << it << " ";
            }
            debug(1) << ") <- {" << f << "}" << endl;
        }
    }
    return resSelect;
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

void Grammar::calcSelect()
{
    info << "Calculating Select..." << endl;
    for (auto &rule : rules)
    {
        for (auto &right : rule.second)
        {
            select[rule.first][right] = calcSelectOf(rule.first, right);
        }
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

void Grammar::printSelect()
{
    info << "Select:" << endl;
    for (auto it = rules.begin(); it != rules.end(); it++)
    {
        for (auto it2 = it->second.begin(); it2 != it->second.end(); it2++)
        {
            cout << it->first << " ::= ";
            for (auto it3 = it2->begin(); it3 != it2->end(); it3++)
            {
                cout << *it3 << " ";
            }
            cout << " : ";
            for (auto it3 = select[it->first][*it2].begin(); it3 != select[it->first][*it2].end(); it3++)
            {
                cout << *it3 << " ";
            }
            cout << endl;
        }
    }
}

void Grammar::printRules()
{
    info << "Grammar: Rules:" << endl;
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
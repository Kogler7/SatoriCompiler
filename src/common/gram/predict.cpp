/**
 * @file gram/basic.cpp
 * @author Zhenjie Wei (2024108@bjtu.edu.cn)
 * @brief Predictive PredictiveGrammar
 * @date 2023-05-12
 *
 * @copyright Copyright (c) 2023
 *
 */

#include "utils/stl.h"
#include "utils/log.h"
#include "utils/table.h"
#include "predict.h"

#define insert_if_not_exist(A, s)               \
    {                                           \
        flag = false;                           \
        tie(ignore, flag) = first[A].insert(s); \
        changed = changed || flag;              \
    }

bool PredictiveGrammar::isLL1Grammar() const // 判断是否为LL(1)文法
{
    info << "PredictiveGrammar: Checking LL(1)" << endl;
    // 假定First集、Follow集、Select集已经计算完毕
    for (auto &A : nonTerms)
    {
        debug(0) << "Processing Non-Terminal " << A << endl;
        // 对于每个产生式A->α
        for (auto &alpha : rules.at(A))
        {
            product_t pA(A, alpha);
            debug(1) << format("Processing Production $->$", A, vec2str(alpha)) << endl;
            // 计算Select集
            symset_t selectA = select.at(pA);
            debug(1) << format("Select($->$) = $", A, vec2str(alpha), set2str(selectA)) << endl;
            // 检查是否有空产生式
            bool aHasEpsilon = _find(firstS.at(alpha), EPSILON);
            // 检查是否有多个产生式的Select集相交
            for (auto &B : nonTerms)
            {
                if (A == B)
                    continue;
                for (auto &beta : rules.at(B))
                {
                    symset_t resSet;
                    product_t pB(B, beta);
                    bool bHasEpsilon = _find(firstS.at(beta), EPSILON);
                    if (aHasEpsilon && bHasEpsilon)
                    {
                        debug(1) << format(
                            "Select($->$) and Select($->$) both contain $.\n",
                            A, vec2str(alpha), B, vec2str(beta));
                        return false;
                    }
                    symset_t selectB = select.at(pB);
                    set_intersection(
                        selectA.begin(),
                        selectA.end(),
                        selectB.begin(),
                        selectB.end(),
                        inserter(resSet, resSet.begin()));
                    if (resSet.size() > 0)
                    {
                        debug(1) << format(
                            "Select($->$) and Select($->$) != null.\n",
                            A, vec2str(alpha), B, vec2str(beta));
                        return false;
                    }
                }
            }
        }
    }
    return true;
}

symset_t PredictiveGrammar::calcFirstOf(symbol_t t)
{
    debug(0) << "Calculating First(" << t << ")" << endl;
    if (hasLeftRecursion)
    {
        return first[t];
    }
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
            for (auto it = right.begin(); it != right.end(); it++)
            {
                if (_find(terminals, *it))
                {
                    // 终结符，直接加入first集
                    first[t].insert(*it);
                    debug(1) << "First(" << t << ") <- {" << *it << "}" << endl;
                    allHaveEpsilon = false;
                    break; // 终结符后面的符号不再计算
                }
                else
                {
                    if (it == right.begin() && *it == t)
                    {
                        // 直接左递归
                        warn << "Direct left recursion detected. Trying to calculate First with Left Recursion..." << endl;
                        hasLeftRecursion = true;
                        calcFirstWithLeftRecursion();
                        return first[t];
                    }
                    symset_t resFirst = calcFirstOf(*it);
                    symset_t tmpFirst = resFirst;
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

symset_t PredictiveGrammar::calcFirstOf(symstr_t s)
{
    debug(0) << format("Calculating First($)", vec2str(s)) << endl;
    if (firstS[s].size() > 0)
    {
        debug(1) << format(
            "First($) has been calculated before.\n",
            vec2str(s));
        return firstS[s]; // 已经计算过
    }
    symset_t resFirst;
    bool allHaveEpsilon = true;
    for (auto &symbol : s)
    {
        if (_find(terminals, symbol))
        {
            // 终结符，直接加入first集
            resFirst.insert(symbol);
            debug(1) << "First(" << vec2str(s) << ") <- {" << symbol << "}" << endl;
            allHaveEpsilon = false;
            break; // 终结符后面的符号不再计算
        }
        else
        {
            symset_t tmpFirst = calcFirstOf(symbol);
            resFirst.insert(tmpFirst.begin(), tmpFirst.end());
            debug(1) << "First(" << vec2str(s) << ") <- " << set2str(tmpFirst) << endl;
            if (!_find(tmpFirst, EPSILON))
            {
                // 该符号的first集不含epsilon，后面的符号不再计算
                allHaveEpsilon = false;
                break;
            }
        }
    }
    if (allHaveEpsilon)
    {
        resFirst.insert(EPSILON);
        debug(1) << "First(" << vec2str(s) << ") <- { epsilon }" << endl;
    }
    firstS[s] = resFirst;
    return resFirst;
}

symset_t PredictiveGrammar::calcFollowOf(symbol_t t)
{
    debug(0) << "Calculating Follow(" << t << ")" << endl;
    if (follow[t].size() > 0)
    {
        debug(1) << "Follow(" << t << ") has been calculated before" << endl;
        return follow[t]; // 已经计算过
    }
    if (t == symStart)
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
                        symset_t resFollow = calcFollowOf(rule.first);
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
                    symset_t resFirst;
                    for (auto tmpIt = symIt + 1; tmpIt != right.end(); tmpIt++)
                    {
                        resFirst = calcFirstOf(*tmpIt);
                        symset_t tmpFirst = resFirst;
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
                            symset_t resFollow = calcFollowOf(rule.first);
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

symset_t PredictiveGrammar::calcSelectOf(product_t p)
{
    debug(0) << "Calculating Select(" << p.first;
    debug_u(0) << " -> " << vec2str(p.second) << ")" << endl;
    symset_t resSelect;
    symset_t resFirst = firstS[p.second];
    symset_t tmpFirst = resFirst;
    tmpFirst.erase(EPSILON); // First(A) - {epsilon}
    resSelect.insert(tmpFirst.begin(), tmpFirst.end());
    debug(1) << "Select(" << p.first << " -> " << vec2str(p.second);
    debug_u(1) << ") <- " << set2str(tmpFirst) << endl;
    if (_find(resFirst, EPSILON))
    {
        symset_t resFollow = calcFollowOf(p.first);
        resSelect.insert(resFollow.begin(), resFollow.end());
        debug(1) << "Select(" << p.first << " -> " << vec2str(p.second);
        debug_u(1) << ") <- " << set2str(resFollow) << endl;
    }
    return resSelect;
}

void PredictiveGrammar::calcFirst()
{
    info << "Calculating First(S)..." << endl;
    for (auto &p : products)
    {
        calcFirstOf(p.first);
        calcFirstOf(p.second);
    }
}

void PredictiveGrammar::calcFollow()
{
    info << "Calculating Follow..." << endl;
    for (auto &it : nonTerms)
    {
        calcFollowOf(it);
    }
}

void PredictiveGrammar::calcSelect()
{
    info << "Calculating Select..." << endl;
    for (auto pdt : products)
    {
        select[pdt] = calcSelectOf(pdt);
    }
}

void PredictiveGrammar::calcFirstWithLeftRecursion()
{
    info << "Calculating First with Left Recursion..." << endl;
    // 计算终结符的First集
    for (auto &t : terminals)
    {
        first[t].insert(t);
    }
    // 计算非终结符的First集
    bool flag, changed = true;
    while (changed)
    {
        changed = false;
        for (auto &A : nonTerms)
        {
            for (auto &right : rules[A])
            {
                if (right.size() == 0)
                {
                    insert_if_not_exist(A, EPSILON);
                    continue;
                }
                bool allHaveEpsilon = true;
                for (auto it = right.begin(); it != right.end(); it++)
                {
                    if (_find(terminals, *it))
                    {
                        insert_if_not_exist(A, *it);
                        allHaveEpsilon = false;
                        break;
                    }
                    else
                    {
                        symset_t resFirst = first[*it];
                        symset_t tmpFirst = resFirst;
                        tmpFirst.erase(EPSILON);
                        for (auto &f : tmpFirst)
                        {
                            insert_if_not_exist(A, f);
                        }
                        if (!_find(resFirst, EPSILON))
                        {
                            allHaveEpsilon = false;
                            break;
                        }
                    }
                }
                if (allHaveEpsilon)
                {
                    insert_if_not_exist(A, EPSILON);
                }
            }
        }
    }
    // 计算符号串的First集
    for (auto &p : products)
    {
        calcFirstOf(p.second);
    }
}

void PredictiveGrammar::printFirst() const
{
    info << "First:" << endl;
    tb_head | "NonTerm" | "First" = table::AL_LFT;
    for (auto it = nonTerms.begin(); it != nonTerms.end(); it++)
    {
        if (first.find(*it) == first.end())
        {
            warn << "First set of " << *it << " not found." << endl;
            continue;
        }
        new_row | *it | set2str(first.at(*it));
    }
    cout << tb_view();
}

void PredictiveGrammar::printFollow() const
{
    info << "Follow:" << endl;
    tb_head | "NonTerm" | "Follow" = table::AL_LFT;
    for (auto it = nonTerms.begin(); it != nonTerms.end(); it++)
    {
        new_row | *it | set2str(follow.at(*it));
    }
    cout << tb_view();
}

void PredictiveGrammar::printFirstS() const
{
    info << "First of Symstr: " << endl;
    tb_head | "Symstr" | "First" = table::AL_LFT;
    for (auto it = products.begin(); it != products.end(); it++)
    {
        const symstr_t &s = it->second;
        if (firstS.find(s) == firstS.end())
        {
            warn << "First set of " << vec2str(s) << " not found." << endl;
            continue;
        }
        new_row | vec2str(s) | set2str(firstS.at(s));
    }
    cout << tb_view();
}

void PredictiveGrammar::printSelect() const
{
    info << "Select: " << endl;
    tb_head | "Product" | "Select" = table::AL_LFT;
    for (auto it = products.begin(); it != products.end(); it++)
    {
        new_row | it->first + "->" + vec2str(it->second) | set2str(select.at(*it));
    }
    cout << tb_view();
}
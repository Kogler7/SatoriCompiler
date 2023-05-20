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

bool PredictiveGrammar::isLL1Grammar() // 判断是否为LL(1)文法
{
    info << "PredictiveGrammar: Checking LL(1)" << endl;
    // 假定First集、Follow集、Select集已经计算完毕
    for (auto &A : nonTerms)
    {
        debug(0) << "Processing Non-Terminal " << A << endl;
        // 对于每个产生式A->α
        for (auto &alpha : rules[A])
        {
            product_t pA(A, alpha);
            debug(1) << format("Processing Production $->$", A, vec2str(alpha)) << endl;
            // 计算Select集
            symset_t selectA = select[pA];
            debug(1) << format("Select($->$) = $", A, vec2str(alpha), set2str(selectA)) << endl;
            // 检查是否有空产生式
            bool aHasEpsilon = _find(firstS[alpha], EPSILON);
            // 检查是否有多个产生式的Select集相交
            for (auto &B : nonTerms)
            {
                if (A == B)
                    continue;
                for (auto &beta : rules[B])
                {
                    symset_t resSet;
                    product_t pB(B, beta);
                    bool bHasEpsilon = _find(firstS[beta], EPSILON);
                    if (aHasEpsilon && bHasEpsilon)
                    {
                        debug(1) << format(
                            "Select($->$) and Select($->$) both contain $.\n",
                            A, vec2str(alpha), B, vec2str(beta));
                        return false;
                    }
                    symset_t selectB = select[pB];
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
                    symset_t resFirst = calcFirstOf(symbol);
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

void PredictiveGrammar::printFirst()
{
    info << "First:" << endl;
    tb_head | "NonTerm" | "First" = table::AL_LFT;
    for (auto it = nonTerms.begin(); it != nonTerms.end(); it++)
    {
        new_row | *it | set2str(first[*it]);
    }
    cout << tb_view();
}

void PredictiveGrammar::printFollow()
{
    info << "Follow:" << endl;
    tb_head | "NonTerm" | "Follow" = table::AL_LFT;
    for (auto it = nonTerms.begin(); it != nonTerms.end(); it++)
    {
        new_row | *it | set2str(follow[*it]);
    }
    cout << tb_view();
}

void PredictiveGrammar::printFirstS()
{
    info << "First of Symstr: " << endl;
    tb_head | "Symstr" | "First" = table::AL_LFT;
    for (auto it = products.begin(); it != products.end(); it++)
    {
        symstr_t &s = it->second;
        new_row | vec2str(s) | set2str(firstS[s]);
    }
    cout << tb_view();
}

void PredictiveGrammar::printSelect()
{
    info << "Select: " << endl;
    tb_head | "Product" | "Select" = table::AL_LFT;
    for (auto it = products.begin(); it != products.end(); it++)
    {
        new_row | it->first + "->" + vec2str(it->second) | set2str(select[*it]);
    }
    cout << tb_view();
}
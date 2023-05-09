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

Grammar::Grammar(term start, set<term> terms, set<term> non_terms, vector<production> products, map<term, set<vector<term>>> rules, map<token_type, term> tok2term)
{
    startTerm = start;
    terminals = terms;
    nonTerms = non_terms;
    this->products = products;
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
                debug_u(1) << s << " ";
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
    // 将规则按照左部分组，分别构造TermTree
    map<term, tt_node_ptr> termTrees;
    for (auto &A : nonTerms)
    {
        termTrees[A] = tt_node::createNode(A);
        for (auto alphaIt = rules[A].begin(); alphaIt != rules[A].end(); alphaIt++)
        {
            auto &alpha = *alphaIt;
            tt_node_ptr cur = termTrees[A];
            for (size_t i = 0; i < alpha.size(); i++)
            {
                auto &sym = alpha[i];
                size_t index = cur->find(sym);
                if (index == -1)
                {
                    tt_node_ptr new_node = tt_node::createNode(sym, alphaIt, i);
                    (*cur) << new_node;
                    cur = new_node;
                }
                else
                {
                    cur = cur->get_child_ptr(index);
                }
            }
            (*cur) << tt_node::createNode("#", alphaIt, 0);
        }
        termTrees[A]->print();
    }
    // 递归遍历TermTree，修正规则集合
    info << "Grammar: Fixing Rules" << endl;
    size_t cnt = 0;
    for (auto &tree : termTrees)
    {
        const term &left = tree.first;
        tt_node_ptr &root = tree.second;
        root->postorder(
            [&](tt_node node)
            {
                if (node.size() > 1 && node.data.index != 0)
                {
                    // 有公共前缀
                    info << "Grammar: Fixing Rule " << left << "->" << node.data.symbol;
                    cout << " " << node.data.index << endl;
                    assert(node.data.ruleIt != rules[left].end());
                    vector<term> curRule = *(node.data.ruleIt);
                    vector<term> prefix;
                    prefix.insert(prefix.end(), curRule.begin(), curRule.begin() + node.data.index + 1);
                    term newNonTerm = left + "^" + to_string(++cnt);
                    nonTerms.insert(newNonTerm);
                    prefix.push_back(newNonTerm);
                    rules[left].insert(prefix);
                    node.foreach (
                        [&](tt_node child)
                        {
                            // 对于每个子节点，将其所代表的子产生式加入新的规则
                            vector<term> suffix;
                            if (child.data.symbol != "#")
                            {
                                suffix.insert(
                                    suffix.end(),
                                    child.data.ruleIt->begin() + child.data.index,
                                    child.data.ruleIt->end());
                            }
                            rules[newNonTerm].insert(suffix);
                            rules[left].erase(child.data.ruleIt);
                        });
                }
            });
    }
}

bool Grammar::isLL1Grammar() // 判断是否为LL(1)文法
{
    info << "Grammar: Checking LL(1)" << endl;
    // 假定First集、Follow集、Select集已经计算完毕
    for (auto &A : nonTerms)
    {
        debug(0) << "Processing Non-Terminal " << A << endl;
        // 对于每个产生式A->α
        for (auto &alpha : rules[A])
        {
            production pA(A, alpha);
            debug(1) << format("Processing Production $->$", A, vec2str(alpha)) << endl;
            // 计算Select集
            set<term> selectA = select[pA];
            debug(1) << format("Select($->$) = $", A, vec2str(alpha), set2str(selectA)) << endl;
            // 检查是否有空产生式
            bool aHasEpsilon = _find(firstP[pA], EPSILON);
            // 检查是否有多个产生式的Select集相交
            for (auto &B : nonTerms)
            {
                if (A == B)
                    continue;
                for (auto &beta : rules[B])
                {
                    set<term> resSet;
                    production pB(B, beta);
                    bool bHasEpsilon = _find(firstP[pB], EPSILON);
                    if (aHasEpsilon && bHasEpsilon)
                    {
                        debug(1) << format(
                            "Select($->$) and Select($->$) both contain $.\n",
                            A, vec2str(alpha), B, vec2str(beta));
                        return false;
                    }
                    set<term> selectB = select[pB];
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

set<term> Grammar::calcFirstOf(production product)
{
    debug(0) << format("Calculating First($->$)", product.first, vec2str(product.second)) << endl;
    if (firstP[product].size() > 0)
    {
        debug(1) << format(
            "First($->$) has been calculated before.\n",
            product.first,
            vec2str(product.second));
        return firstP[product]; // 已经计算过
    }
    set<term> resFirst;
    bool allHaveEpsilon = true;
    for (auto &symbol : product.second)
    {
        if (_find(terminals, symbol))
        {
            // 终结符，直接加入first集
            resFirst.insert(symbol);
            debug(1) << "First(" << product.first << ") <- {" << symbol << "}" << endl;
            allHaveEpsilon = false;
            break; // 终结符后面的符号不再计算
        }
        else
        {
            set<term> tmpFirst = calcFirstOf(symbol);
            resFirst.insert(tmpFirst.begin(), tmpFirst.end());
            debug(1) << "First(" << product.first << ") <- " << container2str(tmpFirst) << endl;
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
        debug(1) << "First(" << product.first << ") <- { epsilon }" << endl;
    }
    firstP[product] = resFirst;
    return resFirst;
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

set<term> Grammar::calcSelectOf(production product)
{
    debug(0) << "Calculating Select(" << product.first;
    debug_u(0) << " -> " << vec2str(product.second) << ")" << endl;
    set<term> resSelect;
    set<term> resFirst = calcFirstOf(product);
    set<term> tmpFirst = resFirst;
    tmpFirst.erase(EPSILON); // First(A) - {epsilon}
    resSelect.insert(tmpFirst.begin(), tmpFirst.end());
    debug(1) << "Select(" << product.first << " -> " << vec2str(product.second);
    debug_u(1) << ") <- " << set2str(tmpFirst) << endl;
    if (_find(resFirst, EPSILON))
    {
        set<term> resFollow = calcFollowOf(product.first);
        resSelect.insert(resFollow.begin(), resFollow.end());
        debug(1) << "Select(" << product.first << " -> " << vec2str(product.second);
        debug_u(1) << ") <- " << set2str(resFollow) << endl;
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
    for (auto pdt : products)
    {
        select[pdt] = calcSelectOf(pdt);
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

void Grammar::printFirstP()
{
    info << "First of Product: " << endl;
    for (auto it = products.begin(); it != products.end(); it++)
    {
        cout << format(
            "$ -> $ : $\n",
            it->first,
            vec2str(it->second),
            set2str(firstP[*it])
        );
    }
}

void Grammar::printSelect()
{
    info << "Select: " << endl;
    for (auto it = products.begin(); it != products.end(); it++)
    {
        cout << format(
            "$ -> $ : $\n",
            it->first,
            vec2str(it->second),
            set2str(select[*it])
        );
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
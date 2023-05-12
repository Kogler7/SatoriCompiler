/**
 * @file gram/basic.cpp
 * @author Zhenjie Wei (2024108@bjtu.edu.cn)
 * @brief Basic Grammar
 * @date 2023-04-29
 *
 * @copyright Copyright (c) 2023
 *
 */

#include "basic.h"
#include "utils/stl.h"
#include "utils/log.h"
#include "utils/table.h"

typedef string symbol;
typedef set<symbol> symset;
typedef vector<symbol> symstr;
typedef pair<symbol, symstr> product;

#define DEBUG_LEVEL 1

Grammar::Grammar(symbol start, symset terms, symset nonTerms, vector<product> products, map<symbol, set<symstr>> rules, map<token_type, symbol> tok2sym)
{
    symStart = start;
    terminals = terms;
    terminals.insert(SYM_END); // SYM_END 用于表示输入串结束
    nonTerms = nonTerms;
    this->products = products;
    this->rules = rules;
    this->tok2sym = tok2sym;
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
                symbol newNonTerm = A + "'";
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
                symbol newNonTerm = A + "'";
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
    map<symbol, tt_node_ptr> termTrees;
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
        const symbol &left = tree.first;
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
                    symstr curRule = *(node.data.ruleIt);
                    symstr prefix;
                    prefix.insert(prefix.end(), curRule.begin(), curRule.begin() + node.data.index + 1);
                    symbol newNonTerm = left + "^" + to_string(++cnt);
                    nonTerms.insert(newNonTerm);
                    prefix.push_back(newNonTerm);
                    rules[left].insert(prefix);
                    node.foreach (
                        [&](tt_node child)
                        {
                            // 对于每个子节点，将其所代表的子产生式加入新的规则
                            symstr suffix;
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
    cout << set2str(terminals) << endl;
}

void Grammar::printNonTerms()
{
    info << "Non-terminals:" << endl;
    cout << set2str(nonTerms) << endl;
}

vector<token> Grammar::transferTokens(vector<token> tokens)
{
    info << "Transferring tokens..." << endl;
    vector<token> res;
    for (auto &t : tokens)
    {
        if (_find(terminals, t.value))
        {
            debug(0) << "terminals: " << t.value << endl;
            res.push_back(token(
                make_shared<symbol>(t.value),
                t.value,
                t.line,
                t.col));
        }
        else if (_find(tok2sym, t.type))
        {
            debug(0) << "tok2sym: " << t.value << endl;
            res.push_back(
                token(
                    make_shared<symbol>(tok2sym[t.type]),
                    t.value,
                    t.line,
                    t.col));
        }
        else
        {
            res.push_back(t);
            warn << "Grammar::transferTokens: Unknown token: " << t.value << endl;
        }
    }
    return res;
}
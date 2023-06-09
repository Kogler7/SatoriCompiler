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

typedef string symbol_t;
typedef set<symbol_t> symset_t;
typedef vector<symbol_t> symstr_t;
typedef pair<symbol_t, symstr_t> product_t;

#define DEBUG_LEVEL 1

Grammar::Grammar(symbol_t start, symset_t terms, symset_t nonTerms, vector<product_t> products, map<symbol_t, set<symstr_t>> rules, map<token_type_t, symbol_t> tok2sym)
{
    symStart = start;
    terminals = terms;
    terminals.insert(SYM_END); // SYM_END 用于表示输入串结束
    nonTerms = nonTerms;
    this->products = products;
    this->rules = rules;
    this->tok2sym = tok2sym;
}

void Grammar::updateStartProduct()
{
    startProduct = make_pair(symStart, *rules.at(symStart).begin());
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
                symbol_t newNonTerm = A + "'";
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
                symbol_t newNonTerm = A + "'";
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
    map<symbol_t, tt_node_ptr_t> termTrees;
    for (auto &A : nonTerms)
    {
        termTrees[A] = tt_node_t::createNode(A);
        for (auto alphaIt = rules[A].begin(); alphaIt != rules[A].end(); alphaIt++)
        {
            auto &alpha = *alphaIt;
            tt_node_ptr_t cur = termTrees[A];
            for (size_t i = 0; i < alpha.size(); i++)
            {
                auto &sym = alpha[i];
                size_t index = cur->find(sym);
                if (index == -1)
                {
                    tt_node_ptr_t new_node = tt_node_t::createNode(sym, alphaIt, i);
                    (*cur) << new_node;
                    cur = new_node;
                }
                else
                {
                    cur = cur->get_child_ptr(index);
                }
            }
            (*cur) << tt_node_t::createNode("#", alphaIt, 0);
        }
        termTrees[A]->print();
    }
    // 递归遍历TermTree，修正规则集合
    info << "Grammar: Fixing Rules" << endl;
    size_t cnt = 0;
    for (auto &tree : termTrees)
    {
        const symbol_t &left = tree.first;
        tt_node_ptr_t &root = tree.second;
        root->postorder(
            [&](tt_node_t node)
            {
                if (node.size() > 1 && node.data.index != 0)
                {
                    // 有公共前缀
                    info << "Grammar: Fixing Rule " << left << "->" << node.data.symbol;
                    cout << " " << node.data.index << endl;
                    assert(node.data.ruleIt != rules[left].end());
                    symstr_t curRule = *(node.data.ruleIt);
                    symstr_t prefix;
                    prefix.insert(prefix.end(), curRule.begin(), curRule.begin() + node.data.index + 1);
                    symbol_t newNonTerm = left + "^" + to_string(++cnt);
                    nonTerms.insert(newNonTerm);
                    prefix.push_back(newNonTerm);
                    rules[left].insert(prefix);
                    node.foreach (
                        [&](tt_node_t child)
                        {
                            // 对于每个子节点，将其所代表的子产生式加入新的规则
                            symstr_t suffix;
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

void Grammar::printRules() const
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

void Grammar::printTerminals() const
{
    info << "Terminals:" << endl;
    cout << set2str(terminals) << endl;
}

void Grammar::printNonTerms() const
{
    info << "Non-terminals:" << endl;
    cout << set2str(nonTerms) << endl;
}

vector<token> Grammar::transferTokens(const vector<token> &tokens) const
{
    info << "Transferring tokens..." << endl;
    vector<token> res;
    for (auto &t : tokens)
    {
        if (_find(terminals, t.value))
        {
            debug(0) << "terminals: " << t.value << endl;
            res.push_back(token(
                make_shared<symbol_t>(t.value),
                t.value,
                t.line,
                t.col));
        }
        else if (_find(tok2sym, t.type))
        {
            debug(0) << "tok2sym: " << t.value << endl;
            res.push_back(
                token(
                    make_shared<symbol_t>(tok2sym.at(t.type)),
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
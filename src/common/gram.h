/**
 * @file gram.h
 * @author Zhenjie Wei (2024108@bjtu.edu.cn)
 * @brief Grammar
 * @date 2023-04-29
 *
 * @copyright Copyright (c) 2023
 *
 */

#pragma once

#include <map>
#include <set>
#include <vector>
#include <string>

#include "common/token.h"
#include "utils/tree.h"
#include "algorithm"

#define EPSILON "@" // 用于表示空串
#define SYM_END "#" // 用于表示输入串结束

using namespace std;

typedef string symbol;
typedef set<symbol> symset;
typedef vector<symbol> symstr;
typedef pair<symbol, symstr> product;

class Grammar
{
    symset calcFirstOf(symbol t);
    symset calcFirstOf(symstr s);
    symset calcFollowOf(symbol t);
    symset calcSelectOf(product p);
    void calcFirst();
    void calcFollow();
    void calcSelect();

public:
    symbol symStart;
    symset terminals;
    symset nonTerms;
    map<token_type, symbol> tok2sym;
    vector<product> products;
    map<symbol, set<symstr>> rules;
    map<symbol, symset> first;
    map<symstr, symset> firstS;
    map<symbol, symset> follow;
    map<product, symset> select;

    Grammar(symbol start, symset terms, symset nonTerms, vector<product> products, map<symbol, set<symstr>> rules, map<token_type, symbol> tok2sym);
    Grammar() { terminals.insert(SYM_END); }
    Grammar(const Grammar &g)
    {
        symStart = g.symStart;
        terminals = g.terminals;
        nonTerms = g.nonTerms;
        tok2sym = g.tok2sym;
        products = g.products;
        rules = g.rules;
        calcFirst();
        calcFollow();
        calcSelect();
    }
    void eliminateLeftRecursion();
    void extractLeftCommonFactor();
    bool isLL1Grammar();
    void printRules();
    void printTerminals();
    void printNonTerms();
    void printFirst();
    void printFollow();
    void printFirstS();
    void printSelect();
    vector<token> transferTokens(vector<token> tokens);
};

class TermTreeNode;
typedef TermTreeNode tt_node;
typedef shared_ptr<TermTreeNode> tt_node_ptr;
typedef vector<tt_node_ptr> tt_childs;

struct tt_node_data
{
    symbol symbol;
    set<symstr>::iterator ruleIt;
    size_t index;
};

class TermTreeNode : public AbstractTreeNode<tt_node_data>
{
public:
    TermTreeNode(tt_node_data t) : AbstractTreeNode<tt_node_data>(t) {}

    static tt_node_ptr createNode(symbol symbol, set<symstr>::iterator ruleIt, size_t index)
    {
        return make_shared<TermTreeNode>(tt_node_data(symbol, ruleIt, index));
    }

    static tt_node_ptr createNode(symbol symbol)
    {
        return make_shared<TermTreeNode>(tt_node_data(symbol, set<symstr>::iterator(), 0));
    }

    size_t find(symbol symbol) const
    {
        auto cmp = [=](tree_node_ptr<tt_node_data> ptr)
        {
            tt_node_ptr node = static_pointer_cast<TermTreeNode>(ptr);
            return node->data.symbol == symbol;
        };
        return AbstractTreeNode<tt_node_data>::find(cmp);
    }

    TermTreeNode &operator<<(const tt_node_ptr node)
    {
        return static_cast<TermTreeNode &>(AbstractTreeNode<tt_node_data>::operator<<(
            static_pointer_cast<AbstractTreeNode<tt_node_data>>(node)));
    }

    TermTreeNode &operator[](size_t index)
    {
        return static_cast<TermTreeNode &>(AbstractTreeNode<tt_node_data>::operator[](index));
    }

    tt_node_ptr get_child_ptr(size_t index) const
    {
        return static_pointer_cast<TermTreeNode>(AbstractTreeNode<tt_node_data>::get_child_ptr(index));
    }

    template <typename func_t>
    void foreach (func_t f) const
    {
        auto nodeF = [=](tree_childs<tt_node_data>::const_reference ref)
        {
            f(static_cast<TermTreeNode &>(*(ref)));
        };
        for_each(this->begin(), this->end(), nodeF);
    }

    template <typename func_t>
    void postorder(func_t f) const
    {
        foreach (
            [=](tt_node &ref)
            { ref.postorder(f); })
            ;
        f(*this);
    }

    string desc() const override
    {
        stringstream ss;
        ss << data.symbol << " " << data.index;
        return ss.str();
    }
};

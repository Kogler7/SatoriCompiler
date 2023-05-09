/**
 * @file gram.h
 * @author Zhenjie Wei (2024108@bjtu.edu.cn)
 * @brief Grammar
 * @date 2023-04-29
 *
 * @copyright Copyright (c) 2023
 *
 */

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

typedef string term;
typedef pair<term, vector<term>> production;

class Grammar
{
public:
    term startTerm;
    set<term> terminals;
    set<term> nonTerms;
    map<token_type, term> tok2term;
    vector<production> products;
    map<term, set<vector<term>>> rules;
    map<term, set<term>> first;
    map<term, set<term>> follow;
    map<production, set<term>> firstP;
    map<production, set<term>> select;

    Grammar(term start, set<term> terms, set<term> non_terms, vector<production> products, map<term, set<vector<term>>> rules, map<token_type, term> tok2term);
    Grammar() {}
    void operator=(const Grammar &g);
    void eliminateLeftRecursion();
    void extractLeftCommonFactor();
    bool isLL1Grammar();
    set<term> calcFirstOf(term t);
    set<term> calcFollowOf(term t);
    set<term> calcFirstOf(production product);
    set<term> calcSelectOf(production product);
    void calcFirst();
    void calcFollow();
    void calcSelect();
    void printRules();
    void printTerminals();
    void printNonTerms();
    void printFirst();
    void printFollow();
    void printFirstP();
    void printSelect();
};

class TermTreeNode;
typedef TermTreeNode tt_node;
typedef shared_ptr<TermTreeNode> tt_node_ptr;
typedef vector<tt_node_ptr> tt_childs;

struct tt_node_data
{
    term symbol;
    set<vector<term>>::iterator ruleIt;
    size_t index;
};

class TermTreeNode : public AbstractTreeNode<tt_node_data>
{
public:
    TermTreeNode(tt_node_data t) : AbstractTreeNode<tt_node_data>(t) {}

    static tt_node_ptr createNode(term symbol, set<vector<term>>::iterator ruleIt, size_t index)
    {
        return make_shared<TermTreeNode>(tt_node_data(symbol, ruleIt, index));
    }

    static tt_node_ptr createNode(term symbol)
    {
        return make_shared<TermTreeNode>(tt_node_data(symbol, set<vector<term>>::iterator(), 0));
    }

    size_t find(term symbol) const
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

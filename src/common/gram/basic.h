/**
 * @file gram/basic.h
 * @author Zhenjie Wei (2024108@bjtu.edu.cn)
 * @brief Basic Grammar
 * @date 2023-04-29
 *
 * @copyright Copyright (c) 2023
 *
 */

#pragma once

#include "common/token.h"
#include "common/tree/tree.h"
#include "algorithm"

#include <map>
#include <set>
#include <vector>
#include <string>

#define EPSILON "$" // 用于表示空串
#define SYM_END "#" // 用于表示输入串结束

using symbol_t = std::string;
using symset_t = std::set<symbol_t>;
using symstr_t = std::vector<symbol_t>;
using product_t = std::pair<symbol_t, symstr_t>;
using product_ref = std::reference_wrapper<product_t>;
using reduced_product_t = product_t;
using semantic_t = std::string;

enum assoc_t
{
    ASSOC_LEFT,
    ASSOC_RIGHT,
    ASSOC_NONE
};

using prec_assoc_t = std::pair<size_t, assoc_t>;

template <typename row_t, typename col_t, typename val_t>
using table_t = std::map<std::pair<row_t, col_t>, val_t>;

template <typename row_t, typename col_t>
using coord_t = std::pair<row_t, col_t>;

template <typename row_t, typename col_t>
constexpr coord_t<row_t, col_t> mkcrd(const row_t &a, const col_t &b)
{
    return coord_t<row_t, col_t>(a, b);
}

class Grammar
{

public:
    symbol_t symStart;
    symset_t mulTerms;
    symset_t nonTerms;
    symset_t terminals;

    product_t startProduct;
    std::vector<product_t> products;
    std::map<symbol_t, std::set<symstr_t>> rules;

    std::map<token_type_t, symbol_t> tok2sym;
    std::map<product_t, semantic_t> semMap;
    std::map<symbol_t, prec_assoc_t> precMap;

    Grammar() { terminals.insert(SYM_END); }
    Grammar(const Grammar &g)
    {
        symStart = g.symStart;
        mulTerms = g.mulTerms;
        nonTerms = g.nonTerms;
        terminals = g.terminals;
        startProduct = g.startProduct;
        products = g.products;
        rules = g.rules;
        tok2sym = g.tok2sym;
        semMap = g.semMap;
        precMap = g.precMap;
    }
    reduced_product_t reduceProduct(const product_t &p) const;
    void updateStartProduct();
    void eliminateLeftRecursion();
    void extractLeftCommonFactor();
    void printRules() const;
    void printTerminals() const;
    void printNonTerms() const;
    void printSemanticMarks() const;
    std::vector<token> transferTokens(const std::vector<token> &tokens) const;
};

class TermTreeNode;

using tt_node_t = TermTreeNode;
using tt_node_ptr_t = std::shared_ptr<TermTreeNode>;
using tt_children_t = std::vector<tt_node_ptr_t>;

struct tt_node_data
{
    symbol_t symbol;
    std::set<symstr_t>::iterator ruleIt;
    size_t index;
};

class TermTreeNode : public AbstractTreeNode<tt_node_data>
{
public:
    TermTreeNode(tt_node_data t) : AbstractTreeNode<tt_node_data>(t) {}

    static tt_node_ptr_t createNode(symbol_t symbol, std::set<symstr_t>::iterator ruleIt, size_t index)
    {
        return std::make_shared<TermTreeNode>(tt_node_data(symbol, ruleIt, index));
    }

    static tt_node_ptr_t createNode(symbol_t symbol)
    {
        return std::make_shared<TermTreeNode>(tt_node_data(symbol, std::set<symstr_t>::iterator(), 0));
    }

    size_t find(symbol_t symbol) const
    {
        auto cmp = [=](tree_node_ptr_t<tt_node_data> ptr)
        {
            tt_node_ptr_t node = static_pointer_cast<TermTreeNode>(ptr);
            return node->data.symbol == symbol;
        };
        return AbstractTreeNode<tt_node_data>::find(cmp);
    }

    TermTreeNode &operator<<(const tt_node_ptr_t node)
    {
        return static_cast<TermTreeNode &>(AbstractTreeNode<tt_node_data>::operator<<(
            static_pointer_cast<AbstractTreeNode<tt_node_data>>(node)));
    }

    TermTreeNode &operator[](size_t index) const
    {
        return static_cast<TermTreeNode &>(AbstractTreeNode<tt_node_data>::operator[](index));
    }

    tt_node_ptr_t get_child_ptr(size_t index) const
    {
        return static_pointer_cast<TermTreeNode>(AbstractTreeNode<tt_node_data>::get_child_ptr(index));
    }

    template <typename func_t>
    void foreach (func_t f) const
    {
        auto nodeF = [=](tree_children_t<tt_node_data>::const_reference ref)
        {
            f(static_cast<TermTreeNode &>(*(ref)));
        };
        for_each(this->begin(), this->end(), nodeF);
    }

    template <typename func_t>
    void postorder(func_t f) const
    {
        foreach (
            [=](tt_node_t &ref)
            { ref.postorder(f); })
            ;
        f(*this);
    }

    std::string descData() const override
    {
        std::stringstream ss;
        ss << data.symbol << " " << data.index;
        return ss.str();
    }
};

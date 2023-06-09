/**
 * @file tree/pst.h
 * @author Zhenjie Wei (2024108@bjtu.edu.cn)
 * @brief Parse Syntax Tree
 * @date 2023-05-01
 *
 * @copyright Copyright (c) 2023
 *
 */

#pragma once

#include <string>
#include <optional>

#include "tree.h"
#include "utils/log.h"
#include "common/gram/basic.h"

using namespace std;

class ParseSyntaxTreeNode;

typedef ParseSyntaxTreeNode pst_node_t;
typedef ParseSyntaxTreeNode pst_tree_t;
typedef shared_ptr<pst_node_t> pst_node_ptr_t;
typedef shared_ptr<pst_node_t> pst_tree_ptr_t;
typedef vector<pst_node_ptr_t> pst_children_t;

enum node_type
{
    NON_TERM,
    TERMINAL
};

struct pst_node_data
{
    node_type type;
    string symbol;
    size_t line, col;
    optional<product_ref> product_opt;
};

extern const product_ref empty_product;

class ParseSyntaxTreeNode : public AbstractTreeNode<pst_node_data>
{
public:
    ParseSyntaxTreeNode(pst_node_data data) : AbstractTreeNode<pst_node_data>(data) {}
    ParseSyntaxTreeNode(node_type typ, string sym, size_t ln, size_t co);

    static pst_node_ptr_t createNode(node_type type, const string &symbol, size_t line, size_t col);
    static pst_node_ptr_t createNode(pst_node_data data);

    pst_node_t &attachProduct(product_t &product);

    pst_node_t &operator[](size_t index) const;
    pst_node_t &operator<<(const pst_node_ptr_t node);

    template <typename func_t>
    void foreach (func_t f) const
    {
        auto nodeF = [=](tree_children_t<pst_node_data>::const_reference ref)
        {
            f(static_cast<pst_node_t &>(*(ref)));
        };
        for_each(this->begin(), this->end(), nodeF);
    }

    template <typename func_t>
    void postorder(func_t f) const
    {
        foreach (
            [=](pst_node_t &ref)
            { ref.postorder(f); })
            ;
        f(*this);
    }

    string descData() const override;
};
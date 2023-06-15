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

#include "tree.h"
#include "utils/log.h"
#include "common/gram/basic.h"

#include <string>
#include <optional>

class ParseSyntaxTreeNode;

using pst_node_t = ParseSyntaxTreeNode;
using pst_tree_t = ParseSyntaxTreeNode;

using pst_node_ptr_t = std::shared_ptr<pst_node_t>;
using pst_tree_ptr_t = std::shared_ptr<pst_tree_t>;
using pst_children_t = std::vector<pst_node_ptr_t>;

enum node_type
{
    NON_TERM,
    TERMINAL
};

struct pst_node_data
{
    node_type type;
    std::string symbol;
    size_t line, col;
    std::optional<product_t> product_opt;
    std::optional<semantic_t> semantic_opt;
};

class ParseSyntaxTreeNode : public AbstractTreeNode<pst_node_data>
{
public:
    ParseSyntaxTreeNode(pst_node_data data) : AbstractTreeNode<pst_node_data>(data) {}
    ParseSyntaxTreeNode(node_type typ, std::string sym, size_t ln, size_t co);

    static pst_node_ptr_t createNode(node_type type, const std::string &symbol, size_t line, size_t col);
    static pst_node_ptr_t createNode(pst_node_data data);

    pst_node_t &attachProduct(product_t &product);
    pst_node_t &attachSemantic(semantic_t &semantic);

    pst_node_t &operator[](size_t index) const;
    pst_node_t &operator<<(const pst_node_ptr_t node);

    pst_node_ptr_t getChildAt(size_t index) const
    {
        auto child = this->at(index);
        return dynamic_pointer_cast<ParseSyntaxTreeNode>(child);
    }

    pst_node_ptr_t firstChild() const
    {
        auto child = this->front();
        return dynamic_pointer_cast<ParseSyntaxTreeNode>(child);
    }

    vector<pst_node_ptr_t> &getChildren() const
    {
        tree_children_t<pst_node_data> *children = (tree_children_t<pst_node_data> *)this;
        return *(std::vector<pst_node_ptr_t> *)children;
    }

    void replace(size_t index, pst_node_ptr_t node)
    {
        (this)->getChildren()[index] = node;
        node->parent = this;
    }

    template <typename func_t>
    void foreach (func_t f) const
    {
        auto nodeF = [=](tree_children_t<pst_node_data>::const_reference ref)
        {
            f(static_cast<pst_node_t &>(*(ref)));
        };
        std::for_each(this->begin(), this->end(), nodeF);
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

    std::string descData() const override;
};
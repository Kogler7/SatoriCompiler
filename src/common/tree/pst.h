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
    product_t product;
};

class ParseSyntaxTreeNode : public AbstractTreeNode<pst_node_data>
{
public:
    ParseSyntaxTreeNode(pst_node_data data) : AbstractTreeNode<pst_node_data>(data) {}
    ParseSyntaxTreeNode(node_type typ, string sym, size_t ln, size_t co);

    static pst_node_ptr_t createNode(node_type type, const string &symbol, size_t line, size_t col);

    pst_node_t &attachProduct(const product_t &product);

    pst_node_t &operator[](size_t index) const;
    pst_node_t &operator<<(const pst_node_ptr_t node);

    string descData() const override;
};
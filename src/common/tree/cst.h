/**
 * @file tree/cst.h
 * @author Zhenjie Wei (2024108@bjtu.edu.cn)
 * @brief Abstract Syntax Tree
 * @date 2023-05-01
 *
 * @copyright Copyright (c) 2023
 *
 */

#pragma once

#include <string>
#include "tree.h"
#include "utils/log.h"

using namespace std;

class ConcreteSyntaxTreeNode;

typedef ConcreteSyntaxTreeNode cst_node_t;
typedef ConcreteSyntaxTreeNode cst_tree_t;
typedef shared_ptr<cst_node_t> cst_node_ptr_t;
typedef shared_ptr<cst_node_t> cst_tree_ptr_t;
typedef vector<cst_node_ptr_t> cst_children_t;

enum node_type
{
    NON_TERM,
    TERMINAL
};

struct cst_node_data
{
    node_type type;
    string symbol;
    size_t line, col;
};

class ConcreteSyntaxTreeNode : public AbstractTreeNode<cst_node_data>
{
public:
    ConcreteSyntaxTreeNode(cst_node_data data) : AbstractTreeNode<cst_node_data>(data) {}
    ConcreteSyntaxTreeNode(node_type typ, string sym, size_t ln, size_t co);

    static cst_node_ptr_t createNode(node_type type, const string &symbol, size_t line, size_t col);

    cst_node_t &operator[](size_t index) const;
    cst_node_t &operator<<(const cst_node_ptr_t node);

    string desc() const override;
};
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

typedef ConcreteSyntaxTreeNode cst_node;
typedef ConcreteSyntaxTreeNode cst_tree;
typedef shared_ptr<cst_node> cst_node_ptr;
typedef shared_ptr<cst_node> cst_tree_ptr;
typedef vector<cst_node_ptr> cst_childs;

enum node_type
{
    NON_TERM,
    TERMINAL
};

struct ast_node_data
{
    node_type type;
    string symbol;
    size_t line, col;
};

class ConcreteSyntaxTreeNode : public AbstractTreeNode<ast_node_data>
{
public:
    ConcreteSyntaxTreeNode(ast_node_data data) : AbstractTreeNode<ast_node_data>(data) {}
    ConcreteSyntaxTreeNode(node_type typ, string sym, size_t ln, size_t co);

    static cst_node_ptr createNode(node_type type, const string &symbol, size_t line, size_t col);

    cst_node &operator[](size_t index);
    cst_node &operator<<(const cst_node_ptr node);

    string desc() const override;
};
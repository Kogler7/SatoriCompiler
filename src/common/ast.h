/**
 * @file ast.h
 * @author Zhenjie Wei (2024108@bjtu.edu.cn)
 * @brief Abstract Syntax Tree
 * @date 2023-05-01
 *
 * @copyright Copyright (c) 2023
 *
 */

#pragma once

#include <string>
#include "utils/tree.h"
#include "utils/log.h"

using namespace std;

class AbstractSyntaxTreeNode;

typedef AbstractSyntaxTreeNode ast_node;
typedef AbstractSyntaxTreeNode ast_tree;
typedef shared_ptr<ast_node> ast_node_ptr;
typedef shared_ptr<ast_node> ast_tree_ptr;
typedef vector<ast_node_ptr> ast_childs;

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

class AbstractSyntaxTreeNode : public AbstractTreeNode<ast_node_data>
{
public:
    AbstractSyntaxTreeNode(ast_node_data data) : AbstractTreeNode<ast_node_data>(data) {}
    AbstractSyntaxTreeNode(node_type typ, string sym, size_t ln, size_t co);

    static ast_node_ptr createNode(node_type type, const string &symbol, size_t line, size_t col);

    ast_node &operator[](size_t index)
    {
        // return static_cast<ast_node &>(this->get(index));
        return static_cast<ast_node &>(AbstractTreeNode<ast_node_data>::get(index));
    }
    ast_node &operator<<(const ast_node_ptr node)
    {
        return static_cast<ast_node &>(AbstractTreeNode<ast_node_data>::set(node));
        // return static_cast<ast_node &>(this->set(node));
    }

    string desc() const override;

    void print()
    {
        AbstractTreeNode<ast_node_data>::print();
    }
};
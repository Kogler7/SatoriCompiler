/**
 * @file cst.cpp
 * @author Zhenjie Wei (2024108@bjtu.edu.cn)
 * @brief Abstract Syntax Tree
 * @date 2023-05-01
 *
 * @copyright Copyright (c) 2023
 *
 */

#include "cst.h"
#include "sstream"
#include "utils/log.h"

ConcreteSyntaxTreeNode::ConcreteSyntaxTreeNode(
    node_type typ, string sym, size_t ln, size_t co) : AbstractTreeNode<ast_node_data>(ast_node_data(typ, sym, ln, co))
{
}

cst_node_ptr ConcreteSyntaxTreeNode::createNode(node_type type, const string &symbol, size_t line, size_t col)
{
    return make_shared<cst_node>(type, symbol, line, col);
}

cst_node &ConcreteSyntaxTreeNode::operator[](size_t index)
{
    return static_cast<cst_node &>(AbstractTreeNode<ast_node_data>::operator[](index));
}

cst_node &ConcreteSyntaxTreeNode::operator<<(const cst_node_ptr node)
{
    return static_cast<cst_node &>(AbstractTreeNode<ast_node_data>::operator<<(node));
}

string ConcreteSyntaxTreeNode::desc() const
{
    stringstream ss;
    ss << data.symbol << " at <" << data.line << ":" << data.col << ">";
    return ss.str();
}
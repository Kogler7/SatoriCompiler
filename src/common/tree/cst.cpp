/**
 * @file tree/cst.cpp
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
    node_type typ, string sym, size_t ln, size_t co) : AbstractTreeNode<cst_node_data>(cst_node_data(typ, sym, ln, co))
{
}

cst_node_ptr_t ConcreteSyntaxTreeNode::createNode(node_type type, const string &symbol, size_t line, size_t col)
{
    return make_shared<cst_node_t>(type, symbol, line, col);
}

cst_node_t &ConcreteSyntaxTreeNode::operator[](size_t index) const
{
    return static_cast<cst_node_t &>(AbstractTreeNode<cst_node_data>::operator[](index));
}

cst_node_t &ConcreteSyntaxTreeNode::operator<<(const cst_node_ptr_t node)
{
    return static_cast<cst_node_t &>(AbstractTreeNode<cst_node_data>::operator<<(node));
}

string ConcreteSyntaxTreeNode::desc() const
{
    stringstream ss;
    if (data.type == TERMINAL)
    {
        ss << '`' << data.symbol << '`';
        ss << " at <" << data.line << ":" << data.col << ">";
    }
    else
    {
        ss << data.symbol;
    }
    return ss.str();
}
/**
 * @file ast.cpp
 * @author Zhenjie Wei (2024108@bjtu.edu.cn)
 * @brief Abstract Syntax Tree
 * @date 2023-05-01
 *
 * @copyright Copyright (c) 2023
 *
 */

#include "ast.h"
#include "sstream"
#include "utils/log.h"

AbstractSyntaxTreeNode::AbstractSyntaxTreeNode(
    node_type typ, string sym, size_t ln, size_t co) : AbstractTreeNode<ast_node_data>(ast_node_data(typ, sym, ln, co))
{
}

ast_node_ptr AbstractSyntaxTreeNode::createNode(node_type type, const string &symbol, size_t line, size_t col)
{
    return make_shared<ast_node>(type, symbol, line, col);
}

string AbstractSyntaxTreeNode::desc() const
{
    stringstream ss;
    ss << data.symbol << " at <" << data.line << ":" << data.col << ">";
    return ss.str();
}
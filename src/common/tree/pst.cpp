/**
 * @file tree/pst.cpp
 * @author Zhenjie Wei (2024108@bjtu.edu.cn)
 * @brief Parse Syntax Tree
 * @date 2023-05-01
 *
 * @copyright Copyright (c) 2023
 *
 */

#include "pst.h"
#include "sstream"
#include "utils/log.h"
#include "utils/stl.h"

using namespace std;

ParseSyntaxTreeNode::ParseSyntaxTreeNode(
    node_type typ, string sym, size_t ln, size_t co) : AbstractTreeNode<pst_node_data>(pst_node_data(typ, sym, ln, co))
{
}

pst_node_ptr_t ParseSyntaxTreeNode::createNode(node_type type, const string &symbol, size_t line, size_t col)
{
    return make_shared<pst_node_t>(type, symbol, line, col);
}

pst_node_ptr_t ParseSyntaxTreeNode::createNode(pst_node_data data)
{
    return make_shared<pst_node_t>(data);
}

pst_node_t &ParseSyntaxTreeNode::attachProduct(product_t &product)
{
    data.product_opt = product;
    return *this;
}

pst_node_t &ParseSyntaxTreeNode::attachSemantic(semantic_t &semantic)
{
    data.semantic_opt = semantic;
    return *this;
}

pst_node_t &ParseSyntaxTreeNode::operator[](size_t index) const
{
    return static_cast<pst_node_t &>(AbstractTreeNode<pst_node_data>::operator[](index));
}

pst_node_t &ParseSyntaxTreeNode::operator<<(const pst_node_ptr_t node)
{
    return static_cast<pst_node_t &>(AbstractTreeNode<pst_node_data>::operator<<(node));
}

string ParseSyntaxTreeNode::descData() const
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
        if (data.semantic_opt.has_value())
        {
            auto &sem = data.semantic_opt.value();
            ss << " : " << sem;
        }
        else if (data.product_opt.has_value())
        {
            auto &pro = data.product_opt.value();
            ss << " <" << pro.first << " -> " << compact(pro.second) << ">";
        }
    }
    return ss.str();
}
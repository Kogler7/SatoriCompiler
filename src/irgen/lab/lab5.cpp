/**
 * @file lab5.cpp
 * @author Zhenjie Wei (2024108@bjtu.edu.cn)
 * @brief Visit Lab5
 * @date 2023-06-27
 *
 * @copyright Copyright (c) 2023
 *
 */

#include "lab5.h"

symbol_t LAB5Visitor::visitS(pst_node_ptr_t node)
{
    quads.push_back("(DEF_FUNC,f,,)");
    visitA(node->firstChild());
    symbol_t res = visitE(node->lastChild());
    quads.push_back(format(
        "(ASSIGN,$,,R)",
        res));
    quads.push_back("(END_FUNC,,,)");
    return "";
}

symbol_t LAB5Visitor::visitA(pst_node_ptr_t node)
{
    pst_children_t params = node->getChildren();
    for (auto param : params)
    {
        symbol_t p = visitP(param);
        funcDef.insert(p);
        quads.push_back(format(
            "(DEF_ARG,$,,)",
            p));
    }
    return "";
}

symbol_t LAB5Visitor::visitE(pst_node_ptr_t node)
{
    product_t &prod = node->data.product_opt.value();
    if (prod.second.size() > 1)
    {
        // E -> E +- T
        symbol_t lhs = visitE(node->firstChild());
        symbol_t rhs = visitT(node->lastChild());
        symbol_t t = format("T$", varCnt++);
        symbol_t op = prod.second[1] == "+" ? "ADD" : "SUB";
        quads.push_back(format(
            "($,$,$,$)",
            op,
            lhs,
            rhs,
            t));
        return t;
    }
    else
    {
        // E -> T
        return visitT(node->firstChild());
    }
}

symbol_t LAB5Visitor::visitT(pst_node_ptr_t node)
{
    product_t &prod = node->data.product_opt.value();
    if (prod.second.size() > 1)
    {
        // T -> T */ F
        symbol_t lhs = visitT(node->firstChild());
        symbol_t rhs = visitF(node->lastChild());
        symbol_t t = format("T$", varCnt++);
        symbol_t op = prod.second[1] == "*" ? "MUL" : "DIV";
        quads.push_back(format(
            "($,$,$,$)",
            op,
            lhs,
            rhs,
            t));
        return t;
    }
    else
    {
        // T -> F
        return visitF(node->firstChild());
    }
}

symbol_t LAB5Visitor::visitF(pst_node_ptr_t node)
{
    pst_node_ptr_t child = node->firstChild();
    if (child->data.symbol == "P")
    {
        symbol_t p = visitP(child);
        assert(
            funcDef.find(p) != funcDef.end(),
            format("Function param $ not defined!", p));
        return p;
    }
    else
    {
        return visitE(child);
    }
}

symbol_t LAB5Visitor::visitP(pst_node_ptr_t node)
{
    return node->firstChild()->data.symbol;
}

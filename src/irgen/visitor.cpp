/**
 * @file visitor.h
 * @author Zhenjie Wei (2024108@bjtu.edu.cn)
 * @brief CST Visitor
 * @date 2023-06-08
 *
 * @copyright Copyright (c) 2023
 *
 */

#include "visitor.h"

#define _is_term_node(node) (node->data.type == TERMINAL)

std::any RSCVisitor::visitProgram(pst_node_ptr_t node)
{
    return std::any();
}

std::any RSCVisitor::visitFuncDef(pst_node_ptr_t node)
{
    return std::any();
}

std::any RSCVisitor::visitFuncDecl(pst_node_ptr_t node)
{
    return std::any();
}

std::any RSCVisitor::visitFuncBlock(pst_node_ptr_t node)
{
    return std::any();
}

std::any RSCVisitor::visitFuncCall(pst_node_ptr_t node)
{
    return std::any();
}

std::any RSCVisitor::visitVarDecl(pst_node_ptr_t node)
{
    return std::any();
}

std::any RSCVisitor::visitVarDef(pst_node_ptr_t node)
{
    return std::any();
}

std::any RSCVisitor::visitVarAssign(pst_node_ptr_t node)
{
    return std::any();
}

std::any RSCVisitor::visitIfStmt(pst_node_ptr_t node)
{
    return std::any();
}

std::any RSCVisitor::visitWhileStmt(pst_node_ptr_t node)
{
    return std::any();
}

std::any RSCVisitor::visitForStmt(pst_node_ptr_t node)
{
    return std::any();
}

std::any RSCVisitor::visitBreakStmt(pst_node_ptr_t node)
{
    return std::any();
}

std::any RSCVisitor::visitContinueStmt(pst_node_ptr_t node)
{
    return std::any();
}

std::any RSCVisitor::visitReturnStmt(pst_node_ptr_t node)
{
    return std::any();
}

std::any RSCVisitor::visitExprStmt(pst_node_ptr_t node)
{
    return std::any();
}

std::any RSCVisitor::visitLVal(pst_node_ptr_t node)
{
    return std::any();
}

std::any RSCVisitor::visitExpr(pst_node_ptr_t node)
{
    return std::any();
}

std::any RSCVisitor::visitBinaryExpr(pst_node_ptr_t node)
{
    return std::any();
}

std::any RSCVisitor::visitUnaryExpr(pst_node_ptr_t node)
{
    return std::any();
}

std::any RSCVisitor::visitLiteral(pst_node_ptr_t node)
{
    return std::any();
}

std::any RSCVisitor::visitFactor(pst_node_ptr_t node)
{
    // 如果node自身是终结符节点，则可能是string, true, false
    // 先处理true, false，剩下的就是string
    return std::any();
}
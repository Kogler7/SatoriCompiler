/**
 * @file visitor.h
 * @author Zhenjie Wei (2024108@bjtu.edu.cn)
 * @brief CST Visitor
 * @date 2023-06-08
 *
 * @copyright Copyright (c) 2023
 *
 */

#pragma once

#include "context.h"
#include "ret_info.h"
#include "common/tree/pst.h"

#include <any>

class RSCVisitor
{
    Context context;

public:
    ret_info visitProgram(pst_node_ptr_t node);

    ret_info visitVarDeclStmt(pst_node_ptr_t node);
    ret_info visitVarDecl(pst_node_ptr_t node);
    ret_info visitVarDef(pst_node_ptr_t node);

    ret_info visitInitVal(pst_node_ptr_t node);

    ret_info visitFuncDeclStmt(pst_node_ptr_t node);
    ret_info visitFuncDecl(pst_node_ptr_t node);
    ret_info visitFuncDef(pst_node_ptr_t node);
    ret_info visitFuncCall(pst_node_ptr_t node);

    ret_info visitArgList(pst_node_ptr_t node);
    ret_info visitParamList(pst_node_ptr_t node);
    ret_info visitParam(pst_node_ptr_t node);

    ret_info visitStmt(pst_node_ptr_t node);
    ret_info visitBlock(pst_node_ptr_t node);
    ret_info visitAssignment(pst_node_ptr_t node);
    ret_info visitIfStmt(pst_node_ptr_t node);
    ret_info visitWhileStmt(pst_node_ptr_t node);
    ret_info visitForStmt(pst_node_ptr_t node);
    ret_info visitBreakStmt(pst_node_ptr_t node);
    ret_info visitContinueStmt(pst_node_ptr_t node);
    ret_info visitReturnStmt(pst_node_ptr_t node);
    ret_info visitExprStmt(pst_node_ptr_t node);

    ret_info visitUnaryExpr(pst_node_ptr_t node);
    ret_info visitMulExpr(pst_node_ptr_t node);
    ret_info visitExpr(pst_node_ptr_t node);
    ret_info visitRelExpr(pst_node_ptr_t node);
    ret_info visitAndExpr(pst_node_ptr_t node);
    ret_info visitOrExpr(pst_node_ptr_t node);
    ret_info visitBoolExpr(pst_node_ptr_t node);

    // factors
    ret_info visitFactor(pst_node_ptr_t node);
    ret_info visitLVal(pst_node_ptr_t node);
    ret_info visitLiteral(pst_node_ptr_t node);
};
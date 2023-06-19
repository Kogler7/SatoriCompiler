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
    ret_info_t visitProgram(pst_node_ptr_t node);

    ret_info_t visitVarDeclStmt(pst_node_ptr_t node);
    ret_info_t visitVarDecl(pst_node_ptr_t node);
    ret_info_t visitVarDef(pst_node_ptr_t node);

    ret_info_t visitInitVal(pst_node_ptr_t node);

    ret_info_t visitFuncDeclStmt(pst_node_ptr_t node);
    ret_info_t visitFuncDecl(pst_node_ptr_t node);
    ret_info_t visitFuncDef(pst_node_ptr_t node);
    ret_info_t visitFuncCall(pst_node_ptr_t node);

    ret_info_t visitArgList(pst_node_ptr_t node);
    ret_info_t visitParamList(pst_node_ptr_t node);
    ret_info_t visitParam(pst_node_ptr_t node);

    ret_info_t visitStmt(pst_node_ptr_t node);
    ret_info_t visitBlock(pst_node_ptr_t node);
    ret_info_t visitAssignment(pst_node_ptr_t node);
    ret_info_t visitIfStmt(pst_node_ptr_t node);
    ret_info_t visitWhileStmt(pst_node_ptr_t node);
    ret_info_t visitForStmt(pst_node_ptr_t node);
    ret_info_t visitBreakStmt(pst_node_ptr_t node);
    ret_info_t visitContinueStmt(pst_node_ptr_t node);
    ret_info_t visitReturnStmt(pst_node_ptr_t node);
    ret_info_t visitExprStmt(pst_node_ptr_t node);

    ret_info_t visitUnaryExpr(pst_node_ptr_t node);
    ret_info_t visitMulExpr(pst_node_ptr_t node);
    ret_info_t visitExpr(pst_node_ptr_t node);
    ret_info_t visitRelExpr(pst_node_ptr_t node);
    ret_info_t visitAndExpr(pst_node_ptr_t node);
    ret_info_t visitOrExpr(pst_node_ptr_t node);
    ret_info_t visitBoolExpr(pst_node_ptr_t node);

    // factors
    ret_info_t visitFactor(pst_node_ptr_t node);
    ret_info_t visitLVal(pst_node_ptr_t node);
    ret_info_t visitLiteral(pst_node_ptr_t node);
};
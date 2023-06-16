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
    StmtRetInfo visitProgram(pst_node_ptr_t node);

    StmtRetInfo visitVarDeclStmt(pst_node_ptr_t node);
    StmtRetInfo visitVarDecl(pst_node_ptr_t node);
    StmtRetInfo visitVarDef(pst_node_ptr_t node);

    StmtRetInfo visitInitVal(pst_node_ptr_t node);
    
    StmtRetInfo visitFuncDeclStmt(pst_node_ptr_t node);
    StmtRetInfo visitFuncDecl(pst_node_ptr_t node);
    StmtRetInfo visitFuncDef(pst_node_ptr_t node);
    StmtRetInfo visitFuncCall(pst_node_ptr_t node);

    StmtRetInfo visitArgList(pst_node_ptr_t node);
    StmtRetInfo visitParamList(pst_node_ptr_t node);
    StmtRetInfo visitParam(pst_node_ptr_t node);

    StmtRetInfo visitStmt(pst_node_ptr_t node);
    StmtRetInfo visitBlock(pst_node_ptr_t node);
    StmtRetInfo visitAssignment(pst_node_ptr_t node);
    StmtRetInfo visitIfStmt(pst_node_ptr_t node);
    StmtRetInfo visitWhileStmt(pst_node_ptr_t node);
    StmtRetInfo visitForStmt(pst_node_ptr_t node);
    StmtRetInfo visitBreakStmt(pst_node_ptr_t node);
    StmtRetInfo visitContinueStmt(pst_node_ptr_t node);
    StmtRetInfo visitReturnStmt(pst_node_ptr_t node);
    StmtRetInfo visitExprStmt(pst_node_ptr_t node);
    
    StmtRetInfo visitUnaryExpr(pst_node_ptr_t node);
    StmtRetInfo visitMulExpr(pst_node_ptr_t node);
    StmtRetInfo visitExpr(pst_node_ptr_t node);
    StmtRetInfo visitRelExpr(pst_node_ptr_t node);
    StmtRetInfo visitAndExpr(pst_node_ptr_t node);
    StmtRetInfo visitOrExpr(pst_node_ptr_t node);
    StmtRetInfo visitBoolExpr(pst_node_ptr_t node);

    // factors
    StmtRetInfo visitFactor(pst_node_ptr_t node);
    StmtRetInfo visitLVal(pst_node_ptr_t node);
    StmtRetInfo visitLiteral(pst_node_ptr_t node); // string, true, false
};
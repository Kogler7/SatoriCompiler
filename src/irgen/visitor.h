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

    StmtRetInfo visitVarDeclStmt(pst_node_ptr_t node); // Alpha
    StmtRetInfo visitVarDecl(pst_node_ptr_t node);     // Alpha
    StmtRetInfo visitVarDef(pst_node_ptr_t node);      // Alpha

    StmtRetInfo visitInitVal(pst_node_ptr_t node); // Alpha

    StmtRetInfo visitFuncDeclStmt(pst_node_ptr_t node);
    StmtRetInfo visitFuncDecl(pst_node_ptr_t node);
    StmtRetInfo visitFuncDef(pst_node_ptr_t node);
    StmtRetInfo visitFuncCall(pst_node_ptr_t node);

    StmtRetInfo visitArgList(pst_node_ptr_t node);   // Alpha
    StmtRetInfo visitParamList(pst_node_ptr_t node); // Alpha
    StmtRetInfo visitParam(pst_node_ptr_t node);     // Alpha

    StmtRetInfo visitStmt(pst_node_ptr_t node);       // Beta
    StmtRetInfo visitBlock(pst_node_ptr_t node);      // Beta
    StmtRetInfo visitAssignment(pst_node_ptr_t node); // Alpha
    StmtRetInfo visitIfStmt(pst_node_ptr_t node);     // Beta
    StmtRetInfo visitWhileStmt(pst_node_ptr_t node);
    StmtRetInfo visitForStmt(pst_node_ptr_t node);
    StmtRetInfo visitBreakStmt(pst_node_ptr_t node);    // Theta
    StmtRetInfo visitContinueStmt(pst_node_ptr_t node); // Theta
    StmtRetInfo visitReturnStmt(pst_node_ptr_t node);   // Theta
    StmtRetInfo visitExprStmt(pst_node_ptr_t node);     // Alpha

    StmtRetInfo visitUnaryExpr(pst_node_ptr_t node); // Alpha
    StmtRetInfo visitMulExpr(pst_node_ptr_t node);   // Alpha
    StmtRetInfo visitExpr(pst_node_ptr_t node);      // Alpha
    StmtRetInfo visitRelExpr(pst_node_ptr_t node);   // Beta
    StmtRetInfo visitAndExpr(pst_node_ptr_t node);   // Beta
    StmtRetInfo visitOrExpr(pst_node_ptr_t node);    // Beta
    StmtRetInfo visitBoolExpr(pst_node_ptr_t node);  // Beta

    // factors
    StmtRetInfo visitFactor(pst_node_ptr_t node);  // Alpha
    StmtRetInfo visitLVal(pst_node_ptr_t node);    // Alpha
    StmtRetInfo visitLiteral(pst_node_ptr_t node); // Alpha
};
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
#include "common/tree/pst.h"

#include <any>

class RSCVisitor
{
    Context context;

public:
    std::any visitProgram(pst_node_ptr_t node);
    std::any visitFuncDef(pst_node_ptr_t node);
    std::any visitFuncDecl(pst_node_ptr_t node);
    std::any visitFuncBlock(pst_node_ptr_t node);
    std::any visitFuncCall(pst_node_ptr_t node);
    std::any visitVarDecl(pst_node_ptr_t node);
    std::any visitVarDef(pst_node_ptr_t node);
    std::any visitVarAssign(pst_node_ptr_t node);


    std::any visitIfStmt(pst_node_ptr_t node);
    std::any visitWhileStmt(pst_node_ptr_t node);
    std::any visitForStmt(pst_node_ptr_t node);
    std::any visitBreakStmt(pst_node_ptr_t node);
    std::any visitContinueStmt(pst_node_ptr_t node);
    std::any visitReturnStmt(pst_node_ptr_t node);
    std::any visitExprStmt(pst_node_ptr_t node);

    // factors
    std::any visitFactor(pst_node_ptr_t node);
    std::any visitLVal(pst_node_ptr_t node);
    std::any visitExpr(pst_node_ptr_t node);
    std::any visitNumber(pst_node_ptr_t node);
    std::any visitLiteral(pst_node_ptr_t node); // string, true, false

    std::any visitBinaryExpr(pst_node_ptr_t node);
    std::any visitUnaryExpr(pst_node_ptr_t node);

    
    
};
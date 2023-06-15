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
#include "instr.h"

#define _is_term_node(node) (node->data.type == TERMINAL)
#define _is_non_term_node(node) (node->data.type == NON_TERM)

StmtRetInfo RSCVisitor::visitProgram(pst_node_ptr_t node)
{
    return StmtRetInfo();
}

StmtRetInfo RSCVisitor::visitVarDeclStmt(pst_node_ptr_t node)
{
    return StmtRetInfo();
}

StmtRetInfo RSCVisitor::visitVarDecl(pst_node_ptr_t node)
{
    return StmtRetInfo();
}

StmtRetInfo RSCVisitor::visitVarType(pst_node_ptr_t node)
{
    return StmtRetInfo();
}

StmtRetInfo RSCVisitor::visitVarDef(pst_node_ptr_t node)
{
    return StmtRetInfo();
}

StmtRetInfo RSCVisitor::visitInitVal(pst_node_ptr_t node)
{
    return StmtRetInfo();
}

StmtRetInfo RSCVisitor::visitFuncDeclStmt(pst_node_ptr_t node)
{
    return StmtRetInfo();
}

StmtRetInfo RSCVisitor::visitFuncDecl(pst_node_ptr_t node)
{
    return StmtRetInfo();
}

StmtRetInfo RSCVisitor::visitFuncDef(pst_node_ptr_t node)
{
    return StmtRetInfo();
}

StmtRetInfo RSCVisitor::visitFuncCall(pst_node_ptr_t node)
{
    return StmtRetInfo();
}

StmtRetInfo RSCVisitor::visitArgList(pst_node_ptr_t node)
{
    return StmtRetInfo();
}

StmtRetInfo RSCVisitor::visitParamList(pst_node_ptr_t node)
{
    return StmtRetInfo();
}

StmtRetInfo RSCVisitor::visitParam(pst_node_ptr_t node)
{
    return StmtRetInfo();
}

StmtRetInfo RSCVisitor::visitStmt(pst_node_ptr_t node)
{
    return StmtRetInfo();
}

StmtRetInfo RSCVisitor::visitBlock(pst_node_ptr_t node)
{
    return StmtRetInfo();
}

StmtRetInfo RSCVisitor::visitAssignment(pst_node_ptr_t node)
{
    return StmtRetInfo();
}

StmtRetInfo RSCVisitor::visitIfStmt(pst_node_ptr_t node)
{
    return StmtRetInfo();
}

StmtRetInfo RSCVisitor::visitWhileStmt(pst_node_ptr_t node)
{
    return StmtRetInfo();
}

StmtRetInfo RSCVisitor::visitForStmt(pst_node_ptr_t node)
{
    return StmtRetInfo();
}

StmtRetInfo RSCVisitor::visitBreakStmt(pst_node_ptr_t node)
{
    return StmtRetInfo();
}

StmtRetInfo RSCVisitor::visitContinueStmt(pst_node_ptr_t node)
{
    return StmtRetInfo();
}

StmtRetInfo RSCVisitor::visitReturnStmt(pst_node_ptr_t node)
{
    return StmtRetInfo();
}

StmtRetInfo RSCVisitor::visitExprStmt(pst_node_ptr_t node)
{
    return StmtRetInfo();
}

StmtRetInfo RSCVisitor::visitUnaryExpr(pst_node_ptr_t node)
{
    return StmtRetInfo();
}

StmtRetInfo RSCVisitor::visitMulExpr(pst_node_ptr_t node)
{
    return StmtRetInfo();
}

StmtRetInfo RSCVisitor::visitExpr(pst_node_ptr_t node)
{
    return StmtRetInfo();
}

StmtRetInfo RSCVisitor::visitRelExpr(pst_node_ptr_t node)
{
    return StmtRetInfo();
}

StmtRetInfo RSCVisitor::visitEqExpr(pst_node_ptr_t node)
{
    return StmtRetInfo();
}

StmtRetInfo RSCVisitor::visitAndExpr(pst_node_ptr_t node)
{
    return StmtRetInfo();
}

// OrExpr -> OrExpr `||` AndExpr | AndExpr
StmtRetInfo RSCVisitor::visitOrExpr(pst_node_ptr_t node)
{
    pst_node_ptr_t child = node->firstChild();
    if (child->data.symbol == "OrExpr")
    {
        return visitOrExpr(child);
    }
    else
    {
        // 处理或运算
        pst_children_t children = node->getChildren();
        auto lhsInfo = visitAndExpr(children[0]);
        assert(is_beta(lhsInfo), "lhs of or expr return info should be beta");
        auto rhsInfo = visitAndExpr(children[1]);
        assert(is_beta(rhsInfo), "rhs of or expr return info should be beta");
        BetaStmtRetInfo lhs = get_beta(lhsInfo);
        BetaStmtRetInfo rhs = get_beta(rhsInfo);
    }
}

// BoolExpr -> OrExpr
StmtRetInfo RSCVisitor::visitBoolExpr(pst_node_ptr_t node)
{
    return visitOrExpr(node->firstChild());
}

// Factor -> (Expr), LVal, Literal, FuncCall
StmtRetInfo RSCVisitor::visitFactor(pst_node_ptr_t node)
{
    pst_node_ptr_t child = node->firstChild();
    // 先处理(Expr), LVal, FuncCall
    if (child->data.symbol == "Expr")
    {
        // (Expr)
        return visitExpr(child);
    }
    else if (child->data.symbol == "LVal")
    {
        // LVal
        return visitLVal(child);
    }
    else if (child->data.symbol == "FuncCall")
    {
        // FuncCall
        return visitFuncCall(child);
    }
    else
    {
        // Literal
        return visitLiteral(child);
    }
}

StmtRetInfo RSCVisitor::visitLVal(pst_node_ptr_t node)
{
    alloc_ptr_t instr = context.symbolTable.find(node->firstChild()->data.symbol);
    assert(
        instr != nullptr,
        format("undefined symbol: $", node->firstChild()->data.symbol));
    return AlphaStmtRetInfo{std::list<user_ptr_t>{instr}};
}

// Factor -> int, real, char, string, true, false
StmtRetInfo RSCVisitor::visitLiteral(pst_node_ptr_t node)
{
    // 如果node自身是终结符节点，则可能是char, string, true, false
    // 先处理true, false，剩下的就是char, string
    if (_is_term_node(node))
    {
        if (node->data.symbol == "true")
        {
            // true
            auto instr = make_const_bool(true);
            return AlphaStmtRetInfo{std::list<user_ptr_t>{instr}};
        }
        else if (node->data.symbol == "false")
        {
            // false
            auto instr = make_const_bool(false);
            return AlphaStmtRetInfo{std::list<user_ptr_t>{instr}};
        }
        else if (node->data.symbol[0] == '\'')
        {
            // char
            auto instr = make_const_char(node->data.symbol[1]);
            return AlphaStmtRetInfo{std::list<user_ptr_t>{instr}};
        }
        else if (node->data.symbol[0] == '\"')
        {
            // string
            auto instr = make_const_str(node->data.symbol.substr(1, node->data.symbol.size() - 2));
            return AlphaStmtRetInfo{std::list<user_ptr_t>{instr}};
        }
        else
        {
            // error
            error << "unknown literal" << std::endl;
            exit(1);
        }
    }
    // 如果node自身是非终结符节点，则可能是int, real
    else
    {
        if (node->data.symbol == "IntLiteral")
        {
            // int
            auto instr = make_const_int(std::stoi(node->firstChild()->data.symbol));
            return AlphaStmtRetInfo{std::list<user_ptr_t>{instr}};
        }
        else if (node->data.symbol == "RealLiteral")
        {
            // real
            auto instr = make_const_real(std::stod(node->firstChild()->data.symbol));
            return AlphaStmtRetInfo{std::list<user_ptr_t>{instr}};
        }
        else
        {
            // error
            error << "unknown literal" << std::endl;
            exit(1);
        }
    }
}
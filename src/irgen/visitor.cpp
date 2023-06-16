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

inline std::string getProSymAt(pst_node_ptr_t node, size_t idx)
{
    assert(_is_non_term_node(node), "node should be non terminal");
    auto &productOpt = node->data.product_opt;
    assert(productOpt.has_value(), "node should have product option");
    assert(idx < productOpt.value().second.size(), "idx out of range");
    return productOpt.value().second[idx];
}

// Program -> { VarDeclStmt | FuncDeclStmt | FuncDef }
StmtRetInfo RSCVisitor::visitProgram(pst_node_ptr_t node)
{
    pst_children_t children = node->getChildren();
    for (pst_node_ptr_t child : children)
    {
        if (child->data.symbol == "VarDeclStmt")
        {
            visitVarDeclStmt(child);
        }
        else if (child->data.symbol == "FuncDeclStmt")
        {
            visitFuncDeclStmt(child);
        }
        else if (child->data.symbol == "FuncDef")
        {
            visitFuncDef(child);
        }
        else
        {
            error << "unknown symbol" << std::endl;
            exit(1);
        }
    }
}

// VarDeclStmt -> VarDecl ;
StmtRetInfo RSCVisitor::visitVarDeclStmt(pst_node_ptr_t node)
{
    return visitVarDecl(node->firstChild());
}

// VarDecl -> { VarDef }
StmtRetInfo RSCVisitor::visitVarDecl(pst_node_ptr_t node)
{
    AlphaStmtRetInfo retInfo;
    // 获取VarDef列表，遍历处理即可
    pst_children_t children = node->getChildren();
    for (pst_node_ptr_t child : children)
    {
        auto varInfo = visitVarDef(child);
        assert(is_alpha(varInfo), "var def return info should be alpha");
        retInfo.list.splice(retInfo.list.end(), get_alpha(varInfo).list);
    }
    return retInfo;
}

// VarDef -> ident : Type
StmtRetInfo RSCVisitor::visitVarDef(pst_node_ptr_t node)
{
    // 获取ident和Type
    // 暂时不考虑数组，因此剩余子节点暂时不处理
    std::string identStr = node->getChildAt(0)->data.symbol;
    std::string typeStr = node->getChildAt(1)->firstChild()->data.symbol;
    type_ptr_t type = make_prime_type(PrimitiveType::str2type(typeStr));
    alloc_ptr_t alloc = context.symbolTable.registerSymbol(identStr, type);
    return AlphaStmtRetInfo{std::list<user_ptr_t>{alloc}};
}

// InitVal -> Expr | { Expr }
StmtRetInfo RSCVisitor::visitInitVal(pst_node_ptr_t node)
{
    // 暂时不考虑数组，因此只需要处理Expr即可
    if (node->childrenCount() == 1)
    {
        // InitVal -> Expr
        return visitExpr(node->firstChild());
    }
    return AlphaStmtRetInfo();
}

// FuncDeclStmt -> FuncDecl ;
StmtRetInfo RSCVisitor::visitFuncDeclStmt(pst_node_ptr_t node)
{
    return visitFuncDecl(node->firstChild());
}

// FuncDecl -> ident ( [ParamList] ) [: Type] ;
StmtRetInfo RSCVisitor::visitFuncDecl(pst_node_ptr_t node)
{
    pst_children_t children = node->getChildren();
    std::string identStr = children[0]->data.symbol;
    pst_node_ptr_t paramsNode = children[1];
    pst_node_ptr_t retTypeNode = children[2];
    // 先根据函数名和返回值类型注册函数
    type_ptr_t retType = nullptr;
    if (retTypeNode->hasChild())
    {
        // FuncDecl -> ident ( [ParamList] ) : Type ;
        std::string typeStr = retTypeNode->firstChild()->data.symbol;
        retType = make_prime_type(PrimitiveType::str2type(typeStr));
    }
    else
    {
        // FuncDecl -> ident ( [ParamList] ) ;
        // 没有指定返回类型，则默认为void
        retType = make_prime_type(PrimitiveType::VOID);
    }
    func_ptr_t func = context.functionTable.registerFunction(identStr, retType);
    // 解析参数列表，为函数添加参数
    auto paramsInfo = visitParamList(paramsNode);
    assert(is_alpha(paramsInfo), "param list return info should be alpha");
    func->addParams(get_alpha(paramsInfo).list);
    return AlphaStmtRetInfo{std::list<user_ptr_t>{func}};
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

// ParamList -> { Param }
StmtRetInfo RSCVisitor::visitParamList(pst_node_ptr_t node)
{
    // 获取Param列表，遍历处理即可
    AlphaStmtRetInfo retInfo;
    pst_children_t children = node->getChildren();
    for (pst_node_ptr_t child : children)
    {
        auto paramInfo = visitParam(child);
        assert(is_alpha(paramInfo), "param return info should be alpha");
        retInfo.list.splice(retInfo.list.end(), get_alpha(paramInfo).list);
    }
    return retInfo;
}

// Param -> ident : Type
StmtRetInfo RSCVisitor::visitParam(pst_node_ptr_t node)
{
    // 获取ident和Type
    // 暂时不考虑数组，因此剩余子节点暂时不处理
    std::string identStr = node->getChildAt(0)->data.symbol;
    std::string typeStr = node->getChildAt(1)->firstChild()->data.symbol;
    type_ptr_t type = make_prime_type(PrimitiveType::str2type(typeStr));
    alloc_ptr_t alloc = context.symbolTable.registerSymbol(identStr, type);
    return AlphaStmtRetInfo{std::list<user_ptr_t>{alloc}};
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

// Stmt -> break ;
StmtRetInfo RSCVisitor::visitBreakStmt(pst_node_ptr_t node)
{
    jmp_ptr_t instr = make_jmp();
    return ThetaStmtRetInfo{{}, instr};
}

// Stmt -> continue ;
StmtRetInfo RSCVisitor::visitContinueStmt(pst_node_ptr_t node)
{
    jmp_ptr_t instr = make_jmp();
    return ThetaStmtRetInfo{{}, instr};
}

// Stmt -> return [Expr] ;
StmtRetInfo RSCVisitor::visitReturnStmt(pst_node_ptr_t node)
{
    size_t childSum = node->childrenCount();
    assert(childSum == 1, "return stmt should have 1 child(ren)");
    pst_node_ptr_t child = node->firstChild(); // Optional
    if (child->childrenCount() == 1)
    {
        // Optional -> Expr ;
        auto exprInfo = visitExpr(child->firstChild());
        assert(is_alpha(exprInfo), "expr return info should be alpha");
        instr_list_t list = get_alpha(exprInfo).list;
        // 生成ret指令并追加到list之后
        ret_ptr_t instr = make_ret(list.back());
        list.push_back(instr);
        // ret是terminator指令，所以返回的是ThetaStmtRetInfo
        // 这里是ThetaStmtRetInfo的特例，因为ret指令不会跳转，所以jmpSsa为nullptr
        return ThetaStmtRetInfo{list, nullptr};
    }
    else
    {
        // Optional -> ;
        return ThetaStmtRetInfo{{make_ret()}, nullptr};
    }
}

// Stmt -> Expr ;
StmtRetInfo RSCVisitor::visitExprStmt(pst_node_ptr_t node)
{
    size_t childSum = node->childrenCount();
    assert(childSum == 1, "expr stmt should have 1 child(ren)");
    pst_node_ptr_t child = node->firstChild(); // Optional
    if (child->childrenCount() == 1)
    {
        // Optional -> Expr ;
        return visitExpr(child->firstChild());
    }
    return StmtRetInfo();
}

// UnaryExpr -> ( `+` | `-` | `!` ) UnaryExpr | Factor
StmtRetInfo RSCVisitor::visitUnaryExpr(pst_node_ptr_t node)
{
    pst_node_ptr_t child = node->firstChild();
    if (child->data.symbol == "Factor")
    {
        // Factor
        return visitFactor(child);
    }
    // ( `+` | `-` | `!` ) UnaryExpr
    auto exprInfo = visitUnaryExpr(child);
    assert(is_alpha(exprInfo), "expr return info should be alpha");
    instr_list_t list = get_alpha(exprInfo).list;
    // 目前仅支持 - 操作，+ 和 ! 操作暂不支持
    auto op = getProSymAt(node, 0); // + | - | !
    assert(op == "-", "only support unary minus");
    user_ptr_t operand = list.back();
    // 生成neg指令并追加到list之后
    neg_ptr_t instr = make_neg(operand, operand->getType()->getOpType());
    list.push_back(instr);
    return exprInfo;
}

// MulExpr -> MulExpr ( `*` | `/` | `%` ) UnaryExpr | UnaryExpr
StmtRetInfo RSCVisitor::visitMulExpr(pst_node_ptr_t node)
{
    pst_node_ptr_t child = node->firstChild();
    if (child->data.symbol == "UnaryExpr")
    {
        // UnaryExpr
        return visitUnaryExpr(child);
    }
    // MulExpr ( `*` | `/` | `%` ) UnaryExpr
    assert(node->childrenCount() == 2, "mul expr should have 1 or 2 child(ren)");
    auto lhsInfo = visitMulExpr(child);
    assert(is_alpha(lhsInfo), "lhs of mul expr return info should be alpha");
    auto rhsInfo = visitUnaryExpr(node->getChildAt(1));
    assert(is_alpha(rhsInfo), "rhs of mul expr return info should be alpha");
    auto op = getProSymAt(node, 1); // * | / | %
    user_ptr_t lhs = get_alpha(lhsInfo).list.back();
    user_ptr_t rhs = get_alpha(rhsInfo).list.back();
    // 检查类型是否匹配
    assert(
        lhs->getType()->getOpType() == rhs->getType()->getOpType(),
        "type of lhs and rhs should be the same");
    // 生成相关指令并追加到list之后
    instr_list_t list = get_alpha(lhsInfo).list;
    user_ptr_t instr = nullptr;
    if (op == "*")
    {
        instr = make_mul(lhs, rhs, lhs->getType()->getOpType());
    }
    else if (op == "/")
    {
        instr = make_div(lhs, rhs, lhs->getType()->getOpType());
    }
    else if (op == "%")
    {
        instr = make_rem(lhs, rhs, lhs->getType()->getOpType());
    }
    else
    {
        error << "unknown operator" << std::endl;
        exit(1);
    }
    list.push_back(instr);
    return AlphaStmtRetInfo{list};
}

// Expr -> Expr ( `+` | `-` ) MulExpr | MulExpr
StmtRetInfo RSCVisitor::visitExpr(pst_node_ptr_t node)
{
    pst_node_ptr_t child = node->firstChild();
    if (child->data.symbol == "MulExpr")
    {
        // MulExpr
        return visitMulExpr(child);
    }
    // Expr ( `+` | `-` ) MulExpr
    assert(node->childrenCount() == 2, "expr should have 1 or 2 child(ren)");
    auto lhsInfo = visitExpr(child);
    assert(is_alpha(lhsInfo), "lhs of expr return info should be alpha");
    auto rhsInfo = visitMulExpr(node->getChildAt(1));
    assert(is_alpha(rhsInfo), "rhs of expr return info should be alpha");
    auto op = getProSymAt(node, 1); // + | -
    user_ptr_t lhs = get_alpha(lhsInfo).list.back();
    user_ptr_t rhs = get_alpha(rhsInfo).list.back();
    // 检查类型是否匹配
    assert(
        lhs->getType()->getOpType() == rhs->getType()->getOpType(),
        "type of lhs and rhs should be the same");
    // 生成相关指令并追加到list之后
    instr_list_t list = get_alpha(lhsInfo).list;
    user_ptr_t instr = nullptr;
    if (op == "+")
    {
        instr = make_add(lhs, rhs, lhs->getType()->getOpType());
    }
    else if (op == "-")
    {
        instr = make_sub(lhs, rhs, lhs->getType()->getOpType());
    }
    else
    {
        error << "unknown operator" << std::endl;
        exit(1);
    }
    list.push_back(instr);
    return AlphaStmtRetInfo{list};
}

// RelExpr -> Expr ( `<` | `<=` | `>` | `>=` | `==` | `!=` ) Expr | Expr
StmtRetInfo RSCVisitor::visitRelExpr(pst_node_ptr_t node)
{
    // 在这一步，将底层传来的AlphaStmtRetInfo转换为BetaStmtRetInfo
    // 构造跳转指令，构造BasicBlock
    size_t childNum = node->childrenCount();
    block_ptr_t bb = make_block("rel_expr");
    cmp_ptr_t cmpInstr = nullptr;
    br_ptr_t brInstr = nullptr;
    if (childNum == 1)
    {
        // 如果是单个Expr，则直接将其和0比较
        // 即设置为Expr != 0，以便将其和后续的逻辑运算符连接
        auto exprInfo = visitExpr(node->firstChild());
        assert(is_alpha(exprInfo), "expr return info should be alpha");
        // 将原来的指令列表包装成BasicBlock
        instr_list_t list = get_alpha(exprInfo).list;
        for (user_ptr_t instr : list)
        {
            bb->addInstr(instr);
        }
        // 添加cmp expr 0指令，追加到bb之后
        user_ptr_t lhs = list.back();
        user_ptr_t rhs = make_const_int(0);
        // cmp n lhs 0
        cmpInstr = make_cmp(lhs, rhs, lhs->getType()->getOpType(), CT_NE);
        // 添加br指令，追加到bb之后
        brInstr = make_br(cmpInstr);
    }
    else
    {
        assert(childNum == 2, "rel expr should have 1 or 2 child(ren)");
        auto lhsInfo = visitRelExpr(node->firstChild());
        assert(is_alpha(lhsInfo), "lhs of rel expr return info should be alpha");
        auto rhsInfo = visitExpr(node->getChildAt(1));
        assert(is_alpha(rhsInfo), "rhs of rel expr return info should be alpha");
        // 在产生式中获取具体的关系运算符
        auto op = getProSymAt(node, 1); // < | <= | > | >= | == | !=
        // 将原来的指令列表包装成BasicBlock
        instr_list_t lhsList = get_alpha(lhsInfo).list;
        instr_list_t rhsList = get_alpha(rhsInfo).list;
        for (user_ptr_t instr : lhsList)
        {
            bb->addInstr(instr);
        }
        for (user_ptr_t instr : rhsList)
        {
            bb->addInstr(instr);
        }
        // 添加cmp lhs rhs指令，追加到bb之后
        user_ptr_t lhs = lhsList.back();
        user_ptr_t rhs = rhsList.back();
        // 检查类型是否匹配
        assert(
            lhs->getType()->getOpType() == rhs->getType()->getOpType(),
            "type of lhs and rhs should be the same");
        // cmp n lhs rhs
        cmpInstr = make_cmp(lhs, rhs, lhs->getType()->getOpType(), opTermToType(op));
        // 添加br指令，追加到bb之后
        brInstr = make_br(cmpInstr);
    }
    bb->addInstr(cmpInstr);
    bb->addInstr(brInstr);
    // 返回BetaStmtRetInfo
    return BetaStmtRetInfo{{bb}, {}, {brInstr}, {brInstr}};
}

// AndExpr -> AndExpr `&&` RelExpr | RelExpr
StmtRetInfo RSCVisitor::visitAndExpr(pst_node_ptr_t node)
{
    size_t childNum = node->childrenCount();
    pst_node_ptr_t child = node->firstChild();
    if (childNum == 1 && child->data.symbol == "RelExpr")
    {
        // AndExpr -> RelExpr
        // 直接返回RelExpr的结果即可
        return visitRelExpr(child);
    }
    assert(childNum == 2, "and expr should have 1 or 2 child(ren)");
    // 处理与运算
    auto lhsInfo = visitAndExpr(child);
    assert(is_beta(lhsInfo), "lhs of and expr return info should be beta");
    auto rhsInfo = visitAndExpr(node->getChildAt(1));
    assert(is_beta(rhsInfo), "rhs of and expr return info should be beta");
    BetaStmtRetInfo lhs = get_beta(lhsInfo);
    BetaStmtRetInfo rhs = get_beta(rhsInfo);
    // 对于与运算，需要将lhs和rhs的bb连接起来
    // 将lhs的真出口tc连接到rhs的入口bb entry
    // 将lhs的假出口fc和rhs的假出口合并后返回
    // 将rhs的真出口作为返回值的真出口直接返回
    BetaStmtRetInfo retInfo{lhs.list, {}, rhs.trueList, lhs.falseList};
    // 将lhs的真出口tc连接到rhs的入口bb entry
    const block_ptr_t bb = dynamic_pointer_cast<InstrBlock>(rhs.list.front());
    for (br_ptr_t br : lhs.trueList)
    {
        br->setTrueTarget(bb);
    }
    retInfo.list.splice(retInfo.list.end(), rhs.list);                // 将rhs的指令列表追加到retInfo的指令列表之后
    retInfo.falseList.splice(retInfo.falseList.end(), rhs.falseList); // 将rhs的假出口追加到retInfo的假出口之后
    return retInfo;
}

// OrExpr -> OrExpr `||` AndExpr | AndExpr
StmtRetInfo RSCVisitor::visitOrExpr(pst_node_ptr_t node)
{
    size_t childNum = node->childrenCount();
    pst_node_ptr_t child = node->firstChild();
    if (childNum == 1 && child->data.symbol == "AndExpr")
    {
        // OrExpr -> AndExpr
        // 直接返回AndExpr的结果即可
        return visitAndExpr(child);
    }
    assert(childNum == 2, "or expr should have 1 or 2 child(ren)");
    // 处理或运算
    auto lhsInfo = visitOrExpr(child);
    assert(is_beta(lhsInfo), "lhs of or expr return info should be beta");
    auto rhsInfo = visitOrExpr(node->getChildAt(1));
    assert(is_beta(rhsInfo), "rhs of or expr return info should be beta");
    BetaStmtRetInfo lhs = get_beta(lhsInfo);
    BetaStmtRetInfo rhs = get_beta(rhsInfo);
    // 对于或运算，需要将lhs和rhs的bb连接起来
    // 将lhs的假出口fc连接到rhs的入口bb entry
    // 将lhs的真出口tc和rhs的真出口合并后返回
    // 将rhs的假出口作为返回值的假出口直接返回
    BetaStmtRetInfo retInfo{lhs.list, {}, lhs.trueList, rhs.falseList};
    // 将lhs的假出口fc连接到rhs的入口bb entry
    const block_ptr_t bb = dynamic_pointer_cast<InstrBlock>(rhs.list.front());
    for (br_ptr_t br : lhs.falseList)
    {
        br->setFalseTarget(bb);
    }
    retInfo.list.splice(retInfo.list.end(), rhs.list);             // 将rhs的指令列表追加到retInfo的指令列表之后
    retInfo.trueList.splice(retInfo.trueList.end(), rhs.trueList); // 将rhs的真出口追加到retInfo的真出口之后
    return retInfo;
}

// BoolExpr -> OrExpr
StmtRetInfo RSCVisitor::visitBoolExpr(pst_node_ptr_t node)
{
    // 仅有一种情况，即OrExpr，直接返回即可
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
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
#include "utils/log.h"

#define DEBUG_LEVEL 0

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
program_ptr_t RSCVisitor::visitProgram(pst_node_ptr_t node)
{
    info << "RSCVisitor: visiting program..." << std::endl;
    assert(
        node->data.symbol == "Program",
        format("Expected node Program, but got $ instead.", node->data.symbol));
    context.symbolTable.newScope();

    program_ptr_t program = make_program();

    pst_children_t children = node->getChildren();

    for (pst_node_ptr_t child : children)
    {
        if (child->data.symbol == "VarDeclStmt")
        {
            ret_info_t globalInfo = visitVarDeclStmt(child, true);
            program->addGlobal(globalInfo.getValue());
        }
        else if (child->data.symbol == "FuncDeclStmt")
        {
            visitFuncDeclStmt(child);
        }
        else if (child->data.symbol == "FuncDef")
        {
            ret_info_t funcInfo = visitFuncDef(child);
            try
            {
                info << "RSCVisitor: function \n"
                     << funcInfo.getValue()->dump() << std::endl;
            }
            catch (const std::exception &e)
            {
                error << e.what() << std::endl;
            }
            program->addFunc(funcInfo.getValue());
        }
        else
        {
            error << "unknown symbol " << child->data.symbol << std::endl;
            exit(1);
        }
    }

    context.symbolTable.popScope();
    return program;
}

// VarDeclStmt -> VarDecl ;
ret_info_t RSCVisitor::visitVarDeclStmt(pst_node_ptr_t node, bool global)
{
    info << "RSCVisitor: visiting var decl stmt..." << std::endl;
    assert(
        node->data.symbol == "VarDeclStmt",
        format("Expected node VarDeclStmt, but got $ instead.", node->data.symbol));
    return visitVarDecl(node->firstChild(), global);
}

// VarDecl -> { VarDef }
ret_info_t RSCVisitor::visitVarDecl(pst_node_ptr_t node, bool global)
{
    info << "RSCVisitor: visiting var decl..." << std::endl;
    assert(
        node->data.symbol == "VarDecl",
        format("Expected node a, but got $ instead.", node->data.symbol));
    ret_info_t retInfo;
    // 获取VarDef列表，遍历处理即可
    pst_children_t children = node->getChildren();

    for (pst_node_ptr_t child : children)
    {
        ret_info_t varInfo = visitVarDef(child, global);
        // 将VarDef的结果追加到list之后
        retInfo.appendInstrList(varInfo.instrList);
        retInfo.setValue(varInfo.getValue());
    }
    return retInfo;
}

// VarDef -> ident : Type [ InitVal ]
ret_info_t RSCVisitor::visitVarDef(pst_node_ptr_t node, bool global)
{
    info << "RSCVisitor: visiting var def..." << std::endl;
    assert(
        node->data.symbol == "VarDef",
        format("Expected node VarDef, but got $ instead.", node->data.symbol));
    ret_info_t retInfo;
    // 获取ident和Type
    // 暂时不考虑数组，因此剩余子节点暂时不处理
    std::string identStr = node->getChildAt(0)->data.symbol;
    std::string typeStr = node->getChildAt(1)->firstChild()->data.symbol;

    type_ptr_t type = make_prime_type(PrimitiveType::str2type(typeStr));
    user_ptr_t value = nullptr;

    pst_node_ptr_t initValNode = node->getChildAt(3);
    user_ptr_t initVal = nullptr;
    if (initValNode->hasChild())
    {
        ret_info_t initValInfo = visitInitVal(initValNode->firstChild());
        initVal = initValInfo.getValue();
    }

    if (global)
    {
        assert(
            initVal != nullptr,
            format("global variable $ must be initialized", identStr));
        const_val_ptr_t init = cast_const(initVal);
        value = context.symbolTable.registerGlobal(identStr, type, init);
        retInfo.addInstr(value);
    }
    else
    {
        value = context.symbolTable.registerAlloca(identStr, type);
        retInfo.addInstr(value);
        if (initVal != nullptr)
        {
            store_ptr_t storeInstr = make_store(initVal, value);
            retInfo.addInstr(storeInstr);
        }
    }

    retInfo.setValue(value);

    return retInfo;
}

// InitVal -> Expr | { Expr }
ret_info_t RSCVisitor::visitInitVal(pst_node_ptr_t node)
{
    info << "RSCVisitor: visiting init val..." << std::endl;
    assert(
        node->data.symbol == "InitVal",
        format("Expected node InitVal, but got $ instead.", node->data.symbol));
    // 暂时不考虑数组，因此只需要处理Expr即可
    if (node->data.product_opt.value().second[0] == "Expr")
    {
        // InitVal -> Expr
        return visitExpr(node->firstChild());
    }
    else
    {
        // InitVal -> { Expr }
        // 暂时不考虑数组，因此不需要处理
        warn << "array is not supported yet" << std::endl;
    }
    return ret_info_t();
}

// FuncDeclStmt -> FuncDecl ;
ret_info_t RSCVisitor::visitFuncDeclStmt(pst_node_ptr_t node)
{
    info << "RSCVisitor: visiting func decl stmt..." << std::endl;
    assert(
        node->data.symbol == "FuncDeclStmt",
        format("Expected node FuncDeclStmt, but got $ instead.", node->data.symbol));
    return visitFuncDecl(node->firstChild());
}

// FuncDecl -> ident ( [ParamList] ) [: Type] ;
ret_info_t RSCVisitor::visitFuncDecl(pst_node_ptr_t node)
{
    info << "RSCVisitor: visiting func decl..." << std::endl;
    assert(
        node->data.symbol == "FuncDecl",
        format("Expected node FuncDecl, but got $ instead.", node->data.symbol));
    pst_children_t children = node->getChildren();

    std::string identStr = children[0]->data.symbol;

    pst_node_ptr_t paramsNode = children[1];
    pst_node_ptr_t retTypeNode = children[2];

    // 先根据函数名和返回值类型注册函数
    prim_ptr_t retType = nullptr;
    if (retTypeNode->hasChild())
    {
        // FuncDecl -> ident ( [ParamList] ) : Type ;
        std::string typeStr = retTypeNode->firstChild()->firstChild()->data.symbol;
        retType = make_prime_type(PrimitiveType::str2type(typeStr));
    }
    else
    {
        // FuncDecl -> ident ( [ParamList] ) ;
        // 没有指定返回类型，则默认为void
        retType = make_prime_type(PrimitiveType::VOID);
    }

    assert(
        context.functionTable.find(identStr) == nullptr,
        format("function $ has already been declared", identStr));
    func_ptr_t func = context.functionTable.registerFunction(identStr, retType);

    // 解析参数列表，为函数添加参数
    if (paramsNode->hasChild())
    {
        ret_info_t paramsInfo = visitParamList(paramsNode->firstChild());
        func->addParams(paramsInfo.valueList);
    }

    return ret_info_t{instr_list_t{func}}.setValue(func);
}

// FuncDef -> FuncDecl Block
ret_info_t RSCVisitor::visitFuncDef(pst_node_ptr_t node)
{
    info << "RSCVisitor: visiting func def..." << std::endl;
    assert(
        node->data.symbol == "FuncDef",
        format("Expected node FuncDef, but got $ instead.", node->data.symbol));
    // 首先检查函数是否已经声明
    // 如果已经声明，则需要对比函数声明和函数定义是否一致
    // 如果没有声明，则直接注册函数
    pst_node_ptr_t funcDeclNode = node->firstChild();

    const std::string &identStr = funcDeclNode->firstChild()->data.symbol;
    func_ptr_t func = context.functionTable.find(identStr);

    if (func == nullptr)
    {
        // 函数未声明，直接注册函数
        ret_info_t funcInfo = visitFuncDecl(funcDeclNode);

        func = dynamic_pointer_cast<FuncInstr>(funcInfo.getValue());
    }
    else
    {
        // 函数已声明，检查函数声明和函数定义是否一致
        std::string retTypeStr = "void";
        pst_node_ptr_t retTypeNode = funcDeclNode->getChildAt(2);
        if (retTypeNode->hasChild())
        {
            // 函数声明有返回值类型
            retTypeStr = retTypeNode->firstChild()->firstChild()->data.symbol;
        }
        pst_node_ptr_t paramsNode = funcDeclNode->getChildAt(1);
        if (paramsNode->hasChild())
        {
            // 函数声明有参数
            // 检查函数定义是否有参数
            ret_info_t paramsInfo = visitParamList(paramsNode->firstChild());

            assert(
                func->matchRetType(PrimitiveType::str2type(retTypeStr)) &&
                    func->matchArgs(paramsInfo.valueList),
                "function declaration and definition mismatch.");
        }
    }

    // 新建作用域，访问解析函数体
    context.symbolTable.newScope();

    // 注册函数参数
    for (user_ptr_t param : func->getParams())
    {
        context.symbolTable.registerAlloca(param->getName(), param->getType());
    }

    pst_node_ptr_t blockNode = node->getChildAt(1);
    ret_info_t blockInfo = visitBlock(blockNode);

    // 下面开始构建函数定义

    // 首先构建函数的入口基本块 entry basic block
    block_ptr_t entryBB = make_block("func_entry");
    // 将函数体内变量的内存分配指令追加到entry之后
    std::vector<user_ptr_t> vars = context.symbolTable.popScope();
    for (user_ptr_t var : vars)
    {
        entryBB->addInstr(var);
    }
    // 为函数返回值分配内存
    alloca_ptr_t retAlloc = make_alloca("retval", func->getRetType());
    entryBB->addInstr(retAlloc);

    // 构建函数的出口基本块 exit basic block
    block_ptr_t exitBB = make_block("func_exit");
    // 为函数返回值赋值
    load_ptr_t loadInstr = make_load(retAlloc);
    ret_ptr_t retInstr = make_ret(loadInstr);
    exitBB->addInstr(loadInstr);
    exitBB->addInstr(retInstr);

    // 构建函数的主体基本块 main basic block
    block_ptr_t mainBB = make_block("func_body");
    mainBB->addInstrList(blockInfo.instrList);
    entryBB->addInstr(make_br(mainBB));

    // 将entry, main, exit基本块追加到函数中
    func->addBlock(entryBB);
    func->addBlock(mainBB);
    func->addBlock(exitBB);

    return ret_info_t{std::list<user_ptr_t>{func}}.setValue(func);
}

// FuncCall -> ident ( [ArgList] )
ret_info_t RSCVisitor::visitFuncCall(pst_node_ptr_t node)
{
    info << "RSCVisitor: visiting func call..." << std::endl;
    assert(
        node->data.symbol == "FuncCall",
        format("Expected node FuncCall, but got $ instead.", node->data.symbol));
    // 获取ident和ArgList
    std::string ident = node->getChildAt(0)->data.symbol;
    if (ident == "print")
    {
        // print函数是内置函数，暂时不生成IR
        warn << "print function is not supported yet" << std::endl;
        return ret_info_t();
    }
    // 查看函数是否已经声明
    func_ptr_t func = context.functionTable.find(ident);
    assert(func != nullptr, "function has not been declared");

    pst_node_ptr_t argListNode = node->getChildAt(1);

    ret_info_t retInfo;
    // 生成call指令并追加到list之后
    call_ptr_t callInstr = make_call(func);
    // 解析ArgList
    if (argListNode->hasChild())
    {
        ret_info_t argListInfo = visitArgList(argListNode->firstChild());
        retInfo.appendInstrList(argListInfo.instrList);
        instr_list_t &list = argListInfo.valueList;
        // 做一步类型转换，将list转换为list<user_ptr_t>
        std::list<user_ptr_t> args(list.begin(), list.end());

        // 检查参数列表是否匹配
        assert(func->matchArgs(args), "function call and declaration mismatch");

        callInstr->addArgs(args);
    }
    retInfo.addInstr(callInstr);
    retInfo.setValue(callInstr);

    return retInfo;
}

// ArgList -> { Expr }
ret_info_t RSCVisitor::visitArgList(pst_node_ptr_t node)
{
    info << "RSCVisitor: visiting arg list..." << std::endl;
    assert(
        node->data.symbol == "ArgList",
        format("Expected node ArgList, but got $ instead.", node->data.symbol));
    // 获取Expr列表，遍历处理即可
    // 将Expr的结果追加到list之后
    // 将每个Expr的结果列表单独放到context中传递
    ret_info_t retInfo;
    pst_children_t children = node->getChildren();

    for (pst_node_ptr_t child : children)
    {
        auto exprInfo = visitExpr(child);
        retInfo.appendInstrList(exprInfo.instrList);
        retInfo.addValue(exprInfo.getValue());
    }

    return retInfo;
}

// ParamList -> { Param }
ret_info_t RSCVisitor::visitParamList(pst_node_ptr_t node)
{
    info << "RSCVisitor: visiting param list..." << std::endl;
    assert(
        node->data.symbol == "ParamList",
        format("Expected node ParamList, but got $ instead.", node->data.symbol));
    // 获取Param列表，遍历处理即可
    ret_info_t retInfo;
    pst_children_t children = node->getChildren();

    for (pst_node_ptr_t child : children)
    {
        ret_info_t paramInfo = visitParam(child);
        // 这里只保留值，不保留指令
        retInfo.addValue(paramInfo.getValue());
    }

    return retInfo;
}

// Param -> ident : Type
ret_info_t RSCVisitor::visitParam(pst_node_ptr_t node)
{
    info << "RSCVisitor: visiting param..." << std::endl;
    assert(
        node->data.symbol == "Param",
        format("Expected node Param, but got $ instead.", node->data.symbol));
    // 获取ident和Type
    // 暂时不考虑数组，因此剩余子节点暂时不处理
    std::string identStr = node->getChildAt(0)->data.symbol;
    std::string typeStr = node->getChildAt(1)->firstChild()->data.symbol;

    type_ptr_t type = make_prime_type(PrimitiveType::str2type(typeStr));
    alloca_ptr_t value = make_alloca(identStr, type);

    return ret_info_t().setValue(value); // 空指令列表，值为value
}

/**
 * Stmt -> Assignment `;`
 *     |   VarDeclStmt
 *     |   `if` `(` BoolExpr `)` Stmt [ `else` Stmt ]
 *     |   `while` `(` BoolExpr `)` Stmt
 *     |   `for` `(` [ VarDecl | Assignment ] `;` [ BoolExpr ] `;` [ Assignment ] `)` Stmt
 *     |   `break` `;`
 *     |   `continue` `;`
 *     |   `print` Expr `;`
 *     |   `return` [ Expr ] `;`
 *     |   [ Expr ] `;`
 *     |   Block
 *     ;
 */
ret_info_t RSCVisitor::visitStmt(pst_node_ptr_t node)
{
    info << "RSCVisitor: visiting stmt..." << std::endl;
    assert(
        node->data.symbol == "Stmt",
        format("Expected node Stmt, but got $ instead.", node->data.symbol));
    // 根据产生式的第一个符号判断Stmt的类型
    product_t &product = node->data.product_opt.value();
    symbol_t &firstSym = product.second[0];

    if (firstSym == "Assignment")
    {
        return visitAssignment(node->firstChild());
    }
    else if (firstSym == "VarDeclStmt")
    {
        return visitVarDeclStmt(node->firstChild());
    }
    else if (firstSym == "if")
    {
        return visitIfStmt(node);
    }
    else if (firstSym == "while")
    {
        return visitWhileStmt(node);
    }
    else if (firstSym == "for")
    {
        return visitForStmt(node);
    }
    else if (firstSym == "break")
    {
        return visitBreakStmt(node);
    }
    else if (firstSym == "continue")
    {
        return visitContinueStmt(node);
    }
    else if (firstSym == "print")
    {
        warn << "print function is not supported yet" << std::endl;
        // return visitPrintStmt(node);
    }
    else if (firstSym == "return")
    {
        return visitReturnStmt(node);
    }
    else if (firstSym == "Block")
    {
        return visitBlock(node->firstChild());
    }
    else // [Expr]
    {
        pst_node_ptr_t opt = node->firstChild();
        if (opt->hasChild())
        {
            // Stmt -> [ Expr ] ; (Optional)
            return visitExprStmt(opt->firstChild());
        }
    }
    return ret_info_t();
}

// Block -> { Stmt }
ret_info_t RSCVisitor::visitBlock(pst_node_ptr_t node)
{
    info << "RSCVisitor: visiting block..." << std::endl;
    assert(
        node->data.symbol == "Block",
        format("Expected node Block, but got $ instead.", node->data.symbol));
    // 遍历Stmt列表，处理每个Stmt
    // 将每个Stmt的结果追加到list之后
    ret_info_t retInfo;
    pst_children_t children = node->getChildren();
    for (pst_node_ptr_t child : children)
    {
        ret_info_t stmtInfo = visitStmt(child);
        // 将Stmt的所有信息整合到retInfo中
        retInfo.unionInfo(stmtInfo);
        // 尝试回填fall through
        if (retInfo.hasFallThrough())
        {
            label_ptr_t fallThroughLabel = make_label("fall");
            retInfo.addInstr(fallThroughLabel);
            retInfo.backpatch(JR_FALL_THROUGH, fallThroughLabel);
        }
    }
    return retInfo;
}

// Assignment -> ident = Expr ;
ret_info_t RSCVisitor::visitAssignment(pst_node_ptr_t node)
{
    info << "RSCVisitor: visiting assignment..." << std::endl;
    assert(
        node->data.symbol == "Assignment",
        format("Expected node Assignment, but got $ instead.", node->data.symbol));
    // 获取ident和Expr
    std::string identStr = node->firstChild()->firstChild()->data.symbol;
    ret_info_t exprInfo = visitExpr(node->getChildAt(1));

    // 获取ident对应的内存分配指令
    user_ptr_t alloc = context.symbolTable.find(identStr);
    assert(
        alloc != nullptr,
        format("variable $ has not been declared", identStr));

    // 生成store指令并追加到list之后
    store_ptr_t storeInstr = make_store(exprInfo.getValue(), alloc);
    exprInfo.addInstr(storeInstr);
    exprInfo.setValue(storeInstr);

    return exprInfo;
}

// IfStmt -> if ( BoolExpr ) Stmt [else Stmt]
ret_info_t RSCVisitor::visitIfStmt(pst_node_ptr_t node)
{
    info << "RSCVisitor: visiting if stmt..." << std::endl;
    assert(
        node->data.symbol == "Stmt",
        format("Expected node Stmt, but got $ instead.", node->data.symbol));
    context.symbolTable.newScope();
    // 直接将BoolExpr返回值作为If返回info
    ret_info_t retInfo = visitBoolExpr(node->getChildAt(0));
    // 将BoolExpr内容追加到list之后
    retInfo.appendInstrList(retInfo.instrList);

    // 获取Stmt
    auto stmtInfo = visitStmt(node->getChildAt(1));

    // 将Stmt内容整合为基本块
    block_ptr_t stmtBB = make_block("if_stmt");
    stmtBB->addInstrList(stmtInfo.instrList);
    retInfo.addInstr(stmtBB);

    // 绑定条件的真出口
    retInfo.backpatch(JR_TRUE_EXIT, stmtBB);

    // 获取else Stmt
    pst_node_ptr_t elseStmtNode = node->getChildAt(2);
    if (elseStmtNode->hasChild())
    {
        ret_info_t elseStmtInfo = visitStmt(elseStmtNode->firstChild());

        // 将else Stmt内容整合为基本块
        block_ptr_t elseStmtBB = make_block("else_stmt");
        elseStmtBB->addInstrList(elseStmtInfo.instrList);
        retInfo.addInstr(elseStmtBB);

        // 绑定条件的假出口
        retInfo.backpatch(JR_FALSE_EXIT, elseStmtBB);
    }
    else
    {
        // 修改条件的假出口为fall through
        retInfo.shiftReason(JR_FALSE_EXIT, JR_FALL_THROUGH);
    }

    context.symbolTable.popScope();

    return retInfo;
}

// WhileStmt -> while ( BoolExpr ) Stmt
ret_info_t RSCVisitor::visitWhileStmt(pst_node_ptr_t node)
{
    info << "RSCVisitor: visiting while stmt..." << std::endl;
    assert(
        node->data.symbol == "Stmt",
        format("Expected node Stmt, but got $ instead.", node->data.symbol));
    context.symbolTable.newScope();
    ret_info_t retInfo;

    // 获取BoolExpr，构造cond基本块
    ret_info_t boolExprInfo = visitBoolExpr(node->getChildAt(0));
    block_ptr_t condBB = make_block("while_cond");
    condBB->addInstrList(boolExprInfo.instrList);
    retInfo.addInstr(condBB);

    // 获取Stmt，构造stmt基本块
    ret_info_t stmtInfo = visitStmt(node->getChildAt(1));
    block_ptr_t stmtBB = make_block("while_stmt");
    stmtBB->addInstrList(stmtInfo.instrList);
    retInfo.addInstr(stmtBB);

    // 回填BoolExpr的真出口，指向stmtBB
    boolExprInfo.backpatch(JR_TRUE_EXIT, stmtBB);

    // BoolExpr的假出口，指向while循环的出口（Fall through）
    retInfo.appendJmpList(boolExprInfo.getTargetsOf(JR_FALSE_EXIT), JR_FALL_THROUGH);

    // 回填Stmt中的break，指向while循环的出口（Fall through）
    retInfo.appendJmpList(stmtInfo.getTargetsOf(JR_BREAK_OUT), JR_FALL_THROUGH);

    // 回填Stmt中的continue，指向entryBB
    stmtInfo.backpatch(JR_CONTINUE, condBB);

    // while最后跳转到condBB
    retInfo.addInstr(make_jmp(condBB));

    context.symbolTable.popScope();
    return retInfo;
}

// ForStmt -> for ( [VarDecl|Assignment] ; [BoolExpr] ; [Assignment] ) Stmt
ret_info_t RSCVisitor::visitForStmt(pst_node_ptr_t node)
{
    info << "RSCVisitor: visiting for stmt..." << std::endl;
    assert(
        node->data.symbol == "Stmt" || node->data.symbol == "Assignment",
        format("Expected node Stmt, but got $ instead.", node->data.symbol));
    context.symbolTable.newScope();
    ret_info_t retInfo;

    block_ptr_t initBB = make_block("for_init");
    block_ptr_t condBB = make_block("for_cond");
    block_ptr_t stmtBB = make_block("for_stmt");
    block_ptr_t lastBB = make_block("for_last");

    // 解析VarDecl|Assignment
    pst_node_ptr_t varDeclOrAssignNode = node->getChildAt(0);
    if (varDeclOrAssignNode->hasChild())
    {
        pst_node_ptr_t initNode = varDeclOrAssignNode->firstChild();
        if (initNode->data.symbol == "VarDecl")
        {
            // VarDecl
            ret_info_t varDeclInfo = visitVarDecl(initNode);
            initBB->addInstrList(varDeclInfo.instrList);
        }
        else
        {
            // Assignment
            ret_info_t assignInfo = visitAssignment(initNode);
            initBB->addInstrList(assignInfo.instrList);
        }
        retInfo.addInstr(initBB);
    }

    // 解析BoolExpr
    pst_node_ptr_t boolExprNode = node->getChildAt(1);
    if (boolExprNode->hasChild())
    {
        ret_info_t boolExprInfo = visitBoolExpr(boolExprNode->firstChild());
        condBB->addInstrList(boolExprInfo.instrList);
        retInfo.addInstr(condBB);

        // 回填BoolExpr的真出口，指向stmtBB
        boolExprInfo.backpatch(JR_TRUE_EXIT, stmtBB);

        // BoolExpr的假出口，指向for循环的出口（Fall through）
        retInfo.appendJmpList(boolExprInfo.getTargetsOf(JR_FALSE_EXIT), JR_FALL_THROUGH);
    }

    // 解析Assignment
    pst_node_ptr_t assignNode = node->getChildAt(2);
    if (assignNode->hasChild())
    {
        ret_info_t assignInfo = visitAssignment(assignNode->firstChild());
        lastBB->addInstrList(assignInfo.instrList);
    }
    lastBB->addInstr(make_jmp(boolExprNode->hasChild() ? condBB : stmtBB));
    retInfo.addInstr(lastBB);

    // 解析Stmt
    pst_node_ptr_t stmtNode = node->getChildAt(3);
    ret_info_t stmtInfo = visitStmt(stmtNode);
    stmtBB->addInstrList(stmtInfo.instrList);
    retInfo.addInstr(stmtBB);

    // 处理Stmt中的break，指向for循环的出口（Fall through）
    retInfo.appendJmpList(stmtInfo.getTargetsOf(JR_BREAK_OUT), JR_FALL_THROUGH);

    // 回填Stmt中的continue，指向lastBB
    stmtInfo.backpatch(JR_CONTINUE, lastBB);

    return retInfo;
}

// Stmt -> break ;
ret_info_t RSCVisitor::visitBreakStmt(pst_node_ptr_t node)
{
    info << "RSCVisitor: visiting break stmt..." << std::endl;
    assert(
        node->data.symbol == "Stmt",
        format("Expected node Stmt, but got $ instead.", node->data.symbol));
    jmp_ptr_t instr = make_jmp();
    ret_info_t retInfo{{instr}};
    retInfo.addJmpTarget(instr, JR_BREAK_OUT);
    return retInfo;
}

// Stmt -> continue ;
ret_info_t RSCVisitor::visitContinueStmt(pst_node_ptr_t node)
{
    info << "RSCVisitor: visiting continue stmt..." << std::endl;
    assert(
        node->data.symbol == "Stmt",
        format("Expected node Stmt, but got $ instead.", node->data.symbol));
    jmp_ptr_t instr = make_jmp();
    ret_info_t retInfo{{instr}};
    retInfo.addJmpTarget(instr, JR_CONTINUE);
    return retInfo;
}

// Stmt -> return [Expr] ;
ret_info_t RSCVisitor::visitReturnStmt(pst_node_ptr_t node)
{
    info << "RSCVisitor: visiting return stmt..." << std::endl;
    assert(
        node->data.symbol == "Stmt",
        format("Expected node Stmt, but got $ instead.", node->data.symbol));
    size_t childSum = node->childrenCount();
    assert(childSum == 1, "return stmt should have 1 child(ren)");

    pst_node_ptr_t exprNode = node->firstChild(); // Optional
    if (exprNode->hasChild())
    {
        // Optional -> Expr ;
        ret_info_t exprInfo = visitExpr(exprNode->firstChild());
        // 生成ret指令并追加到list之后
        ret_ptr_t retInstr = make_ret(exprInfo.getValue());
        exprInfo.addInstr(retInstr);

        return exprInfo;
    }
    else
    {
        // Optional -> ;
        return ret_info_t{instr_list_t{make_ret()}};
    }
}

// Stmt -> Expr ;
ret_info_t RSCVisitor::visitExprStmt(pst_node_ptr_t node)
{
    info << "RSCVisitor: visiting expr stmt..." << std::endl;
    assert(
        node->data.symbol == "Stmt",
        format("Expected node Stmt, but got $ instead.", node->data.symbol));
    size_t childSum = node->childrenCount();
    assert(childSum == 1, "expr stmt should have 1 child(ren)");

    pst_node_ptr_t child = node->firstChild(); // Optional
    if (child->childrenCount() == 1)
    {
        // Optional -> Expr ;
        return visitExpr(child->firstChild());
    }

    return ret_info_t(); // 不可能执行到这里
}

// UnaryExpr -> ( `+` | `-` | `!` ) UnaryExpr | Factor
ret_info_t RSCVisitor::visitUnaryExpr(pst_node_ptr_t node)
{
    info << "RSCVisitor: visiting unary expr..." << std::endl;
    assert(
        node->data.symbol == "UnaryExpr",
        format("Expected node UnaryExpr, but got $ instead.", node->data.symbol));
    pst_node_ptr_t child = node->firstChild();
    if (child->data.symbol == "Factor")
    {
        // Factor
        return visitFactor(child);
    }
    // ( `+` | `-` | `!` ) UnaryExpr
    ret_info_t exprInfo = visitUnaryExpr(child);

    // 目前仅支持 - 操作，+ 和 ! 操作暂不支持
    auto op = getProSymAt(node, 0); // + | - | !
    assert(op == "-", "only support unary minus");

    user_ptr_t operand = exprInfo.getValue();
    // 生成neg指令并追加到list之后
    neg_ptr_t instr = make_neg(operand, operand->getType()->getOpType());
    exprInfo.addInstr(instr);
    exprInfo.setValue(instr);

    return exprInfo;
}

// MulExpr -> MulExpr ( `*` | `/` | `%` ) UnaryExpr | UnaryExpr
ret_info_t RSCVisitor::visitMulExpr(pst_node_ptr_t node)
{
    info << "RSCVisitor: visiting mul expr..." << std::endl;
    assert(
        node->data.symbol == "MulExpr",
        format("Expected node MulExpr, but got $ instead.", node->data.symbol));
    pst_node_ptr_t child = node->firstChild();
    if (child->data.symbol == "UnaryExpr")
    {
        // UnaryExpr
        return visitUnaryExpr(child);
    }

    assert(
        child->data.symbol == "MulExpr",
        format(
            "first child of mul expr should be mul expr or unary expr, but got $",
            child->data.symbol));
    // MulExpr ( `*` | `/` | `%` ) UnaryExpr
    assert(
        node->childrenCount() == 2,
        format("mul expr should have 1 or 2 child(ren), but got $", node->childrenCount()));

    auto lhsInfo = visitMulExpr(child);
    auto rhsInfo = visitUnaryExpr(node->getChildAt(1));

    auto op = getProSymAt(node, 1); // * | / | %

    user_ptr_t lhs = lhsInfo.getValue();
    user_ptr_t rhs = rhsInfo.getValue();

    // 生成相关指令并追加到list之后
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

    ret_info_t retInfo{list_concat(lhsInfo.instrList, rhsInfo.instrList)};
    retInfo.addInstr(instr);
    retInfo.setValue(instr);

    return retInfo;
}

// Expr -> Expr ( `+` | `-` ) MulExpr | MulExpr
ret_info_t RSCVisitor::visitExpr(pst_node_ptr_t node)
{
    info << "RSCVisitor: visiting expr..." << std::endl;
    assert(
        node->data.symbol == "Expr",
        format("Expected node Expr, but got $ instead.", node->data.symbol));
    pst_node_ptr_t child = node->firstChild();
    if (child->data.symbol == "MulExpr")
    {
        // MulExpr
        return visitMulExpr(child);
    }

    assert(
        child->data.symbol == "Expr",
        format(
            "first child of expr should be expr or mul expr, but got $",
            child->data.symbol));

    // Expr ( `+` | `-` ) MulExpr
    assert(
        node->childrenCount() == 2,
        format("expr should have 1 or 2 child(ren), but got $", node->childrenCount()));

    auto lhsInfo = visitExpr(child);
    auto rhsInfo = visitMulExpr(node->getChildAt(1));

    auto op = getProSymAt(node, 1); // + | -

    user_ptr_t lhs = lhsInfo.getValue();
    user_ptr_t rhs = rhsInfo.getValue();

    // 生成相关指令并追加到list之后
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

    ret_info_t retInfo{list_concat(lhsInfo.instrList, rhsInfo.instrList)};

    retInfo.addInstr(instr);
    retInfo.setValue(instr);

    return retInfo;
}

// RelExpr -> Expr ( `<` | `<=` | `>` | `>=` | `==` | `!=` ) Expr | Expr
ret_info_t RSCVisitor::visitRelExpr(pst_node_ptr_t node)
{
    info << "RSCVisitor: visiting rel expr..." << std::endl;
    assert(
        node->data.symbol == "RelExpr",
        format("Expected node RelExpr, but got $ instead.", node->data.symbol));
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
        ret_info_t exprInfo = visitExpr(node->firstChild());

        // 将原来的指令列表包装成BasicBlock
        bb->addInstrList(exprInfo.instrList);

        // 添加cmp expr 0指令，追加到bb之后
        user_ptr_t lhs = exprInfo.getValue();
        user_ptr_t rhs = nullptr;

        prim_ptr_t type = dynamic_pointer_cast<PrimitiveType>(lhs->getType());
        if (is_prim_int(type))
        {
            rhs = make_const_int(0);
        }
        else if (is_prim_real(type))
        {
            rhs = make_const_real(0.0);
        }
        else if (is_prim_bool(type))
        {
            rhs = make_const_bool(false);
        }
        else if (is_prim_char(type))
        {
            rhs = make_const_char('\0');
        }
        else
        {
            error << "Unsupported type" << std::endl;
            exit(1);
        }

        // cmp n lhs 0
        cmpInstr = make_cmp(lhs, rhs, lhs->getType()->getOpType(), CT_NE);
        // 添加br指令，追加到bb之后
        brInstr = make_br(cmpInstr);
    }
    else
    {
        assert(childNum == 2, "rel expr should have 1 or 2 child(ren)");

        ret_info_t lhsInfo = visitExpr(node->firstChild());
        ret_info_t rhsInfo = visitExpr(node->getChildAt(1));

        // 在产生式中获取具体的关系运算符
        auto op = getProSymAt(node, 1); // < | <= | > | >= | == | !=

        // 将原来的指令列表包装成BasicBlock
        bb->addInstrList(lhsInfo.instrList);
        bb->addInstrList(rhsInfo.instrList);

        // 添加cmp lhs rhs指令，追加到bb之后
        user_ptr_t lhs = lhsInfo.getValue();
        user_ptr_t rhs = rhsInfo.getValue();

        // cmp n lhs rhs
        cmpInstr = make_cmp(lhs, rhs, lhs->getType()->getOpType(), opTermToType(op));
        // 添加br指令，追加到bb之后
        brInstr = make_br(cmpInstr);
    }

    bb->addInstr(cmpInstr);
    bb->addInstr(brInstr);

    // 返回StmtRetInfo
    return ret_info_t{{bb}}.addBrTarget(brInstr);
}

// AndExpr -> AndExpr `&&` RelExpr | RelExpr
ret_info_t RSCVisitor::visitAndExpr(pst_node_ptr_t node)
{
    info << "RSCVisitor: visiting and expr..." << std::endl;
    assert(
        node->data.symbol == "AndExpr",
        format("Expected node AndExpr, but got $ instead.", node->data.symbol));
    size_t childNum = node->childrenCount();
    pst_node_ptr_t child = node->firstChild();

    if (childNum == 1 && child->data.symbol == "RelExpr")
    {
        // AndExpr -> RelExpr
        // 直接返回RelExpr的结果即可
        return visitRelExpr(child);
    }

    assert(
        child->data.symbol == "AndExpr",
        format("first child of and expr should be AndExpr or RelExpr, but got $", child->data.symbol));

    assert(
        childNum == 2,
        format("AndExpr should have 1 or 2 child(ren), but got $", childNum));

    // 处理与运算
    auto lhsInfo = visitAndExpr(child);
    auto rhsInfo = visitRelExpr(node->getChildAt(1));

    ret_info_t lhs = lhsInfo;
    ret_info_t rhs = rhsInfo;

    // 对于与运算，需要将lhs和rhs的bb连接起来
    ret_info_t retInfo{list_concat(lhs.instrList, rhs.instrList)};

    retInfo.appendTrueList(rhs.getTargetsOf(JR_TRUE_EXIT));   // 将rhs的真出口作为retInfo的真出口
    retInfo.appendFalseList(lhs.getTargetsOf(JR_FALSE_EXIT)); // 将lhs的假出口作为retInfo的假出口
    retInfo.appendFalseList(rhs.getTargetsOf(JR_FALSE_EXIT)); // 将rhs的假出口作为retInfo的假出口

    // 将lhs的真出口tc连接到rhs的入口bb entry
    block_ptr_t bb = make_block("and_rhs");
    bb->addInstrList(rhs.instrList);
    lhs.backpatch(JR_TRUE_EXIT, bb);

    return retInfo;
}

// OrExpr -> OrExpr `||` AndExpr | AndExpr
ret_info_t RSCVisitor::visitOrExpr(pst_node_ptr_t node)
{
    info << "RSCVisitor: visiting or expr..." << std::endl;
    assert(
        node->data.symbol == "OrExpr",
        format("Expected node OrExpr, but got $ instead.", node->data.symbol));
    size_t childNum = node->childrenCount();
    pst_node_ptr_t child = node->firstChild();
    if (childNum == 1 && child->data.symbol == "AndExpr")
    {
        // OrExpr -> AndExpr
        // 直接返回AndExpr的结果即可
        return visitAndExpr(child);
    }

    assert(
        child->data.symbol == "OrExpr",
        format("first child of or expr should be OrExpr or AndExpr, but got $", child->data.symbol));

    assert(childNum == 2, "or expr should have 1 or 2 child(ren)");

    // 处理或运算
    auto lhsInfo = visitOrExpr(child);
    auto rhsInfo = visitAndExpr(node->getChildAt(1));

    ret_info_t lhs = lhsInfo;
    ret_info_t rhs = rhsInfo;

    // 对于或运算，需要将lhs和rhs的bb连接起来
    ret_info_t retInfo{list_concat(lhs.instrList, rhs.instrList)};
    retInfo.appendTrueList(lhs.getTargetsOf(JR_TRUE_EXIT));   // 将lhs的真出口作为retInfo的真出口
    retInfo.appendTrueList(rhs.getTargetsOf(JR_TRUE_EXIT));   // 将rhs的真出口作为retInfo的真出口
    retInfo.appendFalseList(rhs.getTargetsOf(JR_FALSE_EXIT)); // 将rhs的假出口作为retInfo的假出口

    // 将lhs的假出口fc连接到rhs的入口bb entry
    block_ptr_t bb = make_block("or_rhs");
    bb->addInstrList(rhs.instrList);
    lhs.backpatch(JR_FALSE_EXIT, bb);

    return retInfo;
}

// BoolExpr -> OrExpr
ret_info_t RSCVisitor::visitBoolExpr(pst_node_ptr_t node)
{
    info << "RSCVisitor: visiting bool expr..." << std::endl;
    assert(
        node->data.symbol == "BoolExpr",
        format("Expected node BoolExpr, but got $ instead.", node->data.symbol));
    // 仅有一种情况，即OrExpr，直接返回即可
    return visitOrExpr(node->firstChild());
}

// Factor -> (Expr), LVal, Literal, FuncCall
ret_info_t RSCVisitor::visitFactor(pst_node_ptr_t node)
{
    info << "RSCVisitor: visiting factor..." << std::endl;
    assert(
        node->data.symbol == "Factor",
        format("Expected node Factor, but got $ instead.", node->data.symbol));
    pst_node_ptr_t child = node->firstChild();

    // 先处理(Expr), LVal, FuncCall
    if (child->data.symbol == "Expr")
    {
        // (Expr)
        ret_info_t retInfo = visitExpr(child);
        assert(retInfo.hasValue(), "expr should have value");
        return retInfo;
    }
    else if (child->data.symbol == "LVal")
    {
        // LVal
        ret_info_t retInfo = visitLVal(child);
        assert(retInfo.hasValue(), "lval should have value");
        return retInfo;
    }
    else if (child->data.symbol == "FuncCall")
    {
        // FuncCall
        return visitFuncCall(child);
    }
    else
    {
        // Literal
        ret_info_t retInfo = visitLiteral(child);
        assert(retInfo.hasValue(), "literal should have value");
        return retInfo;
    }
}

ret_info_t RSCVisitor::visitLVal(pst_node_ptr_t node)
{
    info << "RSCVisitor: visiting lval..." << std::endl;
    assert(
        node->data.symbol == "LVal",
        format("Expected node LVal, but got $ instead.", node->data.symbol));
    user_ptr_t instr = context.symbolTable.find(node->firstChild()->data.symbol);

    assert(
        instr != nullptr,
        format("undefined symbol: $", node->firstChild()->data.symbol));

    return ret_info_t{instr_list_t{instr}}.setValue(instr);
}

// Factor -> int, real, char, string, true, false
ret_info_t RSCVisitor::visitLiteral(pst_node_ptr_t node)
{
    info << "RSCVisitor: visiting literal..." << std::endl;
    // 如果node自身是终结符节点，则可能是char, string, true, false
    // 先处理true, false，剩下的就是char, string
    if (_is_term_node(node))
    {
        if (node->data.symbol == "true")
        {
            // true
            auto instr = make_const_bool(true);
            return ret_info_t{instr_list_t{instr}}.setValue(instr);
        }
        else if (node->data.symbol == "false")
        {
            // false
            auto instr = make_const_bool(false);
            return ret_info_t{instr_list_t{instr}}.setValue(instr);
        }
        else if (node->data.symbol[0] == '\'')
        {
            // char
            auto instr = make_const_char(node->data.symbol[1]);
            return ret_info_t{instr_list_t{instr}}.setValue(instr);
        }
        else if (node->data.symbol[0] == '\"')
        {
            // string
            auto instr = make_const_str(node->data.symbol.substr(1, node->data.symbol.size() - 2));
            return ret_info_t{instr_list_t{instr}}.setValue(instr);
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
            return VisitorRetInfo{instr_list_t{instr}}.setValue(instr);
        }
        else if (node->data.symbol == "RealLiteral")
        {
            // real
            auto instr = make_const_real(std::stod(node->firstChild()->data.symbol));
            return VisitorRetInfo{instr_list_t{instr}}.setValue(instr);
        }
        else
        {
            // error
            error << "unknown literal" << std::endl;
            exit(1);
        }
    }
}
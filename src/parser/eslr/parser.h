/**
 * @file elr/parser.h
 * @author Zhenjie Wei (2024108@bjtu.edu.cn)
 * @brief Extended SLR(1) Parser
 * @date 2023-06-05
 *
 * @copyright Copyright (c) 2023
 *
 */

/**
 * 本文件实现扩展的SLR1文法分析程序
 * 该程序的主要功能是根据文法定义文件的声明将输入的token序列转换为抽象语法树
 * 相比于SLR1文法分析程序，该程序的主要改动在于：
 * 1、支持扩展的文法定义，即除了EBNF文法和映射之外，还支持语义动作和优先级与结合性的解析
 * 2、支持扩展的文法分析，即除了SLR1文法分析之外，还支持基于优先级解决移进-规约冲突的LR1文法分析
 * 3、支持扩展的语法树构建，即除了构建CST之外，还支持构建RST和AST，便于后续翻译步骤
 */

#include "common/tree/pst.h"
#include "common/gram/slr1.h"
#include "utils/view/ctx_view.h"

class ExtendedSimpleLR1Parser
{
    SLR1Grammar grammar;
    pst_tree_ptr_t cst; // Concrete Syntax Tree
    pst_tree_ptr_t rst; // Reduced Syntax Tree
    pst_tree_ptr_t ast; // Abstract Syntax Tree
    pair<string, string> descAction(const action_t &act) const;

public:
    ExtendedSimpleLR1Parser(SLR1Grammar &grammar) : grammar(grammar)
    {
        cst = pst_tree_t::createNode(TERMINAL, SYM_END, 0, 0);
    }
    bool parse(vector<token> &input, const ContextViewer &code);
    pst_tree_ptr_t reduceCST();
    pst_tree_ptr_t refactorRST();
    pst_tree_ptr_t getCST() { return cst; }
    pst_tree_ptr_t getRST() { return rst; }
    pst_tree_ptr_t getAST() { return ast; }
};

typedef ExtendedSimpleLR1Parser ESLR1Parser;
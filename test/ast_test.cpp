/**
 * @file ast_test.cpp
 * @author Zhenjie Wei (2024108@bjtu.edu.cn)
 * @brief Test AST
 * @date 2023-05-06
 *
 * @copyright Copyright (c) 2023
 *
 */

#include "test.h"
#include "common/ast.h"

void astTest()
{
    ast_tree_ptr tree = ast_node::createNode(NON_TERM, "FUNCTION_DECL", 0, 0);
    ast_node_ptr ndIdent = ast_node::createNode(NON_TERM, "IDENTIFIER_NODE", 0, 0);
    ast_node_ptr ndType = ast_node::createNode(NON_TERM, "TYPE", 0, 0);
    ast_node_ptr ndArgs = ast_node::createNode(NON_TERM, "DECL_ARGUMENTS", 0, 0);
    ast_node_ptr ndBody = ast_node::createNode(NON_TERM, "BODY", 0, 0);
    ast_node_ptr ndBind = ast_node::createNode(NON_TERM, "BIND_EXPR", 0, 0);
    ast_node_ptr ndBlock = ast_node::createNode(NON_TERM, "BLOCK", 0, 0);
    ast_node_ptr ndRet = ast_node::createNode(NON_TERM, "RETURN_EXPR", 0, 0);
    ast_node_ptr ndPlus = ast_node::createNode(NON_TERM, "PLUS_EXPR", 0, 0);
    ast_node_ptr ndIdent1 = ast_node::createNode(NON_TERM, "IDENTIFIER_NODE", 0, 0);
    ast_node_ptr ndIdent2 = ast_node::createNode(NON_TERM, "IDENTIFIER_NODE", 0, 0);

    *tree << ndIdent << ndType << ndArgs << ndBody;
    *ndIdent << ast_node::createNode(TERMINAL, "add", 0, 0);
    *ndType << ast_node::createNode(NON_TERM, "TYPE", 0, 0);
    (*ndType)[0] << ast_node::createNode(TERMINAL, "int", 0, 0);
    *ndArgs << ast_node::createNode(NON_TERM, "PARM_DECL", 0, 0);
    (*ndArgs)[0] << ast_node::createNode(NON_TERM, "IDENTIFIER_NODE", 0, 0);
    (*ndArgs)[0][0] << ast_node::createNode(TERMINAL, "a", 0, 0);
    (*ndArgs)[0] << ast_node::createNode(NON_TERM, "TYPE", 0, 0);
    (*ndArgs)[0][1] << ast_node::createNode(TERMINAL, "int", 0, 0);
    *ndArgs << ast_node::createNode(NON_TERM, "PARM_DECL", 0, 0);
    (*ndArgs)[1] << ast_node::createNode(NON_TERM, "IDENTIFIER_NODE", 0, 0);
    (*ndArgs)[1][0] << ast_node::createNode(TERMINAL, "b", 0, 0);
    (*ndArgs)[1] << ast_node::createNode(NON_TERM, "TYPE", 0, 0);
    (*ndArgs)[1][1] << ast_node::createNode(TERMINAL, "int", 0, 0);
    *ndBody << ndBind;
    *ndBind << ndBlock << ndRet;
    *ndBlock << ast_node::createNode(TERMINAL, "{", 0, 0);
    *ndRet << ndPlus;
    *ndPlus << ndIdent1 << ndIdent2;
    *ndIdent1 << ast_node::createNode(TERMINAL, "a", 0, 0);
    *ndIdent2 << ast_node::createNode(TERMINAL, "b", 0, 0);
    *ndBody << ast_node::createNode(TERMINAL, "}", 0, 0);
    ndBody->print();
    tree->print();
}
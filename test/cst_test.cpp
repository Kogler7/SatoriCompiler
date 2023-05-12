/**
 * @file ast_test.cpp
 * @author Zhenjie Wei (2024108@bjtu.edu.cn)
 * @brief Test AST
 * @date 2023-05-06
 *
 * @copyright Copyright (c) 2023
 *
 */

#include "common/tree/cst.h"

void cstTest()
{
    cst_tree_ptr tree = cst_node::createNode(NON_TERM, "FUNCTION_DECL", 0, 0);
    cst_node_ptr ndIdent = cst_node::createNode(NON_TERM, "IDENTIFIER_NODE", 0, 0);
    cst_node_ptr ndType = cst_node::createNode(NON_TERM, "TYPE", 0, 0);
    cst_node_ptr ndArgs = cst_node::createNode(NON_TERM, "DECL_ARGUMENTS", 0, 0);
    cst_node_ptr ndBody = cst_node::createNode(NON_TERM, "BODY", 0, 0);
    cst_node_ptr ndBind = cst_node::createNode(NON_TERM, "BIND_EXPR", 0, 0);
    cst_node_ptr ndBlock = cst_node::createNode(NON_TERM, "BLOCK", 0, 0);
    cst_node_ptr ndRet = cst_node::createNode(NON_TERM, "RETURN_EXPR", 0, 0);
    cst_node_ptr ndPlus = cst_node::createNode(NON_TERM, "PLUS_EXPR", 0, 0);
    cst_node_ptr ndIdent1 = cst_node::createNode(NON_TERM, "IDENTIFIER_NODE", 0, 0);
    cst_node_ptr ndIdent2 = cst_node::createNode(NON_TERM, "IDENTIFIER_NODE", 0, 0);

    *tree << ndIdent << ndType << ndArgs << ndBody;
    *ndIdent << cst_node::createNode(TERMINAL, "add", 0, 0);
    *ndType << cst_node::createNode(NON_TERM, "TYPE", 0, 0);
    (*ndType)[0] << cst_node::createNode(TERMINAL, "int", 0, 0);
    *ndArgs << cst_node::createNode(NON_TERM, "PARM_DECL", 0, 0);
    (*ndArgs)[0] << cst_node::createNode(NON_TERM, "IDENTIFIER_NODE", 0, 0);
    (*ndArgs)[0][0] << cst_node::createNode(TERMINAL, "a", 0, 0);
    (*ndArgs)[0] << cst_node::createNode(NON_TERM, "TYPE", 0, 0);
    (*ndArgs)[0][1] << cst_node::createNode(TERMINAL, "int", 0, 0);
    *ndArgs << cst_node::createNode(NON_TERM, "PARM_DECL", 0, 0);
    (*ndArgs)[1] << cst_node::createNode(NON_TERM, "IDENTIFIER_NODE", 0, 0);
    (*ndArgs)[1][0] << cst_node::createNode(TERMINAL, "b", 0, 0);
    (*ndArgs)[1] << cst_node::createNode(NON_TERM, "TYPE", 0, 0);
    (*ndArgs)[1][1] << cst_node::createNode(TERMINAL, "int", 0, 0);
    *ndBody << ndBind;
    *ndBind << ndBlock << ndRet;
    *ndBlock << cst_node::createNode(TERMINAL, "{", 0, 0);
    *ndRet << ndPlus;
    *ndPlus << ndIdent1 << ndIdent2;
    *ndIdent1 << cst_node::createNode(TERMINAL, "a", 0, 0);
    *ndIdent2 << cst_node::createNode(TERMINAL, "b", 0, 0);
    *ndBody << cst_node::createNode(TERMINAL, "}", 0, 0);
    ndBody->print();
    tree->print();
}
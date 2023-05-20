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
    cst_tree_ptr_t tree = cst_node_t::createNode(NON_TERM, "FUNCTION_DECL", 0, 0);
    cst_node_ptr_t ndIdent = cst_node_t::createNode(NON_TERM, "IDENTIFIER_NODE", 0, 0);
    cst_node_ptr_t ndType = cst_node_t::createNode(NON_TERM, "TYPE", 0, 0);
    cst_node_ptr_t ndArgs = cst_node_t::createNode(NON_TERM, "DECL_ARGUMENTS", 0, 0);
    cst_node_ptr_t ndBody = cst_node_t::createNode(NON_TERM, "BODY", 0, 0);
    cst_node_ptr_t ndBind = cst_node_t::createNode(NON_TERM, "BIND_EXPR", 0, 0);
    cst_node_ptr_t ndBlock = cst_node_t::createNode(NON_TERM, "BLOCK", 0, 0);
    cst_node_ptr_t ndRet = cst_node_t::createNode(NON_TERM, "RETURN_EXPR", 0, 0);
    cst_node_ptr_t ndPlus = cst_node_t::createNode(NON_TERM, "PLUS_EXPR", 0, 0);
    cst_node_ptr_t ndIdent1 = cst_node_t::createNode(NON_TERM, "IDENTIFIER_NODE", 0, 0);
    cst_node_ptr_t ndIdent2 = cst_node_t::createNode(NON_TERM, "IDENTIFIER_NODE", 0, 0);

    *tree << ndIdent << ndType << ndArgs << ndBody;
    *ndIdent << cst_node_t::createNode(TERMINAL, "add", 0, 0);
    *ndType << cst_node_t::createNode(NON_TERM, "TYPE", 0, 0);
    (*ndType)[0] << cst_node_t::createNode(TERMINAL, "int", 0, 0);
    *ndArgs << cst_node_t::createNode(NON_TERM, "PARM_DECL", 0, 0);
    (*ndArgs)[0] << cst_node_t::createNode(NON_TERM, "IDENTIFIER_NODE", 0, 0);
    (*ndArgs)[0][0] << cst_node_t::createNode(TERMINAL, "a", 0, 0);
    (*ndArgs)[0] << cst_node_t::createNode(NON_TERM, "TYPE", 0, 0);
    (*ndArgs)[0][1] << cst_node_t::createNode(TERMINAL, "int", 0, 0);
    *ndArgs << cst_node_t::createNode(NON_TERM, "PARM_DECL", 0, 0);
    (*ndArgs)[1] << cst_node_t::createNode(NON_TERM, "IDENTIFIER_NODE", 0, 0);
    (*ndArgs)[1][0] << cst_node_t::createNode(TERMINAL, "b", 0, 0);
    (*ndArgs)[1] << cst_node_t::createNode(NON_TERM, "TYPE", 0, 0);
    (*ndArgs)[1][1] << cst_node_t::createNode(TERMINAL, "int", 0, 0);
    *ndBody << ndBind;
    *ndBind << ndBlock << ndRet;
    *ndBlock << cst_node_t::createNode(TERMINAL, "{", 0, 0);
    *ndRet << ndPlus;
    *ndPlus << ndIdent1 << ndIdent2;
    *ndIdent1 << cst_node_t::createNode(TERMINAL, "a", 0, 0);
    *ndIdent2 << cst_node_t::createNode(TERMINAL, "b", 0, 0);
    *ndBody << cst_node_t::createNode(TERMINAL, "}", 0, 0);
    ndBody->print();
    tree->print();
}
/**
 * @file ast.h
 * @author Zhenjie Wei (2024108@bjtu.edu.cn)
 * @brief Abstract Syntax Tree
 * @date 2023-05-01
 *
 * @copyright Copyright (c) 2023
 *
 */

#pragma once

#include <map>
#include <set>
#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <algorithm>

using namespace std;

class AbstractSyntaxTreeNode;

typedef AbstractSyntaxTreeNode ast_node;
typedef AbstractSyntaxTreeNode ast_tree;
typedef shared_ptr<ast_node> ast_node_ptr;
typedef shared_ptr<ast_node> ast_tree_ptr;
typedef vector<ast_node_ptr> ast_childs;

enum node_type
{
    NON_TERM,
    TERMINAL
};

struct ast_node_data
{
    node_type type;
    string symbol;
    size_t line, col;
};

class AbstractSyntaxTreeNode : ast_childs
{
public:
    ast_node *parent;
    node_type type;
    string symbol;
    size_t line, col;
    
    AbstractSyntaxTreeNode(node_type typ, string sym, size_t ln, size_t co);

    static ast_node_ptr createNode(node_type type, const string &symbol, size_t line, size_t col);
    vector<ast_node>::reference operator[](size_t index);
    ast_node &operator<<(const ast_node_ptr node);
    size_t size() const;

    template <typename func_t>
    void foreach (func_t f) const;

    template <typename func_t>
    void traverse(func_t f) const;

    template <typename func_t>
    void traverse(func_t f, int &level, int &index);

    void dump();
    void dumpSave(const string &file);
};
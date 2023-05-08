/**
 * @file tree.h
 * @author Zhenjie Wei (2024108@bjtu.edu.cn)
 * @brief Abstract Tree
 * @date 2023-05-08
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

template <typename data_t>
class AbstractTreeNode;

template <typename data_t>
using tree_node = AbstractTreeNode<data_t>;

template <typename data_t>
using tree_node_ptr = shared_ptr<AbstractTreeNode<data_t>>;

template <typename data_t>
using tree_childs = vector<tree_node_ptr<data_t>>;

template <typename data_t>
class AbstractTreeNode : tree_childs<data_t>
{
public:
    tree_node<data_t> *parent;
    data_t data;

    AbstractTreeNode(data_t data);

    static tree_node_ptr<data_t> createNode(data_t data);
    tree_childs<data_t>::reference operator[](size_t index);
    tree_node<data_t> &operator<<(const tree_node_ptr<data_t> node);
    size_t size() const;

    virtual string desc() const;

    template <typename func_t>
    void foreach(func_t f) const;

    template <typename func_t>
    void traverse(func_t f) const;

    template <typename func_t>
    void traverse(func_t f, int &level, int &index);

    template <typename func_t>
    void postorder(func_t f) const;

    template <typename func_t>
    void postorder(func_t f, int &level, int &index);

    string dump();
    void print();
};
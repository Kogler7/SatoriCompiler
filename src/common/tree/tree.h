/**
 * @file utils/tree.h
 * @author Zhenjie Wei (2024108@bjtu.edu.cn)
 * @brief Abstract Tree
 * @date 2023-05-08
 *
 * @copyright Copyright (c) 2023
 *
 */

#pragma once

#include "utils/log.h"

#include <map>
#include <set>
#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>

template <typename data_t>
class AbstractTreeNode;

template <typename data_t>
using tree_node_t = AbstractTreeNode<data_t>;

template <typename data_t>
using tree_node_ptr_t = std::shared_ptr<AbstractTreeNode<data_t>>;

template <typename data_t>
using tree_children_t = std::vector<tree_node_ptr_t<data_t>>;

template <typename data_t>
class AbstractTreeNode : public tree_children_t<data_t>
{
public:
    tree_node_t<data_t> *parent;
    data_t data;

    AbstractTreeNode(data_t data) : data(data), parent(nullptr) {}

    static tree_node_ptr_t<data_t> createNode(data_t data)
    {
        return std::make_shared<tree_node_t<data_t>>(data);
    }

    size_t find(data_t data) const
    {
        auto it = find_if(
            tree_children_t<data_t>::begin(), tree_children_t<data_t>::end(),
            [=](tree_node_ptr_t<data_t> node)
            { return node->data == data; });
        return it == tree_children_t<data_t>::end() ? -1 : it - tree_children_t<data_t>::begin();
    }

    template <typename func_t>
    size_t find(func_t cmp) const
    {
        auto it = find_if(tree_children_t<data_t>::begin(), tree_children_t<data_t>::end(), cmp);
        return it == tree_children_t<data_t>::end() ? -1 : it - tree_children_t<data_t>::begin();
    }

    void reverseChildren()
    {
        reverse(tree_children_t<data_t>::begin(), tree_children_t<data_t>::end());
    }

    tree_node_t<data_t> &operator[](size_t index) const
    {
        const auto &child = this->at(index);
        return static_cast<tree_node_t<data_t> &>(*child);
    }

    tree_node_ptr_t<data_t> get_child_ptr(size_t index) const
    {
        tree_node_ptr_t<data_t> child = this->at(index);
        return child;
    }

    tree_node_t<data_t> &operator<<(const tree_node_ptr_t<data_t> node)
    {
        node->parent = this;
        this->push_back(node);
        return static_cast<tree_node_t<data_t> &>(*this);
    }

    size_t size() const
    {
        return tree_children_t<data_t>::size();
    }

    virtual std::string descData() const
    {
        return "";
    }

    template <typename func_t>
    void foreach (func_t f) const
    {
        auto nodeF = [=](tree_children_t<data_t>::const_reference ref)
        {
            f(*ref);
        };
        for_each(tree_children_t<data_t>::begin(), tree_children_t<data_t>::end(), nodeF);
    }

    template <typename func_t>
    void traverse(func_t f) const
    {
        f(*this);
        foreach (
            [=](tree_node_t<data_t> &ref)
            { ref.traverse(f); })
            ;
    }

    template <typename func_t>
    void traverse(func_t f, int &level, int &index)
    {
        tree_node_t<data_t> &self = *this;
        f(self);
        level++;
        int tmpIdx = index++;
        index = 0;
        foreach (
            [&](tree_node_t<data_t> &ref)
            {
                ref.traverse(f, level, index);
                index++;
            })
            ;
        level--;
        index = tmpIdx;
    }

    template <typename func_t>
    void postorder(func_t f) const
    {
        foreach (
            [=](tree_node_t<data_t> &ref)
            { ref.postorder(f); })
            ;
        f(*this);
    }

    template <typename func_t>
    void postorder(func_t f, int &level, int &index)
    {
        tree_node_t<data_t> &self = *this;
        level++;
        int tmpIdx = index++;
        index = 0;
        foreach (
            [&](tree_node_t<data_t> &ref)
            {
                ref.postorder(f, level, index);
                index++;
            })
            ;
        level--;
        index = tmpIdx;
        f(self);
    }

    std::string dumpTree()
    {
        std::stringstream ss;
        std::vector<bool> visible;
        int level = 0;
        int index = 0;
        traverse(
            [&](tree_node_t<data_t> &node)
            {
                if (visible.size() <= level)
                    visible.push_back(true);
                if (level > 0)
                {
                    if (node.parent == nullptr)
                    {
                        warn << "DumpTree: Node <" << node.descData() << "> has no parent!" << std::endl;
                    }
                    else if (index == node.parent->size() - 1)
                    {
                        visible[level - 1] = false;
                    }
                }
                auto getHead = [=](int level) -> std::string
                {
                    int i = 0;
                    std::string ret;
                    while (i < level - 1)
                    {
                        if (visible[i])
                            ret += "|  ";
                        else
                            ret += "   ";
                        i++;
                    }
                    if (level > 0)
                        ret += "|--";
                    return ret;
                };
                ss << getHead(level);
                ss << node.descData();
                ss << std::endl;
                if (level > 0)
                    for (int i = level; i < visible.size(); i++)
                        visible[i] = true;
            },
            level, index);
        return ss.str();
    }

    void print()
    {
        std::cout << this->dumpTree();
    }
};
/**
 * @file tree.cpp
 * @author Zhenjie Wei (2024108@bjtu.edu.cn)
 * @brief Abstract Syntax Tree
 * @date 2023-05-08
 *
 * @copyright Copyright (c) 2023
 *
 */

#include "tree.h"
#include "sstream"

template <typename data_t>
AbstractTreeNode<data_t>::AbstractTreeNode(data_t data) : data(data)
{
    parent = nullptr;
}

template <typename data_t>
tree_node_ptr<data_t> AbstractTreeNode<data_t>::createNode(data_t data)
{
    return make_shared<tree_node<data_t>>(data);
}

template <typename data_t>
tree_childs<data_t>::reference AbstractTreeNode<data_t>::operator[](size_t index)
{
    return *(tree_childs<data_t>::operator[](index));
}

template <typename data_t>
tree_node<data_t> &AbstractTreeNode<data_t>::operator<<(const tree_node_ptr<data_t> node)
{
    node->parent = this;
    push_back(node);
    return *this;
}

template <typename data_t>
size_t AbstractTreeNode<data_t>::size() const
{
    return tree_childs<data_t>::size();
}

template <typename data_t>
template <typename func_t>
void AbstractTreeNode<data_t>::foreach (func_t f) const
{
    auto nodeF = [=](tree_childs<data_t>::const_reference ref)
    {
        f(*ref);
    };
    for_each(tree_childs<data_t>::begin(), tree_childs<data_t>::end(), nodeF);
}

template <typename data_t>
template <typename func_t>
void AbstractTreeNode<data_t>::traverse(func_t f) const
{
    f(*this);
    foreach (
        [=](tree_node<data_t> &ref)
        { ref.traverse(f); })
        ;
}

template <typename data_t>
template <typename func_t>
void AbstractTreeNode<data_t>::traverse(func_t f, int &level, int &index)
{
    tree_node<data_t> &self = *this;
    f(self);
    level++;
    int tmpIdx = index++;
    index = 0;
    foreach (
        [&](tree_node<data_t> &ref)
        {
            ref.traverse(f, level, index);
            index++;
        })
        ;
    level--;
    index = tmpIdx;
}

template <typename data_t>
template <typename func_t>
void AbstractTreeNode<data_t>::postorder(func_t f) const
{
    foreach (
        [=](tree_node<data_t> &ref)
        { ref.postorder(f); })
        ;
    f(*this);
}

template <typename data_t>
template <typename func_t>
void AbstractTreeNode<data_t>::postorder(func_t f, int &level, int &index)
{
    tree_node<data_t> &self = *this;
    level++;
    int tmpIdx = index++;
    index = 0;
    foreach (
        [&](tree_node<data_t> &ref)
        {
            ref.postorder(f, level, index);
            index++;
        })
        ;
    level--;
    index = tmpIdx;
    f(self);
}

template <typename data_t>
string AbstractTreeNode<data_t>::dump()
{
    stringstream ss;
    vector<bool> visible;
    int level = 0;
    int index = 0;
    traverse(
        [&](tree_node<data_t> &node)
        {
            if (visible.size() <= level)
                visible.push_back(true);
            if (level > 0 && index == node.parent->size() - 1)
                visible[level - 1] = false;
            auto getHead = [=](int level) -> string
            {
                int i = 0;
                string ret;
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
            ss << desc();
            ss << endl;
            if (level > 0)
                for (int i = level; i < visible.size(); i++)
                    visible[i] = true;
        },
        level, index);
}

template <typename data_t>
void AbstractTreeNode<data_t>::print()
{
    cout << dump();
}
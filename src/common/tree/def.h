/**
 * @file def.h
 * @author Zhenjie Wei (2024108@bjtu.edu.cn)
 * @brief Abstract Syntax Tree Definition
 * @date 2023-05-06
 *
 * @copyright Copyright (c) 2023
 *
 */

#pragma once

#define foreachChilds(f) for (auto &child : *this) f(child)

#define DEF_TREE_NODE(node_ident, node_desc, node_type, child_cnt) \
    class node_ident : public ASTNode                               \
    {                                                               \
    public:                                                         \
        node_ident()                                                \
        {                                                           \
            setProperty("type", node_type);                         \
        }                                                           \
        node_ident(initializer_list<PropertyContainer::value_type> list) \
            : ASTNode(list)                                         \
        {                                                           \
            setProperty("type", node_type);                         \
        }                                                           \
        node_ident(const node_ident &node) : ASTNode(node)          \
        {                                                           \
            setProperty("type", node_type);                         \
        }                                                           \
        node_ident(node_ident &&node) : ASTNode(move(node))         \
        {                                                           \
            setProperty("type", node_type);                         \
        }                                                           \
        node_ident &operator=(const node_ident &node)               \
        {                                                           \
            ASTNode::operator=(node);                               \
            return *this;                                           \
        }                                                           \
        node_ident &operator=(node_ident &&node)                    \
        {                                                           \
            ASTNode::operator=(move(node));                         \
            return *this;                                           \
        }                                                           \
        virtual ~node_ident() {}                                    \
        virtual string toString() const                             \
        {                                                           \
            return node_desc;                                       \
        }                                                           \
        virtual size_t childCount() const                           \
        {                                                           \
            return child_cnt;                                       \
        }                                                           \
    }
/**
 * @file sem.h
 * @author Zhenjie Wei (2024108@bjtu.edu.cn)
 * @brief Semantic Definition
 * @date 2023-06-01
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#include "utils/log.h"
#include "utils/apply.h"
#include <map>
#include <string>
#include <functional>

#pragma once

class ASTNode
{
public:
    virtual void act() const { info << 1; }
    ASTNode() {}
};

extern std::map<std::string, std::function<std::shared_ptr<ASTNode>()>> astNodeFactories;

#define MAKE_NODE(ident) astNodeFactories[ident]()

#define DECL_NODE(node_ident, node_desc, node_act, child_cnt) \
    class node_ident : public ASTNode                         \
    {                                                         \
    public:                                                   \
        node_ident() : ASTNode()                              \
        {                                                     \
        }                                                     \
        void act() const override                             \
        {                                                     \
            node_act;                                         \
        }                                                     \
    };

#define _REG_NODE(ident) \
    astNodeFactories.insert(std::make_pair(#ident, []() -> std::shared_ptr<ident> { return std::make_shared<ident>(); }));

#define REG_NODES(x, ...)                              \
    void reg_nodes()                                   \
    {                                                  \
        _EXPAND(_APPLY_X(x, , _REG_NODE, __VA_ARGS__)) \
    }
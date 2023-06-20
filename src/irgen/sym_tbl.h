/**
 * @file sym_tbl.h
 * @author Zhenjie Wei (2024108@bjtu.edu.cn)
 * @brief Symbol Table
 * @date 2023-06-07
 *
 * @copyright Copyright (c) 2023
 *
 */

#pragma once

#include "instr.h"
#include "utils/nested_map.h"

#include <stack>

using scope_t = NestedMap<std::string, user_ptr_t>;
using scope_ptr_t = std::shared_ptr<scope_t>;

class SymbolTable
{
    std::stack<scope_ptr_t> tableStk;

public:
    void newScope();
    std::list<user_ptr_t> popScope();
    [[nodiscard]] user_ptr_t find(const std::string &name);
    [[nodiscard]] user_ptr_t registerAlloca(const std::string &name, type_ptr_t type);
    [[nodiscard]] user_ptr_t registerGlobal(const std::string &name, type_ptr_t type, const_val_ptr_t init);
};

class StaticDataTable
{
    std::map<std::string, user_ptr_t> table;
};

class FunctionTable
{
    std::map<std::string, func_ptr_t> table;

public:
    void insert(const std::string &name, func_ptr_t func) { table.insert({name, func}); }
    [[nodiscard]] func_ptr_t find(const std::string &name) const
    {
        auto it = table.find(name);
        if (it != table.end())
        {
            return it->second;
        }
        return nullptr;
    }
    [[nodiscard]] bool has(const std::string &name) const { return table.find(name) != table.end(); }
    [[nodiscard]] func_ptr_t registerFunction(const std::string &name, prim_ptr_t type)
    {
        func_ptr_t func = make_func(name, type);
        table.insert({name, func});
        return func;
    }
};
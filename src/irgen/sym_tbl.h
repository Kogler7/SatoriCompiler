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

using scope_t = NestedMap<std::string, alloc_ptr_t>;
using scope_ptr_t = std::shared_ptr<scope_t>;

class SymbolTable
{
    std::stack<scope_ptr_t> tableStk;

public:
    void newScope();
    std::vector<alloc_ptr_t> popScope();
    alloc_ptr_t find(const std::string &name);
};

class StaticDataTable
{
    std::map<std::string, alloc_ptr_t> table;
};
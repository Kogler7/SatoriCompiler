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

#include <stack>

#include "ssa.h"
#include "utils/nested_map.h"

using scope_t = NestedMap<std::string, alloc_ptr_t>;
using scope_ptr_t = std::shared_ptr<scope_t>;

class SymbolTable
{
    std::stack<scope_ptr_t> tableStk;

public:
    void newScope();
    std::vector<alloc_ptr_t> popScope();
};
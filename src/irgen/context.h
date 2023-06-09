/**
 * @file context.h
 * @author Zhenjie Wei (2024108@bjtu.edu.cn)
 * @brief Semantic Analysis Context
 * @date 2023-06-08
 *
 * @copyright Copyright (c) 2023
 *
 */

#pragma once

#include <vector>

#include "ssa.h"
#include "sym_tbl.h"

class SemanticContext;

class SemanticContext
{
public:
    program_ptr_t programSsa;
    func_ptr_t funcSsa;
    alloc_ptr_t retVal;
    block_ptr_t entryBb;
    block_ptr_t exitBb;
    std::vector<alloc_ptr_t> allocSsaList;
	SymbolTable symbolTable;
	void clear();
};
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

#include "instr.h"
#include "sym_tbl.h"

#include <vector>

class Context;

class Context
{
public:
    program_ptr_t programSsa;
    std::list<alloc_ptr_t> allocList;
	SymbolTable symbolTable;
    FunctionTable functionTable;
	void clear();
};
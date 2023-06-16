/**
 * @file sym_tbl.cpp
 * @author Zhenjie Wei (2024108@bjtu.edu.cn)
 * @brief Symbol Table
 * @date 2023-06-07
 *
 * @copyright Copyright (c) 2023
 *
 */

#include "sym_tbl.h"
#include "utils/log.h"

void SymbolTable::newScope()
{
	if (tableStk.empty())
	{
		tableStk.emplace(std::make_shared<scope_t>());
	}
	else
	{
		auto newTable = std::make_shared<scope_t>(tableStk.top());
		tableStk.emplace(std::make_shared<scope_t>(tableStk.top()));
	}
}

std::vector<alloc_ptr_t> SymbolTable::popScope()
{
	assert(!tableStk.empty());
	auto top = tableStk.top()->self();
	auto r = std::vector<alloc_ptr_t>();
	for (auto &[_, ssa] : top)
	{
		r.emplace_back(ssa);
	}
	tableStk.pop();
	return r;
}

alloc_ptr_t SymbolTable::find(const std::string &name)
{
	scope_ptr_t top = tableStk.top();
	if (top->has(name))
	{
		return top->at(name).value();
	}
	return nullptr;
}

alloc_ptr_t SymbolTable::registerSymbol(const std::string &name, type_ptr_t type)
{
	scope_ptr_t top = tableStk.top();
	assert(top != nullptr, "SymbolTable::registerSymbol: top is nullptr");
	assert(!top->has(name), "SymbolTable::registerSymbol: symbol already exists");
	alloc_ptr_t alloc = make_alloc(name, type);
	top->insert(name, alloc);
	return alloc;
}
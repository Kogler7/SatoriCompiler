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

void SymbolTable::newScope() {
	if (tableStk.empty()) {
		tableStk.emplace(std::make_shared<scope_t>());
	} else {
		auto newTable = std::make_shared<scope_t>(tableStk.top());
		tableStk.emplace(std::make_shared<scope_t>(tableStk.top()));
	}
}

std::vector<alloc_ptr_t> SymbolTable::popScope() {
	assert(!tableStk.empty());
	auto top = tableStk.top()->self();
	auto r = std::vector<alloc_ptr_t>();
	for (auto & [_, ssa]: top) {
		r.emplace_back(ssa);
	}
	tableStk.pop();
	return r;
}
/**
 * @file ret_info.h
 * @author Zhenjie Wei (2024108@bjtu.edu.cn)
 * @brief CST Visitor Return Info
 * @date 2023-06-08
 *
 * @copyright Copyright (c) 2023
 *
 */

#pragma once

#include "ssa.h"
#include "use.h"

#include <list>
#include <vector>
#include <variant>

struct AlphaStmtRetInfo
{
	std::list<user_ptr_t> list;
};

struct BetaStmtRetInfo
{
	//	todo
	std::vector<jump_ptr_t> fallSsaList;
	std::vector<branch_ptr_t> tBrSsaList;
	std::vector<branch_ptr_t> fBrSsaList;
};

struct ThetaStmtRetInfo
{
	std::list<user_ptr_t> list;
	jump_ptr_t jmpSsa;
};

using StmtRetInfo = std::variant<AlphaStmtRetInfo, BetaStmtRetInfo, ThetaStmtRetInfo>;
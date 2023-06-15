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

#include "instr.h"
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
	std::list<user_ptr_t> list;
	std::vector<jump_ptr_t> fallList;
	std::vector<br_ptr_t> trueList;
	std::vector<br_ptr_t> falseList;
};

struct ThetaStmtRetInfo
{
	std::list<user_ptr_t> list;
	jump_ptr_t jmpSsa;
};

using StmtRetInfo = std::variant<AlphaStmtRetInfo, BetaStmtRetInfo, ThetaStmtRetInfo>;
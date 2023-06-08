/**
 * @file ssa.h
 * @author Zhenjie Wei (2024108@bjtu.edu.cn)
 * @brief Static Single Assignment
 * @date 2023-06-07
 *
 * @copyright Copyright (c) 2023
 *
 */

#pragma once

#include "type.h"
#include "use.h"

class AllocSsa;
using alloc_ptr_t = std::shared_ptr<AllocSsa>;

class LoadSsa;
using load_ptr_t = std::shared_ptr<LoadSsa>;
class StoreSsa;
using store_ptr_t = std::shared_ptr<StoreSsa>;

class FuncSsa;
using func_ptr_t = std::shared_ptr<FuncSsa>;
class ReturnSsa;
using return_ptr_t = std::shared_ptr<ReturnSsa>;

class BranchSsa;
using branch_ptr_t = std::shared_ptr<BranchSsa>;
class JumpSsa;
using jump_ptr_t = std::shared_ptr<JumpSsa>;

class BlockSsa;
using block_ptr_t = std::shared_ptr<BlockSsa>;

class ProgramSsa;
using program_ptr_t = std::shared_ptr<ProgramSsa>;
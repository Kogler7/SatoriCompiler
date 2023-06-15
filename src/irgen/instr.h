/**
 * @file instr.h
 * @author Zhenjie Wei (2024108@bjtu.edu.cn)
 * @brief LLVM IR Instruction
 * @date 2023-06-07
 *
 * @copyright Copyright (c) 2023
 *
 */

#pragma once

#include "type.h"
#include "use.h"

#include <list>
#include <vector>

// Memory Access and Addressing Operations （内存访问和寻址操作）
class AllocInstr;
using alloc_ptr_t = std::shared_ptr<AllocInstr>;
#define make_alloc(name, type) std::make_shared<AllocInstr>(name, type)
class LoadInstr;
using load_ptr_t = std::shared_ptr<LoadInstr>;
#define make_load(from) std::make_shared<LoadInstr>(from)
class StoreInstr;
using store_ptr_t = std::shared_ptr<StoreInstr>;
#define make_store(from, to) std::make_shared<StoreInstr>(from, to)
class GEPInstr; // GetElementPtr
using gep_ptr_t = std::shared_ptr<GEPInstr>;

// Function Call Instructions （函数调用和返回指令）
class FuncInstr; // 抽象指令，由其他指令组合而成
using func_ptr_t = std::shared_ptr<FuncInstr>;
class CallInstr;
using call_ptr_t = std::shared_ptr<CallInstr>;

// Terminator Instructions （终端指令）
class RetInstr;
using ret_ptr_t = std::shared_ptr<RetInstr>;
class BrInstr;
using br_ptr_t = std::shared_ptr<BrInstr>;

// Unary Operations （一元运算）
class NegInstr;
using neg_ptr_t = std::shared_ptr<NegInstr>;

// Binary Operations （二元运算）
class AddInstr;
using add_ptr_t = std::shared_ptr<AddInstr>;
class SubInstr;
using sub_ptr_t = std::shared_ptr<SubInstr>;
class MulInstr;
using mul_ptr_t = std::shared_ptr<MulInstr>;
class DivInstr;
using div_ptr_t = std::shared_ptr<DivInstr>;
class RemInstr;
using rem_ptr_t = std::shared_ptr<RemInstr>;

// Block and Program
class InstrBlock;
using block_ptr_t = std::shared_ptr<InstrBlock>;
class Program;
using program_ptr_t = std::shared_ptr<Program>;

class AllocInstr : public User
{

public:
    AllocInstr(type_ptr_t type, std::string name = "") : User(type, name) {}
    ~AllocInstr() = default;
    std::string dump() const override;
};

class LoadInstr : public User
{
    Use from;

public:
    LoadInstr(value_ptr_t from, std::string name = "") : User(from->getType(), name), from(from, this) {}
    ~LoadInstr() = default;
    std::string dump() const override;
};

class StoreInstr : public User
{
    Use from;
    Use to;

public:
    StoreInstr(value_ptr_t from, value_ptr_t to) : User(), from(from, this), to(to, this) {}
    ~StoreInstr() = default;
    std::string dump() const override;
};

class FuncInstr : public User
{
    std::string name;
    std::vector<use_ptr_t> args;
    std::list<block_ptr_t> blocks;

public:
    FuncInstr(std::string name, std::vector<use_ptr_t> args) : name(std::move(name)), args(std::move(args)) {}
    ~FuncInstr() = default;
    std::string dump() const override;
};

class RetInstr : public User
{
    use_ptr_t value;

public:
    explicit RetInstr(use_ptr_t value) : value(std::move(value)) {}
    ~RetInstr() = default;
    std::string dump() const override;
};
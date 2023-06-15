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

enum OperandType
{
    OT_UNSIGNED = 'u',
    OT_SIGNED = 's',
    OT_FLOAT = 'f',
};

enum CompareType
{
    CT_EQ = 'e',
    CT_NE = 'n',
    CT_GT = 'g',
    CT_GE = 'G',
    CT_LT = 'l',
    CT_LE = 'L',
};

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
#define make_func(retType, args, blocks, name) std::make_shared<FuncInstr>(retType, args, blocks, name)

class CallInstr;
using call_ptr_t = std::shared_ptr<CallInstr>;
#define make_call(func) std::make_shared<CallInstr>(func)

// Terminator Instructions （终端指令）
class RetInstr;
using ret_ptr_t = std::shared_ptr<RetInstr>;
#define make_ret(retval) std::make_shared<RetInstr>(retval)

class BrInstr;
using br_ptr_t = std::shared_ptr<BrInstr>;
#define make_br(cond) std::make_shared<BrInstr>(cond)

class JmpInstr; // 无条件跳转
using jmp_ptr_t = std::shared_ptr<JmpInstr>;
#define make_jmp(target) std::make_shared<JmpInstr>(target)

// Unary Operations （一元运算）
class NegInstr;
using neg_ptr_t = std::shared_ptr<NegInstr>;
#define make_neg(from) std::make_shared<NegInstr>(from)

// Binary Operations （二元运算）
class AddInstr;
using add_ptr_t = std::shared_ptr<AddInstr>;
#define make_add(lhs, rhs, opType) std::make_shared<AddInstr>(lhs, rhs, opType)

class SubInstr;
using sub_ptr_t = std::shared_ptr<SubInstr>;
#define make_sub(lhs, rhs, opType) std::make_shared<SubInstr>(lhs, rhs, opType)

class MulInstr;
using mul_ptr_t = std::shared_ptr<MulInstr>;
#define make_mul(lhs, rhs, opType) std::make_shared<MulInstr>(lhs, rhs, opType)

class DivInstr;
using div_ptr_t = std::shared_ptr<DivInstr>;
#define make_div(lhs, rhs, opType) std::make_shared<DivInstr>(lhs, rhs, opType)

class RemInstr;
using rem_ptr_t = std::shared_ptr<RemInstr>;
#define make_rem(lhs, rhs, opType) std::make_shared<RemInstr>(lhs, rhs, opType)

class CmpInstr;
using cmp_ptr_t = std::shared_ptr<CmpInstr>;
#define make_cmp(lhs, rhs, cmpType) std::make_shared<CmpInstr>(lhs, rhs, cmpType)

// Block and Program
class InstrBlock;
using block_ptr_t = std::shared_ptr<InstrBlock>;
#define make_block(name) std::make_shared<InstrBlock>(name)

class Program;
using program_ptr_t = std::shared_ptr<Program>;
#define make_program() std::make_shared<Program>()

// Constant Values
class ConstantInt;
using const_int_ptr_t = std::shared_ptr<ConstantInt>;
#define make_const_int(value) std::make_shared<ConstantInt>(value)

class ConstantReal;
using const_real_ptr_t = std::shared_ptr<ConstantReal>;
#define make_const_real(value) std::make_shared<ConstantReal>(value)

class ConstantBool;
using const_bool_ptr_t = std::shared_ptr<ConstantBool>;
#define make_const_bool(value) std::make_shared<ConstantBool>(value)

class ConstantChar;
using const_char_ptr_t = std::shared_ptr<ConstantChar>;
#define make_const_char(value) std::make_shared<ConstantChar>(value)

class ConstantString;
using const_str_ptr_t = std::shared_ptr<ConstantString>;
#define make_const_str(value) std::make_shared<ConstantString>(value)

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

class GEPInstr : public User
{
    Use from;
    std::vector<Use> indices;

public:
    GEPInstr(value_ptr_t from, std::vector<value_ptr_t> indices, std::string name = "") : User(from->getType(), name), from(from, this)
    {
        for (auto &index : indices)
        {
            this->indices.emplace_back(index, this);
        }
    }
    ~GEPInstr() = default;

    std::string dump() const override;
};

class FuncInstr : public User
{
    type_ptr_t retType;
    std::vector<use_ptr_t> args;
    std::list<use_ptr_t> blocks;

public:
    FuncInstr(type_ptr_t retType, std::string name = "")
        : User(nullptr, std::move(name)), retType(std::move(retType)) {}
    ~FuncInstr() = default;

    type_ptr_t getRetType() { return retType; }

    const std::vector<use_ptr_t> &getArgs() { return args; }

    void addBlock(block_ptr_t block) { blocks.push_back(make_use(std::move(block), this)); }

    block_ptr_t newBlock(const std::string &name = "")
    {
        block_ptr_t block = make_block(name);
        blocks.push_back(make_use(block, this));
        return block;
    }

    std::string dump() const override;
};

class CallInstr : public User
{
    func_ptr_t func;

public:
    CallInstr(func_ptr_t func) : User(func->getRetType(), name), func(std::move(func))
    {
        const auto &args = this->func->getArgs();
        // for (auto &arg : args)
        // {
        //     this->args.emplace_back(arg, this);
        // }
    }
    ~CallInstr() = default;

    std::string dump() const override;
};

class RetInstr : public User
{
    use_ptr_t retval;

public:
    explicit RetInstr(use_ptr_t retval) : retval(std::move(retval)) {}
    ~RetInstr() = default;

    std::string dump() const override;
};

class BrInstr : public User
{
    use_ptr_t cond;
    block_ptr_t tc, fc;

public:
    explicit BrInstr(value_ptr_t cond)
        : cond(make_use(std::move(cond), this)), tc(nullptr), fc(nullptr) {}
    BrInstr(value_ptr_t cond, const block_ptr_t &tc, const block_ptr_t &fc)
        : cond(make_use(std::move(cond), this)), tc(std::move(tc)), fc(std::move(fc)) {}
    ~BrInstr() = default;

    void setTrueTarget(const block_ptr_t &tc) { this->tc = tc; }

    void setFalseTarget(const block_ptr_t &fc) { this->fc = fc; }

    bool isTerminator() override { return true; }

    std::string dump() const override;
};

class JmpInstr : public User
{
    block_ptr_t target;

public:
    explicit JmpInstr(const block_ptr_t &target) : target(std::move(target)) {}
    ~JmpInstr() = default;

    void setTarget(const block_ptr_t &target) { this->target = target; }

    bool isTerminator() override { return true; }

    std::string dump() const override;
};

class NegInstr : public User
{
    use_ptr_t from;
    OperandType opType;

public:
    explicit NegInstr(value_ptr_t from, OperandType opType)
        : from(make_use(std::move(from), this)), opType(opType) {}
    ~NegInstr() = default;

    std::string dump() const override;
};

class AddInstr : public User
{
    use_ptr_t lhs;
    use_ptr_t rhs;
    OperandType opType;

public:
    AddInstr(value_ptr_t lhs, value_ptr_t rhs, OperandType opType)
        : lhs(make_use(std::move(lhs), this)), rhs(make_use(std::move(rhs), this)), opType(opType) {}
    ~AddInstr() = default;

    std::string dump() const override;
};

class SubInstr : public User
{
    use_ptr_t lhs;
    use_ptr_t rhs;
    OperandType opType;

public:
    SubInstr(value_ptr_t lhs, value_ptr_t rhs, OperandType opType)
        : lhs(make_use(std::move(lhs), this)), rhs(make_use(std::move(rhs), this)), opType(opType) {}
    ~SubInstr() = default;

    std::string dump() const override;
};

class MulInstr : public User
{
    use_ptr_t lhs;
    use_ptr_t rhs;
    OperandType opType;

public:
    MulInstr(value_ptr_t lhs, value_ptr_t rhs, OperandType opType)
        : lhs(make_use(std::move(lhs), this)), rhs(make_use(std::move(rhs), this)), opType(opType) {}
    ~MulInstr() = default;

    std::string dump() const override;
};

class DivInstr : public User
{
    use_ptr_t lhs;
    use_ptr_t rhs;
    OperandType opType;

public:
    DivInstr(value_ptr_t lhs, value_ptr_t rhs, OperandType opType)
        : lhs(make_use(std::move(lhs), this)), rhs(make_use(std::move(rhs), this)), opType(opType) {}
    ~DivInstr() = default;

    std::string dump() const override;
};

class RemInstr : public User
{
    use_ptr_t lhs;
    use_ptr_t rhs;
    OperandType opType;

public:
    RemInstr(value_ptr_t lhs, value_ptr_t rhs, OperandType opType)
        : lhs(make_use(std::move(lhs), this)), rhs(make_use(std::move(rhs), this)), opType(opType) {}
    ~RemInstr() = default;

    std::string dump() const override;
};

class CmpInstr : public User
{
    use_ptr_t lhs;
    use_ptr_t rhs;
    OperandType opType;
    CompareType cmpType;

public:
    CmpInstr(user_ptr_t lhs, user_ptr_t rhs, OperandType opType, CompareType cmpType)
        : lhs(make_use(std::move(lhs), this)), rhs(make_use(std::move(rhs), this)), opType(opType), cmpType(cmpType) {}
    ~CmpInstr() = default;

    std::string dump() const override;
};

class InstrBlock : public User
{
    std::vector<use_ptr_t> instrs;

public:
    InstrBlock() = default;
    InstrBlock(const std::string &name) : User(nullptr, std::move(name)) {}
    ~InstrBlock() = default;

    void addInstr(user_ptr_t instr) { instrs.push_back(make_use(instr, this)); }

    std::string dump() const override;
};

class Program : public User
{
    std::vector<use_ptr_t> funcs;

public:
    Program() = default;
    ~Program() = default;

    void addFunc(user_ptr_t func) { funcs.push_back(make_use(std::move(func), this)); }

    std::string dump() const override;
};

class ConstantInt : public User
{
    int value;

public:
    explicit ConstantInt(int value) : value(value) {}
    ~ConstantInt() = default;

    bool isConstant() const override { return true; }
    std::string dump() const;
};

class ConstantReal : public User
{
    double value;

public:
    explicit ConstantReal(double value) : value(value) {}
    ~ConstantReal() = default;

    bool isConstant() const override { return true; }
    std::string dump() const;
};

class ConstantBool : public User
{
    bool value;

public:
    explicit ConstantBool(bool value) : value(value) {}
    ~ConstantBool() = default;

    bool isConstant() const override { return true; }
    std::string dump() const;
};

class ConstantChar : public User
{
    char value;

public:
    explicit ConstantChar(char value) : value(value) {}
    ~ConstantChar() = default;

    bool isConstant() const override { return true; }
    std::string dump() const;
};

class ConstantString : public User
{
    std::string value;

public:
    explicit ConstantString(std::string value) : value(std::move(value)) {}
    ~ConstantString() = default;

    bool isConstant() const override { return true; }
    std::string dump() const;
};
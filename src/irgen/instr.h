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
#include "utils/log.h"

#include <map>
#include <list>
#include <vector>

enum CompareType
{
    CT_EQ,
    CT_NE,
    CT_GT,
    CT_GE,
    CT_LT,
    CT_LE,
};

enum JumpReason
{
    JR_FALL_THROUGH,
    JR_BREAK_OUT,
    JR_CONTINUE,
    JR_TRUE_EXIT,
    JR_FALSE_EXIT,
};

class JumpTarget;

using target_ptr_t = std::shared_ptr<JumpTarget>;
using target_list_t = std::list<target_ptr_t>;
#define make_target() std::make_shared<JumpTarget>()

inline CompareType opTermToType(std::string opStr)
{
    if (opStr == "==")
        return CT_EQ;
    if (opStr == "!=")
        return CT_NE;
    if (opStr == ">")
        return CT_GT;
    if (opStr == ">=")
        return CT_GE;
    if (opStr == "<")
        return CT_LT;
    if (opStr == "<=")
        return CT_LE;
    assert(false, "unknown compare type");
    return CT_EQ;
}

class LabelInstr;
using label_ptr_t = std::shared_ptr<LabelInstr>;
#define make_label(name) std::make_shared<LabelInstr>(name)
#define cast_label(instr) std::dynamic_pointer_cast<LabelInstr>(instr)

// Memory Access and Addressing Operations （内存访问和寻址操作）
class AllocaInstr;
using alloca_ptr_t = std::shared_ptr<AllocaInstr>;
#define make_alloca(name, type) std::make_shared<AllocaInstr>(name, type)
#define cast_alloca(instr) std::dynamic_pointer_cast<AllocaInstr>(instr)

class GlobalInstr;
using global_ptr_t = std::shared_ptr<GlobalInstr>;
#define make_global(name, type, value) std::make_shared<GlobalInstr>(name, type, value)
#define cast_global(instr) std::dynamic_pointer_cast<GlobalInstr>(instr)

class LoadInstr;
using load_ptr_t = std::shared_ptr<LoadInstr>;
#define make_load(from) std::make_shared<LoadInstr>(from)
#define cast_load(instr) std::dynamic_pointer_cast<LoadInstr>(instr)

class StoreInstr;
using store_ptr_t = std::shared_ptr<StoreInstr>;
#define make_store(from, to) std::make_shared<StoreInstr>(from, to)
#define cast_store(instr) std::dynamic_pointer_cast<StoreInstr>(instr)

class GEPInstr; // GetElementPtr
using gep_ptr_t = std::shared_ptr<GEPInstr>;

// Function Call Instructions （函数调用和返回指令）
class FuncInstr; // 抽象指令，由其他指令组合而成
using func_ptr_t = std::shared_ptr<FuncInstr>;
#define make_func(name, retType) std::make_shared<FuncInstr>(name, retType)
#define cast_func(instr) std::dynamic_pointer_cast<FuncInstr>(instr)

class CallInstr;
using call_ptr_t = std::shared_ptr<CallInstr>;
#define make_call(func) std::make_shared<CallInstr>(func)
#define cast_call(instr) std::dynamic_pointer_cast<CallInstr>(instr)

// Terminator Instructions （终端指令）
class RetInstr;
using ret_ptr_t = std::shared_ptr<RetInstr>;
#define make_ret(retval) std::make_shared<RetInstr>(retval)
#define cast_ret(instr) std::dynamic_pointer_cast<RetInstr>(instr)

class BrInstr;
using br_ptr_t = std::shared_ptr<BrInstr>;
#define make_br(cond) std::make_shared<BrInstr>(cond)
#define cast_br(instr) std::dynamic_pointer_cast<BrInstr>(instr)

class JmpInstr; // 无条件跳转
using jmp_ptr_t = std::shared_ptr<JmpInstr>;
#define make_jmp(target) std::make_shared<JmpInstr>(target)
#define cast_jmp(instr) std::dynamic_pointer_cast<JmpInstr>(instr)

// Unary Operations （一元运算）
class NegInstr;
using neg_ptr_t = std::shared_ptr<NegInstr>;
#define make_neg(from, opType) std::make_shared<NegInstr>(from, opType)
#define cast_neg(instr) std::dynamic_pointer_cast<NegInstr>(instr)

// Binary Operations （二元运算）
class AddInstr;
using add_ptr_t = std::shared_ptr<AddInstr>;
#define make_add(lhs, rhs, opType) std::make_shared<AddInstr>(lhs, rhs, opType)
#define cast_add(instr) std::dynamic_pointer_cast<AddInstr>(instr)

class SubInstr;
using sub_ptr_t = std::shared_ptr<SubInstr>;
#define make_sub(lhs, rhs, opType) std::make_shared<SubInstr>(lhs, rhs, opType)
#define cast_sub(instr) std::dynamic_pointer_cast<SubInstr>(instr)

class MulInstr;
using mul_ptr_t = std::shared_ptr<MulInstr>;
#define make_mul(lhs, rhs, opType) std::make_shared<MulInstr>(lhs, rhs, opType)
#define cast_mul(instr) std::dynamic_pointer_cast<MulInstr>(instr)

class DivInstr;
using div_ptr_t = std::shared_ptr<DivInstr>;
#define make_div(lhs, rhs, opType) std::make_shared<DivInstr>(lhs, rhs, opType)
#define cast_div(instr) std::dynamic_pointer_cast<DivInstr>(instr)

class RemInstr;
using rem_ptr_t = std::shared_ptr<RemInstr>;
#define make_rem(lhs, rhs, opType) std::make_shared<RemInstr>(lhs, rhs, opType)
#define cast_rem(instr) std::dynamic_pointer_cast<RemInstr>(instr)

class CmpInstr;
using cmp_ptr_t = std::shared_ptr<CmpInstr>;
#define make_cmp(lhs, rhs, opType, cmpType) std::make_shared<CmpInstr>(lhs, rhs, opType, cmpType)
#define cast_cmp(instr) std::dynamic_pointer_cast<CmpInstr>(instr)

// Block and Program
class InstrBlock;
using block_ptr_t = std::shared_ptr<InstrBlock>;
#define make_block(name) std::make_shared<InstrBlock>(name)
#define cast_block(instr) std::dynamic_pointer_cast<InstrBlock>(instr)

class Program;
using program_ptr_t = std::shared_ptr<Program>;
#define make_program() std::make_shared<Program>()
#define cast_program(instr) std::dynamic_pointer_cast<Program>(instr)

// Constant Values
class Constant;
using const_val_ptr_t = std::shared_ptr<Constant>;
#define cast_const(instr) std::dynamic_pointer_cast<Constant>(instr)

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

class LabelInstr : public User
{
public:
    LabelInstr() = default;
    explicit LabelInstr(const std::string &name) : User(nullptr, name) {}
    ~LabelInstr() = default;

    std::string dump() const override;
};

class AllocaInstr : public User
{
    type_ptr_t ptrType;

public:
    AllocaInstr(std::string name, type_ptr_t type) : User(type, name)
    {
        ptrType = make_ptr_type(type);
    }
    ~AllocaInstr() = default;

    type_ptr_t getPtrType() const { return ptrType; }
    bool nameIsUnique() const override { return true; }

    std::string dump() const override;
};

class GlobalInstr : public User
{
    const_val_ptr_t initValue;

public:
    GlobalInstr(std::string name, type_ptr_t type, const_val_ptr_t value)
        : User(type, name), initValue(value) {}
    ~GlobalInstr() = default;

    std::string dump() const override;
};

class LoadInstr : public User
{
    Use from;

public:
    LoadInstr(alloca_ptr_t from)
        : User(from->getType(), "load"), from(from, this) {}
    ~LoadInstr() = default;

    std::string dump() const override;
};

class StoreInstr : public User
{
    Use from;
    Use to;

public:
    StoreInstr(value_ptr_t from, alloca_ptr_t to)
        : User(nullptr, "store"), from(from, this), to(to, this) {}
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
    prim_ptr_t retType;
    std::map<std::string, user_ptr_t> params;
    std::list<use_ptr_t> blocks;

public:
    FuncInstr(std::string name, prim_ptr_t retType)
        : User(nullptr, std::move(name)), retType(std::move(retType)) {}
    ~FuncInstr() = default;

    prim_ptr_t getRetType() { return retType; }

    bool addParam(prim_ptr_t type, std::string name)
    {
        if (params.count(name))
        {
            return false;
        }
        params[name] = make_user(type, name);
        return true;
    }

    void addParams(std::list<user_ptr_t> pList)
    {
        for (auto &arg : pList)
        {
            params[arg->getName()] = arg;
        }
    }

    user_ptr_t getParam(std::string name)
    {
        if (!params.count(name))
        {
            return nullptr;
        }
        return params[name];
    }

    std::vector<user_ptr_t> getParams()
    {
        std::vector<user_ptr_t> r;
        for (auto &[_, param] : params)
        {
            r.push_back(param);
        }
        return r;
    }

    bool matchRetType(PrimitiveType::PrimType pType)
    {
        return pType == retType->getType();
    }

    bool matchArgs(const std::list<user_ptr_t> args)
    {
        if (args.size() != this->params.size())
        {
            return false;
        }
        auto it1 = this->params.begin();
        auto it2 = args.begin();
        while (it1 != this->params.end())
        {
            if (*it1->second->getType() != *(*it2)->getType())
            {
                error << format(
                    "FuncInstr: argument type mismatch: $ and $.\n",
                    it1->second->getType()->dump(),
                    (*it2)->getType()->dump());
                return false;
            }
            ++it1;
            ++it2;
        }
        return true;
    }

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
    std::list<user_ptr_t> args;

public:
    CallInstr(func_ptr_t func) : User(func->getRetType(), "call"), func(func) {}
    ~CallInstr() = default;

    void addArgs(std::list<user_ptr_t> argList)
    {
        args.splice(args.end(), argList);
    }

    std::string dump() const override;
};

class RetInstr : public User
{
    use_ptr_t retval;

public:
    RetInstr() : retval(nullptr), User(nullptr, "return") {}
    explicit RetInstr(value_ptr_t retval) : retval(make_use(std::move(retval), this)) {}
    ~RetInstr() = default;

    std::string dump() const override;
};

class JumpTarget
{
public:
    user_ptr_t target;

    JumpTarget() : target(nullptr) {}
    void patch(user_ptr_t target)
    {
        this->target = target;
    }
};

class BrInstr : public User
{
    use_ptr_t cond;
    target_ptr_t tc, fc;

public:
    explicit BrInstr(value_ptr_t cond)
        : cond(make_use(std::move(cond), this)), tc(make_target()), fc(make_target()), User(nullptr, "branch") {}
    ~BrInstr() = default;

    std::pair<target_ptr_t, target_ptr_t> getTargets() { return {tc, fc}; }

    bool isTerminator() override { return true; }

    std::string dump() const override;
};

class JmpInstr : public User
{
    target_ptr_t target;

public:
    JmpInstr() : target(make_target()), User(nullptr, "jump") {}
    JmpInstr(user_ptr_t targetInstr) : target(make_target())
    {
        error << targetInstr->dump();
        target->patch(targetInstr);
    }
    ~JmpInstr() = default;

    target_ptr_t getTarget() { return target; }

    bool isTerminator() override { return true; }

    std::string dump() const override;
};

class NegInstr : public User
{
    use_ptr_t from;
    OperandType opType;

public:
    explicit NegInstr(value_ptr_t from, OperandType opType)
        : from(make_use(std::move(from), this)), opType(opType), User(from->getType(), "neg") {}
    ~NegInstr() = default;

    std::string dump() const override;
};

class AddInstr : public User
{
    use_ptr_t lhs;
    use_ptr_t rhs;
    OperandType opType;

public:
    AddInstr(user_ptr_t lhs, user_ptr_t rhs, OperandType opType)
        : opType(opType), User(lhs->getType(), "add")
    {
        assert(
            *lhs->getType() == *rhs->getType(),
            format(
                "AddInstr: lhs and rhs type mismatch: $ and $",
                lhs->getType()->dump(),
                rhs->getType()->dump()));
        this->lhs = make_use(std::move(lhs), this);
        this->rhs = make_use(std::move(rhs), this);
    }
    ~AddInstr() = default;

    std::string dump() const override;
};

class SubInstr : public User
{
    use_ptr_t lhs;
    use_ptr_t rhs;
    OperandType opType;

public:
    SubInstr(user_ptr_t lhs, user_ptr_t rhs, OperandType opType)
        : opType(opType), User(lhs->getType(), "sub")
    {
        assert(
            *lhs->getType() == *rhs->getType(),
            format(
                "MulInstr: lhs and rhs type mismatch: $ and $",
                lhs->getType()->dump(),
                rhs->getType()->dump()));
        this->lhs = make_use(std::move(lhs), this);
        this->rhs = make_use(std::move(rhs), this);
    }
    ~SubInstr() = default;

    std::string dump() const override;
};

class MulInstr : public User
{
    use_ptr_t lhs;
    use_ptr_t rhs;
    OperandType opType;

public:
    MulInstr(user_ptr_t lhs, user_ptr_t rhs, OperandType opType)
        : opType(opType), User(lhs->getType(), "mul")
    {
        assert(
            *lhs->getType() == *rhs->getType(),
            format(
                "MulInstr: lhs and rhs type mismatch: $ and $",
                lhs->getType()->dump(),
                rhs->getType()->dump()));
        this->lhs = make_use(std::move(lhs), this);
        this->rhs = make_use(std::move(rhs), this);
    }
    ~MulInstr() = default;

    std::string dump() const override;
};

class DivInstr : public User
{
    use_ptr_t lhs;
    use_ptr_t rhs;
    OperandType opType;

public:
    DivInstr(user_ptr_t lhs, user_ptr_t rhs, OperandType opType)
        : opType(opType), User(lhs->getType(), "div")
    {
        assert(
            *lhs->getType() == *rhs->getType(),
            format(
                "DivInstr: lhs and rhs type mismatch: $ and $",
                lhs->getType()->dump(),
                rhs->getType()->dump()));
        this->lhs = make_use(std::move(lhs), this);
        this->rhs = make_use(std::move(rhs), this);
    }
    ~DivInstr() = default;

    std::string dump() const override;
};

class RemInstr : public User
{
    use_ptr_t lhs;
    use_ptr_t rhs;
    OperandType opType;

public:
    RemInstr(user_ptr_t lhs, user_ptr_t rhs, OperandType opType)
        : opType(opType), User(lhs->getType(), "rem")
    {
        assert(
            *lhs->getType() == *rhs->getType(),
            format(
                "RemInstr: lhs and rhs type mismatch: $ and $",
                lhs->getType()->dump(),
                rhs->getType()->dump()));
        this->lhs = make_use(std::move(lhs), this);
        this->rhs = make_use(std::move(rhs), this);
    }
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
        : opType(opType), cmpType(cmpType), User(make_prime_type(PrimitiveType::PrimType::BOOL), "cmp")
    {
        assert(
            *lhs->getType() == *rhs->getType(),
            format(
                "CmpInstr: lhs and rhs type mismatch: $ and $",
                lhs->getType()->dump(),
                rhs->getType()->dump()));
        this->lhs = make_use(std::move(lhs), this);
        this->rhs = make_use(std::move(rhs), this);
    }
    ~CmpInstr() = default;

    std::string dump() const override;
};

class InstrBlock : public User
{
    std::list<use_ptr_t> instrs;

public:
    InstrBlock() = default;
    InstrBlock(const std::string &name) : User(nullptr, std::move(name)) {}
    ~InstrBlock() = default;

    void addInstr(user_ptr_t instr) { instrs.push_back(make_use(instr, this)); }
    void addInstrList(std::list<user_ptr_t> instrList)
    {
        for (auto &instr : instrList)
        {
            instrs.push_back(make_use(instr, this));
        }
    }
    void addInstrListFromFront(std::list<user_ptr_t> instrList)
    {
        for (auto rit = instrList.rbegin(); rit != instrList.rend(); ++rit)
        {
            instrs.push_front(make_use(*rit, this));
        }
    }

    std::string dump() const override;
};

class Program : public User
{
    std::list<use_ptr_t> globals;
    std::list<use_ptr_t> funcs;

public:
    Program() : User(nullptr, "program") {}
    ~Program() = default;

    void addGlobal(user_ptr_t global) { globals.push_back(make_use(std::move(global), this)); }
    void addFunc(user_ptr_t func) { funcs.push_back(make_use(std::move(func), this)); }

    std::string dump() const override;
};

class Constant : public User
{
public:
    Constant(type_ptr_t type, std::string name) : User(type, name) {}
    ~Constant() = default;

    bool isConstant() const override { return true; }

    virtual std::string dump() const = 0;
};

class ConstantInt : public Constant
{
    int constVal;

public:
    explicit ConstantInt(int constVal)
        : constVal(constVal), Constant(make_prime_type(PrimitiveType::PrimType::INT), "const int") {}
    ~ConstantInt() = default;

    std::string dump() const override;
};

class ConstantReal : public Constant
{
    double constVal;

public:
    explicit ConstantReal(double constVal)
        : constVal(constVal), Constant(make_prime_type(PrimitiveType::PrimType::REAL), "const real") {}
    ~ConstantReal() = default;

    std::string dump() const override;
};

class ConstantBool : public Constant
{
    bool constVal;

public:
    explicit ConstantBool(bool constVal)
        : constVal(constVal), Constant(make_prime_type(PrimitiveType::PrimType::BOOL), "const bool") {}
    ~ConstantBool() = default;

    std::string dump() const override;
};

class ConstantChar : public Constant
{
    char constVal;

public:
    explicit ConstantChar(char constVal)
        : constVal(constVal), Constant(make_prime_type(PrimitiveType::PrimType::CHAR), "const char") {}
    ~ConstantChar() = default;

    std::string dump() const override;
};

class ConstantString : public Constant
{
    std::string constVal;

public:
    explicit ConstantString(std::string constVal)
        : constVal(std::move(constVal)), Constant(make_prime_type(PrimitiveType::PrimType::STR), "const str") {}
    ~ConstantString() = default;

    std::string dump() const override;
};
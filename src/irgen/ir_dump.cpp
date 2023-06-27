/**
 * @file ir_dump.cpp
 * @author Zhenjie Wei (2024108@bjtu.edu.cn)
 * @brief LLVM IR Instruction Dumper
 * @date 2023-06-27
 *
 * @copyright Copyright (c) 2023
 *
 */

#include "irgen/ir_dump.h"
#include "instr.h"
#include "utils/str.h"

#include <map>
#include <string>
#include <sstream>
#include <iomanip>

std::string opType2Str(OperandType op)
{
    switch (op)
    {
    case OT_UNSIGNED:
        return "u";
    case OT_SIGNED:
        return "s";
    case OT_FLOAT:
        return "f";
    default:
        assert(false, "opType2Str: unknown operand type");
    }
}

std::string cmpType2Str(CompareType cmp)
{
    switch (cmp)
    {
    case CT_EQ:
        return "eq";
    case CT_NE:
        return "ne";
    case CT_GT:
        return "gt";
    case CT_GE:
        return "ge";
    case CT_LT:
        return "lt";
    case CT_LE:
        return "le";
    default:
        assert(false, "cmpType2Str: unknown compare type");
    }
}

std::string getIdOf(const Value *v)
{
    static std::map<std::string, size_t> nameMap;
    static std::map<const Value *, size_t> valueIdMap;
    if (!valueIdMap.count(v))
    {
        valueIdMap[v] = ++nameMap[v->getName()];
    }
    return std::to_string(valueIdMap[v]);
}

inline std::string getLabelTag(const Value *b)
{
    return format("$.$", b->getName(), getIdOf(b));
}

inline std::string getValueTag(dumper_ptr_t dumper, const Value *v, bool global = false)
{
    if (v->isConstant())
        return v->dump(dumper);
    std::string lead = global ? "@" : "%";
    std::string name = v->getName();
    std::string id = v->nameIsUnique() ? "" : "." + getIdOf(v);
    return lead + name + id;
}

std::string LLVMDumper::dumpLabel(const LabelInstr *label)
{
    return format("$:\n", getLabelTag(label));
}

std::string LLVMDumper::dumpAlloca(const AllocaInstr *alloc)
{
    return format("    $ = alloca $\n", getValueTag(this, alloc), alloc->getType()->dump());
}

std::string LLVMDumper::dumpGlobal(const GlobalInstr *global)
{
    return format(
        "$ = global $ $\n",
        getValueTag(this, global, true),
        global->getType()->dump(),
        global->getInitValue()->dump(this));
}

std::string LLVMDumper::dumpLoad(const LoadInstr *load)
{
    return format(
        "    $ = load $, $ $\n",
        getValueTag(this, load),
        load->getType()->dump(),
        cast_alloca(load->getFromUse().getValue())->getPtrType()->dump(),
        getValueTag(this, load->getFromUse().getValue().get()));
}

std::string LLVMDumper::dumpStore(const StoreInstr *store)
{
    return format(
        "    store $ $, $ $\n",
        store->getFromUse().getValue()->getType()->dump(),
        getValueTag(this, store->getFromUse().getValue().get()),
        cast_alloca(store->getToUse().getValue())->getPtrType()->dump(),
        getValueTag(this, store->getToUse().getValue().get()));
}

std::string LLVMDumper::dumpFunc(const FuncInstr *func)
{
    std::string s = format("define $ @$(", func->getRetType()->dump(), func->getName());
    auto params = func->getParams();
    for (auto param : params)
    {
        s += param->getType()->dump() + " " + getValueTag(this, param.get());
        if (param != params.back())
            s += ", ";
    }
    s += ") {\n";
    for (auto &block : func->getBlocks())
    {
        s += block->getValue()->dump(this);
    }
    s += "}\n\n";
    return s;
}

std::string LLVMDumper::dumpCall(const CallInstr *call)
{
    std::string s = format(
        "    $ = call $ @$(",
        getValueTag(this, call),
        call->getFunc()->getRetType()->dump(),
        call->getFunc()->getName());
    auto args = call->getArgs();
    for (auto it = args.begin(); it != args.end(); ++it)
    {
        s += getValueTag(this, it->get());
        if (it != std::prev(args.end()))
            s += ", ";
    }
    s += ")\n";
    return s;
}

std::string LLVMDumper::dumpRet(const RetInstr *ret)
{
    if (ret->getRetval() == nullptr)
        return "    ret void\n";
    return format(
        "    ret $ $\n",
        ret->getRetval()->getValue()->getType()->dump(),
        getValueTag(this, ret->getRetval()->getValue().get()));
}

std::string LLVMDumper::dumpBr(const BrInstr *br)
{
    target_ptr_t tc, fc;
    tie(tc, fc) = br->getTargets();
    bool lf = tc->target == nullptr;
    bool rf = fc->target == nullptr;
    if (lf || rf)
        return format("*   br nullptr $ $\n", lf ? "true" : "false", rf ? "true" : "false");
    assert(tc->target != nullptr, "BrInstr: true target is nullptr");
    assert(fc->target != nullptr, "BrInstr: false target is nullptr");
    return format(
        "    br i1 $, label $, label $\n",
        getValueTag(this, br->getCond()->getValue().get()),
        getLabelTag(tc->target.get()),
        getLabelTag(fc->target.get()));
}

std::string LLVMDumper::dumpJmp(const JmpInstr *jmp)
{
    auto target = jmp->getTarget();
    if (target->target == nullptr)
        return "*   br nullptr\n";
    assert(target->target != nullptr, "JmpInstr: target is nullptr");
    return format("    br label $\n", getLabelTag(target->target.get()));
}

std::string LLVMDumper::dumpNeg(const NegInstr *neg)
{
    return format(
        "    $ = $neg $ $\n",
        getValueTag(this, neg),
        opType2Str(neg->getOpType()),
        getValueTag(this, neg->getFromUse()->getValue().get()));
}

std::string LLVMDumper::dumpAdd(const AddInstr *add)
{
    auto lhs = add->getLhsUse();
    auto rhs = add->getRhsUse();
    return format(
        "    $ = $add $ $, $\n",
        getValueTag(this, add),
        opType2Str(add->getOpType()),
        lhs->getValue()->getType()->dump(),
        getValueTag(this, lhs->getValue().get()),
        getValueTag(this, rhs->getValue().get()));
}

std::string LLVMDumper::dumpSub(const SubInstr *sub)
{
    auto lhs = sub->getLhsUse();
    auto rhs = sub->getRhsUse();
    return format(
        "    $ = $sub $ $, $\n",
        getValueTag(this, sub),
        opType2Str(sub->getOpType()),
        lhs->getValue()->getType()->dump(),
        getValueTag(this, lhs->getValue().get()),
        getValueTag(this, rhs->getValue().get()));
}

std::string LLVMDumper::dumpMul(const MulInstr *mul)
{
    auto lhs = mul->getLhsUse();
    auto rhs = mul->getRhsUse();
    return format(
        "    $ = $mul $ $, $\n",
        getValueTag(this, mul),
        opType2Str(mul->getOpType()),
        lhs->getValue()->getType()->dump(),
        getValueTag(this, lhs->getValue().get()),
        getValueTag(this, rhs->getValue().get()));
}

std::string LLVMDumper::dumpDiv(const DivInstr *div)
{
    auto lhs = div->getLhsUse();
    auto rhs = div->getRhsUse();
    return format(
        "    $ = $div $ $, $\n",
        getValueTag(this, div),
        opType2Str(div->getOpType()),
        lhs->getValue()->getType()->dump(),
        getValueTag(this, lhs->getValue().get()),
        getValueTag(this, rhs->getValue().get()));
}

std::string LLVMDumper::dumpRem(const RemInstr *rem)
{
    auto lhs = rem->getLhsUse();
    auto rhs = rem->getRhsUse();
    return format(
        "    $ = $rem $ $, $\n",
        getValueTag(this, rem),
        opType2Str(rem->getOpType()),
        lhs->getValue()->getType()->dump(),
        getValueTag(this, lhs->getValue().get()),
        getValueTag(this, rhs->getValue().get()));
}

std::string LLVMDumper::dumpCmp(const CmpInstr *cmp)
{
    auto lhs = cmp->getLhsUse();
    auto rhs = cmp->getRhsUse();
    std::string s = format(
        "    $ = $cmp $ $ $, $\n",
        getValueTag(this, cmp),
        opType2Str(cmp->getOpType()),
        cmpType2Str(cmp->getCmpType()),
        lhs->getValue()->getType()->dump(),
        getValueTag(this, lhs->getValue().get()),
        getValueTag(this, rhs->getValue().get()));
    return s;
}

std::string LLVMDumper::dumpBlock(const InstrBlock *block)
{
    std::string s = format("$:\n", getLabelTag(block));
    for (auto &instr : block->getInstrs())
    {
        s += instr->getValue()->dump(this);
    }
    return s + "\n";
}

std::string LLVMDumper::dumpProgram(const Program *program)
{
    std::string s;

    for (auto &global : program->getGlobals())
    {
        s += global->getValue()->dump(this);
    }
    s += "\n";
    for (auto &func : program->getFuncs())
    {
        s += func->getValue()->dump(this);
    }

    return s;
}

std::string LLVMDumper::dumpConstInt(const ConstantInt *c_int)
{
    return std::to_string(c_int->getConstVal());
}

std::string LLVMDumper::dumpConstReal(const ConstantReal *c_real)
{
    double constVal = c_real->getConstVal();
    std::stringstream ss;
    std::string realStr = std::to_string(constVal);
    ss << std::fixed << std::setprecision(2) << constVal;
    return ss.str();
}

std::string LLVMDumper::dumpConstBool(const ConstantBool *c_bool)
{
    return c_bool->getConstVal() ? "true" : "false";
}

std::string LLVMDumper::dumpConstChar(const ConstantChar *c_char)
{
    return "'" + std::string(1, c_char->getConstVal()) + "'";
}

std::string LLVMDumper::dumpConstString(const ConstantString *c_str)
{
    return "\"" + c_str->getConstVal() + "\"";
}

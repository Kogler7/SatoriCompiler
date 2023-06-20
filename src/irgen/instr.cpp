/**
 * @file instr.cpp
 * @author Zhenjie Wei (2024108@bjtu.edu.cn)
 * @brief LLVM IR Instruction
 * @date 2023-06-07
 *
 * @copyright Copyright (c) 2023
 *
 */

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

inline std::string getValueTag(const Value *v, bool global = false)
{
    if (v->isConstant())
        return v->dump();
    std::string lead = global ? "@" : "%";
    std::string name = v->getName();
    std::string id = v->nameIsUnique() ? "" : "." + getIdOf(v);
    return lead + name + id;
}

std::string LabelInstr::dump() const
{
    return format("$:\n", getLabelTag(this));
}

std::string AllocaInstr::dump() const
{
    return format("    $ = alloca $\n", getValueTag(this), type->dump());
}

std::string GlobalInstr::dump() const
{
    return format(
        "$ = global $ $\n",
        getValueTag(this, true),
        type->dump(),
        initValue->dump());
}

std::string LoadInstr::dump() const
{
    return format(
        "    $ = load $, $ $\n",
        getValueTag(this),
        type->dump(),
        cast_alloca(from.getValue())->getPtrType()->dump(),
        getValueTag(from.getValue().get()));
}

std::string StoreInstr::dump() const
{
    return format(
        "    store $ $, $ $\n",
        from.getValue()->getType()->dump(),
        getValueTag(from.getValue().get()),
        cast_alloca(to.getValue())->getPtrType()->dump(),
        getValueTag(to.getValue().get()));
}

std::string GEPInstr::dump() const { return "not implemented."; }

std::string FuncInstr::dump() const
{
    std::string s = format("define $ @$(", retType->dump(), name);
    for (auto it = params.begin(); it != params.end(); ++it)
    {
        user_ptr_t param = it->second;
        s += param->getType()->dump() + " " + getValueTag(param.get());
        if (it != std::prev(params.end()))
            s += ", ";
    }
    s += ") {\n";
    for (auto &block : blocks)
    {
        s += block->getValue()->dump();
    }
    s += "}\n\n";
    return s;
}

std::string CallInstr::dump() const
{
    std::string s = format(
        "    $ = call $ @$(", 
        getValueTag(this),
        func->getRetType()->dump(),
        func->getName());
    for (auto it = args.begin(); it != args.end(); ++it)
    {
        s += getValueTag(it->get());
        if (it != std::prev(args.end()))
            s += ", ";
    }
    s += ")\n";
    return s;
}

std::string RetInstr::dump() const
{
    if (retval == nullptr)
        return "    ret void\n";
    return format(
        "    ret $ $\n",
        retval->getValue()->getType()->dump(),
        getValueTag(retval->getValue().get()));
}

std::string BrInstr::dump() const
{
    bool lf = tc->target == nullptr;
    bool rf = fc->target == nullptr;
    if (lf || rf)
        return format("*   br nullptr $ $\n", lf ? "true" : "false", rf ? "true" : "false");
    assert(tc->target != nullptr, "BrInstr: true target is nullptr");
    assert(fc->target != nullptr, "BrInstr: false target is nullptr");
    return format(
        "    br i1 $, label $, label $\n",
        getValueTag(cond->getValue().get()),
        getLabelTag(tc->target.get()),
        getLabelTag(fc->target.get()));
}

std::string JmpInstr::dump() const
{
    if (target->target == nullptr)
        return "*   br nullptr\n";
    assert(target->target != nullptr, "JmpInstr: target is nullptr");
    return format("    br label $\n", getLabelTag(target->target.get()));
}

std::string NegInstr::dump() const
{
    return format(
        "    $ = $neg $ $\n",
        getValueTag(this),
        opType2Str(opType),
        getValueTag(from->getValue().get()));
}

std::string AddInstr::dump() const
{
    return format(
        "    $ = $add $ $, $\n",
        getValueTag(this),
        opType2Str(opType),
        lhs->getValue()->getType()->dump(),
        getValueTag(lhs->getValue().get()),
        getValueTag(rhs->getValue().get()));
}

std::string SubInstr::dump() const
{
    return format(
        "    $ = $sub $ $, $\n",
        getValueTag(this),
        opType2Str(opType),
        lhs->getValue()->getType()->dump(),
        getValueTag(lhs->getValue().get()),
        getValueTag(rhs->getValue().get()));
}

std::string MulInstr::dump() const
{
    return format(
        "    $ = $mul $ $, $\n",
        getValueTag(this),
        opType2Str(opType),
        lhs->getValue()->getType()->dump(),
        getValueTag(lhs->getValue().get()),
        getValueTag(rhs->getValue().get()));
}

std::string DivInstr::dump() const
{
    return format(
        "    $ = $div $ $, $\n",
        getValueTag(this),
        opType2Str(opType),
        lhs->getValue()->getType()->dump(),
        getValueTag(lhs->getValue().get()),
        getValueTag(rhs->getValue().get()));
}

std::string RemInstr::dump() const
{
    return format(
        "    $ = $rem $ $, $\n",
        getValueTag(this),
        opType2Str(opType),
        lhs->getValue()->getType()->dump(),
        getValueTag(lhs->getValue().get()),
        getValueTag(rhs->getValue().get()));
}

std::string CmpInstr::dump() const
{
    std::string s = format(
        "    $ = $cmp $ $ $, $\n",
        getValueTag(this),
        opType2Str(opType),
        cmpType2Str(cmpType),
        lhs->getValue()->getType()->dump(),
        getValueTag(lhs->getValue().get()),
        getValueTag(rhs->getValue().get()));
    return s;
}

std::string InstrBlock::dump() const
{
    std::string s = format("$:\n", getLabelTag(this));
    for (auto &instr : instrs)
    {
        s += instr->getValue()->dump();
    }
    return s + "\n";
}

std::string Program::dump() const
{
    std::string s;

    for (auto &global : globals)
    {
        s += global->getValue()->dump();
    }
    s += "\n";
    for (auto &func : funcs)
    {
        s += func->getValue()->dump();
    }

    return s;
}

std::string ConstantInt::dump() const
{
    return std::to_string(constVal);
}

std::string ConstantReal::dump() const
{
    std::stringstream ss;
    std::string realStr = std::to_string(constVal);
    ss << std::fixed << std::setprecision(2) << constVal;
    return ss.str();
}

std::string ConstantBool::dump() const
{
    return constVal ? "true" : "false";
}

std::string ConstantChar::dump() const
{
    return "'" + std::string(1, constVal) + "'";
}

std::string ConstantString::dump() const
{
    return "\"" + constVal + "\"";
}
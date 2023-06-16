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

std::string getIdOf(const Value *v)
{
    static size_t id = 0;
    static std::map<const Value *, size_t> valueIdMap;
    if (!valueIdMap.count(v))
        valueIdMap[v] = ++id;
    return std::to_string(valueIdMap[v]);
}

inline std::string getLabelTag(const InstrBlock *b)
{
    return format("l_$_$", b->getName(), getIdOf(b));
}

inline std::string getValueTag(const Value *v)
{
    return format("%v_$_$", v->getName(), getIdOf(v));
}

std::string AllocInstr::dump() const
{
    return format("\t$ = alloca $\n", getValueTag(this), type->dump());
}

std::string LoadInstr::dump() const
{
    return format(
        "\t$ = load $, $ $\n",
        getValueTag(this),
        type->dump(),
        from.getValue()->getType()->dump(),
        getValueTag(from.getValue().get()));
}

std::string StoreInstr::dump() const
{
    return format(
        "\tstore $ $, $ $\n",
        from.getValue()->getType()->dump(),
        getValueTag(from.getValue().get()),
        to.getValue()->getType()->dump(),
        getValueTag(to.getValue().get()));
}

std::string GEPInstr::dump() const { return "not implemented."; }

std::string FuncInstr::dump() const
{
    std::string s = format("define $ $(", retType->dump(), name);
    // for (auto &arg : args)
    // {
    //     s += format("$ $, ", arg->getType()->dump(), arg->getName());
    // }
    s += ") {\n";
    for (auto &block : blocks)
    {
        s += block->getValue()->dump();
    }
    s += "}\n";
    return s;
}

std::string CallInstr::dump() const
{
    std::string s = format("\t$ = call $(", getValueTag(this), func->getName());
    // for (auto &arg : args)
    // {
    //     s += format("$ $, ", arg->getType()->dump(), arg->getName());
    // }
    s += ")\n";
    return s;
}

std::string RetInstr::dump() const
{
    if (retval == nullptr)
        return "\tret void\n";
    return format(
        "\tret $ $\n",
        retval->getValue()->getType()->dump(),
        getValueTag(retval->getValue().get()));
}

std::string BrInstr::dump() const
{
    return format(
        "\tbr i1 $, label $, label $\n",
        getValueTag(cond->getValue().get()),
        getLabelTag(tc.get()),
        getLabelTag(fc.get()));
}

std::string JmpInstr::dump() const
{
    return format("\tbr label $\n", getLabelTag(target.get()));
}

std::string NegInstr::dump() const
{
    return format("\t$ = $neg $ $\n", getValueTag(this), opType, getValueTag(from->getValue().get()));
}

std::string AddInstr::dump() const
{
    return format(
        "\t$ = $add $ $, $\n",
        getValueTag(this),
        opType,
        getValueTag(lhs->getValue().get()),
        getValueTag(rhs->getValue().get()));
}

std::string SubInstr::dump() const
{
    return format(
        "\t$ = $sub $ $, $\n",
        getValueTag(this),
        opType,
        getValueTag(lhs->getValue().get()),
        getValueTag(rhs->getValue().get()));
}

std::string MulInstr::dump() const
{
    return format(
        "\t$ = $mul $ $, $\n",
        getValueTag(this),
        opType,
        getValueTag(lhs->getValue().get()),
        getValueTag(rhs->getValue().get()));
}

std::string DivInstr::dump() const
{
    return format(
        "\t$ = $div $ $, $\n",
        getValueTag(this),
        opType,
        getValueTag(lhs->getValue().get()),
        getValueTag(rhs->getValue().get()));
}

std::string RemInstr::dump() const
{
    return format(
        "\t$ = $rem $ $, $\n",
        getValueTag(this),
        opType,
        getValueTag(lhs->getValue().get()),
        getValueTag(rhs->getValue().get()));
}

std::string CmpInstr::dump() const
{
    std::string s = format(
        "\t$ = $cmp $ $ $, $\n",
        getValueTag(this),
        opType,
        cmpType,
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
    return s;
}

std::string Program::dump() const
{
    std::string s;
    for (auto &func : funcs)
    {
        s += func->getValue()->dump();
    }
    return s;
}

std::string ConstantInt::dump() const
{
    return std::to_string(value);
}

std::string ConstantReal::dump() const
{
    return std::to_string(value);
}

std::string ConstantBool::dump() const
{
    return value ? "true" : "false";
}

std::string ConstantChar::dump() const
{
    return "'" + std::string(1, value) + "'";
}

std::string ConstantString::dump() const
{
    return "\"" + value + "\"";
}
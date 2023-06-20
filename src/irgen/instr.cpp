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
    return std::string(1, cmp);
}

std::string getIdOf(const Value *v)
{
    static size_t id = 0;
    static std::map<const Value *, size_t> valueIdMap;
    if (!valueIdMap.count(v))
        valueIdMap[v] = ++id;
    return std::to_string(valueIdMap[v]);
}

inline std::string getLabelTag(const Value *b)
{
    return format("l_$_$", b->getName(), getIdOf(b));
}

inline std::string getValueTag(const Value *v)
{
    return format("%v_$_$", v->getName(), getIdOf(v));
}

std::string LabelInstr::dump() const
{
    return format("$:\n", getLabelTag(this));
}

std::string AllocaInstr::dump() const
{
    return format("\t$ = alloca $\n", getValueTag(this), type->dump());
}

std::string GlobalInstr::dump() const
{
    return format(
        "\t$ = global $ $\n",
        getValueTag(this),
        type->dump(),
        initValue->dump());
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
    for (auto it = params.begin(); it != params.end(); ++it)
    {
        user_ptr_t param = it->second;
        s += param->getType()->dump() + " " + param->getName();
        if (it != std::prev(params.end()))
            s += ", ";
    }
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
        return "\tret void\n";
    return format(
        "\tret $ $\n",
        retval->getValue()->getType()->dump(),
        getValueTag(retval->getValue().get()));
}

std::string BrInstr::dump() const
{
    return "\tbr\n";
    assert(tc->target != nullptr, "BrInstr: true target is nullptr");
    assert(fc->target != nullptr, "BrInstr: false target is nullptr");
    return format(
        "\tbr i1 $, label $, label $\n",
        getValueTag(cond->getValue().get()),
        getLabelTag(tc->target.get()),
        getLabelTag(fc->target.get()));
}

std::string JmpInstr::dump() const
{
    assert(target != nullptr, "JmpInstr: target is nullptr");
    return format("\tbr label $\n", getLabelTag(target->target.get()));
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
        opType2Str(opType),
        getValueTag(lhs->getValue().get()),
        getValueTag(rhs->getValue().get()));
}

std::string SubInstr::dump() const
{
    return format(
        "\t$ = $sub $ $, $\n",
        getValueTag(this),
        opType2Str(opType),
        getValueTag(lhs->getValue().get()),
        getValueTag(rhs->getValue().get()));
}

std::string MulInstr::dump() const
{
    return format(
        "\t$ = $mul $ $, $\n",
        getValueTag(this),
        opType2Str(opType),
        getValueTag(lhs->getValue().get()),
        getValueTag(rhs->getValue().get()));
}

std::string DivInstr::dump() const
{
    return format(
        "\t$ = $div $ $, $\n",
        getValueTag(this),
        opType2Str(opType),
        getValueTag(lhs->getValue().get()),
        getValueTag(rhs->getValue().get()));
}

std::string RemInstr::dump() const
{
    return format(
        "\t$ = $rem $ $, $\n",
        getValueTag(this),
        opType2Str(opType),
        getValueTag(lhs->getValue().get()),
        getValueTag(rhs->getValue().get()));
}

std::string CmpInstr::dump() const
{
    std::string s = format(
        "\t$ = $cmp $ $ $, $\n",
        getValueTag(this),
        opType2Str(opType),
        cmpType2Str(cmpType),
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

    for (auto &global : globals)
    {
        s += global->getValue()->dump();
    }

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
    return std::to_string(constVal);
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
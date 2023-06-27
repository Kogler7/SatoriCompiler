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

std::string LabelInstr::dump(dumper_ptr_t dumper) const
{
    return dumper->dumpLabel(this);
}

std::string AllocaInstr::dump(dumper_ptr_t dumper) const
{
    return dumper->dumpAlloca(this);
}

std::string GlobalInstr::dump(dumper_ptr_t dumper) const
{
    return dumper->dumpGlobal(this);
}

std::string LoadInstr::dump(dumper_ptr_t dumper) const
{
    return dumper->dumpLoad(this);
}

std::string StoreInstr::dump(dumper_ptr_t dumper) const
{
    return dumper->dumpStore(this);
}

std::string GEPInstr::dump(dumper_ptr_t dumper) const { return "not implemented."; }

std::string FuncInstr::dump(dumper_ptr_t dumper) const
{
    return dumper->dumpFunc(this);
}

std::string CallInstr::dump(dumper_ptr_t dumper) const
{
    return dumper->dumpCall(this);
}

std::string RetInstr::dump(dumper_ptr_t dumper) const
{
    return dumper->dumpRet(this);
}

std::string BrInstr::dump(dumper_ptr_t dumper) const
{
    return dumper->dumpBr(this);
}

std::string JmpInstr::dump(dumper_ptr_t dumper) const
{
    return dumper->dumpJmp(this);
}

std::string NegInstr::dump(dumper_ptr_t dumper) const
{
    return dumper->dumpNeg(this);
}

std::string AddInstr::dump(dumper_ptr_t dumper) const
{
    return dumper->dumpAdd(this);
}

std::string SubInstr::dump(dumper_ptr_t dumper) const
{
    return dumper->dumpSub(this);
}

std::string MulInstr::dump(dumper_ptr_t dumper) const
{
    return dumper->dumpMul(this);
}

std::string DivInstr::dump(dumper_ptr_t dumper) const
{
    return dumper->dumpDiv(this);
}

std::string RemInstr::dump(dumper_ptr_t dumper) const
{
    return dumper->dumpRem(this);
}

std::string CmpInstr::dump(dumper_ptr_t dumper) const
{
    return dumper->dumpCmp(this);
}

std::string InstrBlock::dump(dumper_ptr_t dumper) const
{
    return dumper->dumpBlock(this);
}

std::string Program::dump(dumper_ptr_t dumper) const
{
    return dumper->dumpProgram(this);
}

std::string ConstantInt::dump(dumper_ptr_t dumper) const
{
    return dumper->dumpConstInt(this);
}

std::string ConstantReal::dump(dumper_ptr_t dumper) const
{
    return dumper->dumpConstReal(this);
}

std::string ConstantBool::dump(dumper_ptr_t dumper) const
{
    return dumper->dumpConstBool(this);
}

std::string ConstantChar::dump(dumper_ptr_t dumper) const
{
    return dumper->dumpConstChar(this);
}

std::string ConstantString::dump(dumper_ptr_t dumper) const
{
    return dumper->dumpConstString(this);
}
/**
 * @file asm.cpp
 * @author Zhenjie Wei (2024108@bjtu.edu.cn)
 * @brief Instruction Selection Dumper
 * @date 2023-06-27
 *
 * @copyright Copyright (c) 2023
 *
 */

#include "codegen/asm.h"
#include "irgen/instr.h"
#include "utils/str.h"

#include <map>
#include <string>
#include <sstream>
#include <iomanip>

std::string ASMDumper::dumpLabel(const LabelInstr *label)
{
    return "$:\n";
}

std::string ASMDumper::dumpAlloca(const AllocaInstr *alloc)
{
    int offset = stackFrame.addValue(alloc, 4);
    std::string sub = format("    sub esp, 4\n");
    return sub;
}

std::string ASMDumper::dumpGlobal(const GlobalInstr *global)
{
    return "global";
}

std::string ASMDumper::dumpLoad(const LoadInstr *load)
{
    return "load";
}

std::string ASMDumper::dumpStore(const StoreInstr *store)
{
    int offset = stackFrame.getOffset(store);
    // std::string mov = format("    mov [ebp - $], $\n", offset, store->getVal()->dump(this));
    return "store";
}

std::string ASMDumper::dumpFunc(const FuncInstr *func)
{
    return "func";
}

std::string ASMDumper::dumpCall(const CallInstr *call)
{
    return "call";
}

std::string ASMDumper::dumpRet(const RetInstr *ret)
{
    return "ret";
}

std::string ASMDumper::dumpBr(const BrInstr *br)
{
    return "br";
}

std::string ASMDumper::dumpJmp(const JmpInstr *jmp)
{
    return "jmp";
}

std::string ASMDumper::dumpNeg(const NegInstr *neg)
{
    return "neg";
}

std::string ASMDumper::dumpAdd(const AddInstr *add)
{
    return "add";
}

std::string ASMDumper::dumpSub(const SubInstr *sub)
{
    return "sub";
}

std::string ASMDumper::dumpMul(const MulInstr *mul)
{
    return "mul";
}

std::string ASMDumper::dumpDiv(const DivInstr *div)
{
    return "div";
}

std::string ASMDumper::dumpRem(const RemInstr *rem)
{
    return "rem";
}

std::string ASMDumper::dumpCmp(const CmpInstr *cmp)
{
    return "cmp";
}

std::string ASMDumper::dumpBlock(const InstrBlock *block)
{
    return "block";
}

std::string ASMDumper::dumpProgram(const Program *program)
{
    return "program";
}

std::string ASMDumper::dumpConstInt(const ConstantInt *c_int)
{
    return std::to_string(c_int->getConstVal());
}

std::string ASMDumper::dumpConstReal(const ConstantReal *c_real)
{
    double constVal = c_real->getConstVal();
    std::stringstream ss;
    std::string realStr = std::to_string(constVal);
    ss << std::fixed << std::setprecision(2) << constVal;
    return ss.str();
}

std::string ASMDumper::dumpConstBool(const ConstantBool *c_bool)
{
    return c_bool->getConstVal() ? "true" : "false";
}

std::string ASMDumper::dumpConstChar(const ConstantChar *c_char)
{
    return "'" + std::string(1, c_char->getConstVal()) + "'";
}

std::string ASMDumper::dumpConstString(const ConstantString *c_str)
{
    return "\"" + c_str->getConstVal() + "\"";
}

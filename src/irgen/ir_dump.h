/**
 * @file ir_dump.h
 * @author Zhenjie Wei (2024108@bjtu.edu.cn)
 * @brief LLVM IR Instruction Dumper
 * @date 2023-06-27
 *
 * @copyright Copyright (c) 2023
 *
 */

#pragma once

#include "common/dumper.h"

class LLVMDumper;
using llvm_dumper_ptr_t = LLVMDumper *;
#define make_llvm_dumper() std::make_shared<LLVMDumper>()

class LLVMDumper : public Dumper
{
public:
    std::string dumpLabel(const LabelInstr *label);
    std::string dumpAlloca(const AllocaInstr *alloc);
    std::string dumpGlobal(const GlobalInstr *global);
    std::string dumpLoad(const LoadInstr *load);
    std::string dumpStore(const StoreInstr *store);
    std::string dumpFunc(const FuncInstr *func);
    std::string dumpCall(const CallInstr *call);
    std::string dumpRet(const RetInstr *ret);
    std::string dumpBr(const BrInstr *br);
    std::string dumpJmp(const JmpInstr *jmp);
    std::string dumpNeg(const NegInstr *neg);
    std::string dumpAdd(const AddInstr *add);
    std::string dumpSub(const SubInstr *sub);
    std::string dumpMul(const MulInstr *mul);
    std::string dumpDiv(const DivInstr *div);
    std::string dumpRem(const RemInstr *rem);
    std::string dumpCmp(const CmpInstr *cmp);
    std::string dumpBlock(const InstrBlock *block);
    std::string dumpProgram(const Program *program);

    std::string dumpConstInt(const ConstantInt *c_int);
    std::string dumpConstReal(const ConstantReal *c_real);
    std::string dumpConstBool(const ConstantBool *c_bool);
    std::string dumpConstChar(const ConstantChar *c_char);
    std::string dumpConstString(const ConstantString *c_str);
};
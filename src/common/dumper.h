/**
 * @file dumper.h
 * @author Zhenjie Wei (2024108@bjtu.edu.cn)
 * @brief Instruction Dumper
 * @date 2023-06-27
 *
 * @copyright Copyright (c) 2023
 *
 */

#pragma once

#include <string>

class LabelInstr;
class AllocaInstr;
class GlobalInstr;
class LoadInstr;
class StoreInstr;
class GEPInstr;
class FuncInstr;
class CallInstr;
class RetInstr;
class BrInstr;
class JmpInstr;
class NegInstr;
class AddInstr;
class SubInstr;
class MulInstr;
class DivInstr;
class RemInstr;
class CmpInstr;
class InstrBlock;
class Program;
class Constant;
class ConstantInt;
class ConstantReal;
class ConstantBool;
class ConstantChar;
class ConstantString;

class Dumper;
using dumper_ptr_t = Dumper *;
#define make_dumper() std::make_shared<Dumper>()

class Dumper
{
public:
    virtual std::string dumpLabel(const LabelInstr *label) = 0;
    virtual std::string dumpAlloca(const AllocaInstr *alloc) = 0;
    virtual std::string dumpGlobal(const GlobalInstr *global) = 0;
    virtual std::string dumpLoad(const LoadInstr *load) = 0;
    virtual std::string dumpStore(const StoreInstr *store) = 0;
    virtual std::string dumpFunc(const FuncInstr *func) = 0;
    virtual std::string dumpCall(const CallInstr *call) = 0;
    virtual std::string dumpRet(const RetInstr *ret) = 0;
    virtual std::string dumpBr(const BrInstr *br) = 0;
    virtual std::string dumpJmp(const JmpInstr *jmp) = 0;
    virtual std::string dumpNeg(const NegInstr *neg) = 0;
    virtual std::string dumpAdd(const AddInstr *add) = 0;
    virtual std::string dumpSub(const SubInstr *sub) = 0;
    virtual std::string dumpMul(const MulInstr *mul) = 0;
    virtual std::string dumpDiv(const DivInstr *div) = 0;
    virtual std::string dumpRem(const RemInstr *rem) = 0;
    virtual std::string dumpCmp(const CmpInstr *cmp) = 0;
    virtual std::string dumpBlock(const InstrBlock *block) = 0;
    virtual std::string dumpProgram(const Program *program) = 0;

    virtual std::string dumpConstInt(const ConstantInt *c_int) = 0;
    virtual std::string dumpConstReal(const ConstantReal *c_real) = 0;
    virtual std::string dumpConstBool(const ConstantBool *c_bool) = 0;
    virtual std::string dumpConstChar(const ConstantChar *c_char) = 0;
    virtual std::string dumpConstString(const ConstantString *c_str) = 0;
};
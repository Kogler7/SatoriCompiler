/**
 * @file regis.h
 * @author Zhenjie Wei (2024108@bjtu.edu.cn)
 * @brief Register Allocation
 * @date 2023-06-27
 *
 * @copyright Copyright (c) 2023
 *
 */

#pragma once

#include "irgen/instr.h"

#include <map>

class RegisterAllocator
{
private:
    std::map<const Value *, size_t> valueRegMap;
    size_t regCount;

public:
    RegisterAllocator() : regCount(0) {}

    void addValue(const Value *v)
    {
        valueRegMap[v] = regCount++;
    }

    size_t getReg(const Value *v) const
    {
        assert(valueRegMap.count(v), "RegisterAllocator::getReg: value not in register");
        return valueRegMap.at(v);
    }

    size_t getRegCount() const { return regCount; }
};
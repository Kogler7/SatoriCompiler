/**
 * @file stack.h
 * @author Zhenjie Wei (2024108@bjtu.edu.cn)
 * @brief Stack Frame
 * @date 2023-06-27
 *
 * @copyright Copyright (c) 2023
 *
 */

#pragma once

#include "irgen/instr.h"

#include <map>

class StackFrame
{
private:
    std::map<const Value *, size_t> valueOffsetMap;
    size_t frameSize;

public:
    StackFrame() : frameSize(0) {}

    int addValue(const Value *v, size_t size)
    {
        valueOffsetMap[v] = frameSize;
        frameSize += size;
        return frameSize - size;
    }

    void popValue(size_t cnt)
    {
        for (size_t i = 0; i < cnt; i++)
        {
            valueOffsetMap.erase(valueOffsetMap.rbegin()->first);
            frameSize--;
        }
    }

    int getOffset(const Value *v) const
    {
        assert(valueOffsetMap.count(v), "StackFrame::getOffset: value not in stack frame");
        return valueOffsetMap.at(v);
    }

    size_t getSize() const { return frameSize; }
};
/**
 * @file type.h
 * @author Zhenjie Wei (2024108@bjtu.edu.cn)
 * @brief Type System
 * @date 2023-06-07
 *
 * @copyright Copyright (c) 2023
 *
 */

#include "type.h"
#include "utils/log.h"

std::string PrimitiveType::dump() const
{
    switch (type)
    {
    case INT:
        return "i32";
    case REAL:
        return "float";
    case BOOL:
        return "i1";
    case CHAR:
        return "i8";
    case STR:
        return "str";
    case VOID:
        return "void";
    default:
        assert(false, "PrimitiveType::dump: unknown primitive type");
    }
    return "";
}

std::string PointerType::dump() const
{
    return ptr->dump() + "*";
}
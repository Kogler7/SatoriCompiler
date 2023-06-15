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
        return "int";
    case REAL:
        return "real";
    case BOOL:
        return "bool";
    case CHAR:
        return "char";
    case STR:
        return "str";
    default:
        error << "unknown primitive type" << std::endl;
    }
}

std::string PointerType::dump() const
{
    return ptr->dump() + "*";
}
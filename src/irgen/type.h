/**
 * @file type.h
 * @author Zhenjie Wei (2024108@bjtu.edu.cn)
 * @brief Type System
 * @date 2023-06-07
 *
 * @copyright Copyright (c) 2023
 *
 */

#pragma once

#include "utils/log.h"

#include <string>
#include <memory>

enum OperandType
{
    OT_UNSIGNED = 'u',
    OT_SIGNED = 's',
    OT_FLOAT = 'f',
};

class Type;
using type_ptr_t = std::shared_ptr<Type>;

class PrimitiveType;
using prim_ptr_t = std::shared_ptr<PrimitiveType>;
#define make_prime_type(type) std::make_shared<PrimitiveType>(type)

class PointerType;
using ptr_ptr_t = std::shared_ptr<PointerType>;
#define make_ptr_type(type) std::make_shared<PointerType>(type)

class Type
{
public:
    virtual ~Type() = default;

    virtual std::string dump() const = 0;

    virtual type_ptr_t getDeRefed() const = 0;

    virtual OperandType getOpType() const { return OT_SIGNED; }

    virtual bool operator==(const Type &other) const = 0;
};

class PrimitiveType : public Type
{
public:
    enum PrimType
    {
        INT,
        REAL,
        BOOL,
        CHAR,
        STR,
        VOID
    };

    explicit PrimitiveType(PrimType type) : type(type) {}

    std::string dump() const override;

    type_ptr_t getDeRefed() const override { return nullptr; }

    PrimType getType() const { return type; }

    OperandType getOpType() const override
    {
        switch (type)
        {
        case INT:
            return OT_SIGNED;
        case REAL:
            return OT_FLOAT;
        case BOOL:
            return OT_UNSIGNED;
        case CHAR:
            return OT_UNSIGNED;
        case STR:
            return OT_UNSIGNED;
        default:
            assert(false, "unknown primitive type");
            return OT_SIGNED;
        }
    }

    static PrimType str2type(const std::string &str)
    {
        if (str == "int")
            return PrimitiveType::INT;
        if (str == "real")
            return PrimitiveType::REAL;
        if (str == "bool")
            return PrimitiveType::BOOL;
        if (str == "char")
            return PrimitiveType::CHAR;
        if (str == "str")
            return PrimitiveType::STR;
        assert(
            false,
            format("unknown primitive type: $", str));
        return PrimitiveType::INT;
    }

    bool operator==(const Type &other) const override
    {
        if (auto other_prim = dynamic_cast<const PrimitiveType *>(&other))
            return type == other_prim->type;
        return false;
    }

protected:
    PrimType type;
};

class PointerType : public Type
{
public:
    explicit PointerType(type_ptr_t type) : ptr(type) {}

    std::string dump() const override;

    type_ptr_t getDeRefed() const override { return ptr; }

    OperandType getOpType() const override { return OT_UNSIGNED; }

    bool operator==(const Type &other) const override
    {
        if (auto other_ptr = dynamic_cast<const PointerType *>(&other))
            return ptr == other_ptr->ptr;
        return false;
    }

protected:
    type_ptr_t ptr;
};

#define is_prim_int(type) (type->getType() == PrimitiveType::INT)
#define is_prim_real(type) (type->getType() == PrimitiveType::REAL)
#define is_prim_bool(type) (type->getType() == PrimitiveType::BOOL)
#define is_prim_char(type) (type->getType() == PrimitiveType::CHAR)
#define is_prim_str(type) (type->getType() == PrimitiveType::STR)
#define is_prim_void(type) (type->getType() == PrimitiveType::VOID)

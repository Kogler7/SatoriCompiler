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

#include <string>
#include <memory>

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
    };

    explicit PrimitiveType(PrimType type) : type(type) {}
    std::string dump() const override;
    type_ptr_t getDeRefed() const override { return nullptr; }

protected:
    PrimType type;
};

class PointerType : public Type
{
public:
    explicit PointerType(type_ptr_t type) : ptr(type) {}
    std::string dump() const override;
    type_ptr_t getDeRefed() const override { return ptr; }

protected:
    type_ptr_t ptr;
};
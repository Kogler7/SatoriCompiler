/**
 * @file use.h
 * @author Zhenjie Wei (2024108@bjtu.edu.cn)
 * @brief Use Relation
 * @date 2023-06-07
 *
 * @copyright Copyright (c) 2023
 *
 */

#pragma once

#include "type.h"

#include <set>
#include <string>

class Value;
class User;
class Use;

using use_ptr_t = std::shared_ptr<Use>;
using user_ptr_t = std::shared_ptr<User>;
using value_ptr_t = std::shared_ptr<Value>;

class Value
{
protected:
    Type type;
    std::string name;
    std::set<use_ptr_t> uses;

public:
    Value(Type type, std::string name) : type(type), name(name) {}
    ~Value() = default;
    Type getType() { return type; }
};

class User : public Value
{
public:
    explicit User(Type type, std::string name) : Value(type, name) {}
};

class Use
{
    user_ptr_t user;
    value_ptr_t value;

public:
    Use(user_ptr_t user, value_ptr_t value) : user(std::move(user)), value(std::move(value)) {}
    ~Use() = default;
    user_ptr_t getUser() { return user; }
    value_ptr_t getValue() { return value; }
    void setUser(user_ptr_t user) { this->user = std::move(user); }
    void setValue(value_ptr_t value) { this->value = std::move(value); }
};
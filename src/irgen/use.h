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
    type_ptr_t type;
    std::string name;
    std::set<use_ptr_t> uses;

public:
    Value(type_ptr_t type, std::string name) : type(type), name(name) {}
    ~Value() = default;
    type_ptr_t getType() { return type; }
    void addUse(use_ptr_t use) { uses.insert(use); }
    bool delUse(use_ptr_t use) { return uses.erase(use); }
    virtual bool isConstant() const { return false; }
    virtual std::string dump() const { return name; }
};

class User : public Value
{
public:
    explicit User(type_ptr_t type = nullptr, std::string name = "") : Value(type, name) {}
    virtual bool isTerminator() { return false; }
};

class Use
{
    User *user;
    value_ptr_t value;

public:
    Use(value_ptr_t value, User *user) : user(user), value(value) {}
    ~Use() = default;
    User *getUser() { return user; }
    value_ptr_t getValue() { return value; }
    void setUser(User *user) { this->user = user; }
    void setValue(value_ptr_t value) { this->value = value; }
};
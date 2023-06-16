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
#define make_use(value, user) std::make_shared<Use>(value, user)
using user_ptr_t = std::shared_ptr<User>;
#define make_user(type, name) std::make_shared<User>(type, name)
using value_ptr_t = std::shared_ptr<Value>;
#define make_value(type, name) std::make_shared<Value>(type, name)

class Value
{
protected:
    type_ptr_t type;
    std::string name;
    std::set<Use *> uses;

public:
    Value(type_ptr_t type, std::string name) : type(type), name(name) {}
    ~Value() = default;

    type_ptr_t getType() { return type; }

    void addUse(Use *use) { uses.insert(use); }
    bool delUse(Use *use) { return uses.erase(use); }

    void setName(std::string name) { this->name = name; }
    std::string getName() const { return name; }
    
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
    Use(value_ptr_t value, User *user) : user(user), value(value)
    {
        if (value)
            value->addUse(this);
    }
    ~Use() = default;

    value_ptr_t getValue() const { return value; }

    User *getUser() { return user; }

    void setUser(User *user) { this->user = user; }
    
    void setValue(value_ptr_t value) { this->value = value; }
};
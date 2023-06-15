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

#include <memory>

class Type;
class IntegerType;
class RealType;
class BooleanType;
class StringType;

class Type
{

};

using type_ptr_t = std::shared_ptr<Type>;

class IntegerType : public Type
{

};

class RealType : public Type
{

};

class BooleanType : public Type
{

};

class StringType : public Type
{

};
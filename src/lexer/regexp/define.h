/**
 * @file define.h
 * @author Zhenjie Wei (2024108@bjtu.edu.cn)
 * @brief Regular Expression Definition Parser
 * @date 2023-04-22
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#pragma once

#include "utils/view/viewer.h"

#include <map>
#include <set>
#include <string>

enum Operator
{
	STAR = 0x01,	 // *
	PLUS = 0x02,	 // +
	QUES = 0x03,	 // ?
	SELECT = 0x04,	 // |
	CONCAT = 0x05,	 // ~
	PARENT_L = 0x06, // (
	PARENT_R = 0x07, // )
	UNI = 0x0e,		 // .
};

extern std::set<char> leftOp;

extern std::set<char> rightOp;

extern std::set<char> doubleOp;

extern std::set<char> opSet;

enum Symbol
{
	EPSILON = 0x00,	  // \e
	SET = 0x10,		  // []
	ALPHA = 0x12,	  // \a
	DIGIT = 0x13,	  // \d
	WORD = 0x14,	  // \w
	SPACE = 0x15,	  // \s
	NON_SPACE = 0x16, // \S
};

extern std::map<char, char> chr2op;

extern std::map<char, std::string> op2str;

extern std::map<char, std::string> sym2str;

extern std::set<char> uni_set;

extern std::set<char> alpha_set;

extern std::set<char> digit_set;

extern std::set<char> word_set;

extern std::set<char> space_set;

extern std::set<char> non_space_set;

std::string char2str(char c);

// 算符优先级
extern std::map<char, int> precedence;

extern std::map<char, char> escape;

/**
 * @brief 解析转义字符，返回对应的字符
 *
 * @param v Viewer
 * @return char
 */
char parseEscape(Viewer &v);

/**
 * @brief 将带有特殊不可见字符的表达式转换为可读的字符串
 *
 * @param v Viewer
 * @return string
 */
std::string exp2str(std::string exp);
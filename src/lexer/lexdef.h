/**
 * @file lexdef.h
 * @author Zhenjie Wei (2024108@bjtu.edu.cn)
 * @brief Lexical Definition Parser
 * @date 2023-04-22
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#pragma once
#include <map>
#include <set>
#include <string>
#include "viewer.h"

using namespace std;

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

extern set<char> leftOp;

extern set<char> rightOp;

extern set<char> doubleOp;

extern set<char> opSet;

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

extern map<char, char> chr2op;

extern map<char, string> op2str;

extern map<char, string> sym2str;

extern set<char> uni_set;

extern set<char> alpha_set;

extern set<char> digit_set;

extern set<char> word_set;

extern set<char> space_set;

extern set<char> non_space_set;

string char2str(char c);

// 算符优先级
extern map<char, int> precedence;

extern map<char, char> escape;

/**
 * @brief 解析转义字符，返回对应的字符
 *
 * @param v viewer
 * @return char
 */
char parseEscape(viewer &v);

/**
 * @brief 将带有特殊不可见字符的表达式转换为可读的字符串
 *
 * @param v viewer
 * @return string
 */
string exp2str(string exp);
/**
 * @file define.cpp
 * @author Zhenjie Wei (2024108@bjtu.edu.cn)
 * @brief Regular Expression Definition Parser
 * @date 2023-04-22
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#include "define.h"

set<char> leftOp = {
    STAR,
    PLUS,
    QUES,
    PARENT_R};

set<char> rightOp = {
    PARENT_L};

set<char> doubleOp = {
    SELECT,
    CONCAT,
};

set<char> opSet = {
    STAR,
    PLUS,
    QUES,
    SELECT,
    CONCAT,
    PARENT_L,
    PARENT_R,
};

map<char, char> chr2op = {
    {'*', STAR},
    {'+', PLUS},
    {'?', QUES},
    {'|', SELECT},
    {'^', CONCAT},
    {'(', PARENT_L},
    {')', PARENT_R},
    {'.', UNI},
};

map<char, string> op2str = {
    {STAR, "STAR"},
    {PLUS, "PLUS"},
    {QUES, "QUES"},
    {SELECT, "SELECT"},
    {CONCAT, "CONCAT"},
    {PARENT_L, "PARENT_L"},
    {PARENT_R, "PARENT_R"},
    {UNI, "UNI"},
};

map<char, string> sym2str = {
    {EPSILON, "EPSILON"},
    {SET, "SET"},
    {ALPHA, "ALPHA"},
    {DIGIT, "DIGIT"},
    {WORD, "WORD"},
    {SPACE, "SPACE"},
    {NON_SPACE, "NON_SPACE"},
};

set<char> uni_set = []() -> set<char>
{
    set<char> s;
    for (char c = 0x09; c <= 0x0d; c++)
    {
        s.insert(c);
    }
    for (char c = 0x20; c <= 0x7e; c++)
    {
        s.insert(c);
    }
    return s;
}();

set<char> alpha_set = []() -> set<char>
{
    set<char> s;
    for (char c = 'a'; c <= 'z'; c++)
    {
        s.insert(c);
    }
    for (char c = 'A'; c <= 'Z'; c++)
    {
        s.insert(c);
    }
    return s;
}();

set<char> digit_set = {
    '0',
    '1',
    '2',
    '3',
    '4',
    '5',
    '6',
    '7',
    '8',
    '9',
};

set<char> word_set = []() -> set<char>
{
    set<char> s = alpha_set;
    for (char c : digit_set)
    {
        s.insert(c);
    }
    s.insert('_');
    return s;
}();

set<char> space_set = {
    '\t',
    '\n',
    '\r',
    '\f',
    '\v',
    ' ',
};

set<char> non_space_set = []() -> set<char>
{
    set<char> s;
    s.insert(uni_set.begin(), uni_set.end());
    for (char c : space_set)
    {
        s.erase(c);
    }
    return s;
}();

string char2str(char c)
{
    if (op2str.find(c) != op2str.end())
    {
        return op2str[c];
    }
    else if (sym2str.find(c) != sym2str.end())
    {
        return sym2str[c];
    }
    else
    {
        return string(1, c);
    }
}

// 算符优先级
map<char, int> precedence = {
    {STAR, 3},
    {PLUS, 3},
    {QUES, 3},
    {CONCAT, 2},
    {SELECT, 1},
};

map<char, char> escape = {
    {'n', '\n'},
    {'t', '\t'},
    {'r', '\r'},
    {'f', '\f'},
    {'v', '\v'},
    {'\\', '\\'},
    {'.', '.'},
    {'^', '^'},
    {'?', '?'},
    {'*', '*'},
    {'+', '+'},
    {'|', '|'},
    {'(', '('},
    {')', ')'},
    {'[', '['},
    {']', ']'},
    {'e', EPSILON},
    {'a', ALPHA},
    {'d', DIGIT},
    {'w', WORD},
    {'s', SPACE},
    {'S', NON_SPACE},
};

/**
 * @brief 解析转义字符，返回对应的字符
 *
 * @param v viewer
 * @return char
 */
char parseEscape(viewer &v)
{
	char c = v.step();
	// 解析普通转义字符
	if (escape.find(c) != escape.end())
	{
		return escape[c];
	}
	// 解析16进制转义字符
	else if (c == 'x')
	{
		char c1 = v.step();
		char c2 = v.step();
		if (c1 >= '0' && c1 <= '9')
			c1 -= '0';
		else if (c1 >= 'a' && c1 <= 'f')
			c1 -= 'a' - 10;
		else if (c1 >= 'A' && c1 <= 'F')
			c1 -= 'A' - 10;
		else
			throw "Invalid escape sequence";
		if (c2 >= '0' && c2 <= '9')
			c2 -= '0';
		else if (c2 >= 'a' && c2 <= 'f')
			c2 -= 'a' - 10;
		else if (c2 >= 'A' && c2 <= 'F')
			c2 -= 'A' - 10;
		else
			throw "Invalid escape sequence";
		char result = c1 * 16 + c2;
		return result < 0x7f ? result : throw "Invalid escape sequence";
	}
	return c;
}

/**
 * @brief 将带有特殊不可见字符的表达式转换为可读的字符串
 *
 * @param v viewer
 * @return string
 */
string exp2str(string exp)
{
	string result;
	for (char c : exp)
	{
		if (op2str.find(c) != op2str.end())
		{
			result += "`" + op2str[c] + "`";
		}
		else if (sym2str.find(c) != sym2str.end())
		{
			result += "`" + sym2str[c] + "`";
		}
		else
		{
			result += string(1, c);
		}
	}
	return result;
}
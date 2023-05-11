/**
 * @file table.h
 * @author Zhenjie Wei (2024108@bjtu.edu.cn)
 * @brief My Custom Table Render
 * @date 2023-04-22
 *
 * @copyright Copyright (c) 2023
 *
 */

#pragma once

#include <string>
#include <queue>
#include <sstream>
#include <iostream>

#define _red(x) "\033[31m" << x << "\033[0m"
#define _green(x) "\033[32m" << x << "\033[0m"
#define _yellow(x) "\033[33m" << x << "\033[0m"
#define _blue(x) "\033[34m" << x << "\033[0m"

#define info std::cout << _green("[info] ")
#define warn std::cout << _yellow("[warn] ")
#define error std::cout << _red("[error] ")

#define DEBUG_LEVEL -1

#define debug(level)          \
	if (level <= DEBUG_LEVEL) \
	std::cout << _blue("   [" #level "] ")

#define debug_u(level)        \
	if (level <= DEBUG_LEVEL) \
	std::cout

class StrFormatter
{
	std::stringstream ss;
	std::queue<std::string> segs;

public:
	StrFormatter(std::string s);

	StrFormatter(const char *s) : StrFormatter(std::string(s)) {}

	template <typename T>
	StrFormatter &operator,(const T &t)
	{
		if (segs.empty())
			return *this;
		ss << segs.front();
		segs.pop();
		ss << t;
		return *this;
	}

	std::string str();
};

#define format(fmt, ...) ((StrFormatter(fmt), ##__VA_ARGS__).str())

inline void print_ln()
{
}
inline void print_ln(std::string s)
{
	std::cout << s << std::endl;
}

#define assert(x, msg)                                    \
	if (!(x))                                             \
	{                                                     \
		error << "Assertion \"" << #x << "\" FAILED at "; \
		std::cout << "\033[4m";                           \
		std::cout << __FILE__ << ":" << __LINE__;         \
		std::cout << "\033[0m" << std::endl;              \
		print_ln(msg);                                    \
		exit(1);                                          \
	}

template <typename T>
std::string container2str(T s, std::string sep = ", ", std::string lr = "{}")
{
	std::stringstream ss;
	if (lr.size() > 0)
		(ss << lr[0]);
	for (auto it = s.begin(); it != s.end(); it++)
	{
		if (it != s.begin())
			ss << sep;
		ss << *it;
	}
	if (lr.size() > 1)
		(ss << lr[1]);
	return ss.str();
}

#define set2str(s) container2str(s, ", ", "{}")
#define vec2str(s) container2str(s, ", ", "[]")
#define compact(s) container2str(s, "", "")


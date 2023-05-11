/**
 * @file log.h
 * @author Zhenjie Wei (2024108@bjtu.edu.cn)
 * @brief Level-based Log
 * @date 2023-04-22
 *
 * @copyright Copyright (c) 2023
 *
 */

#pragma once

#include <string>
#include <queue>
#include <vector>
#include <sstream>
#include <iostream>
#include <iomanip>
#include <algorithm>

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

enum t_align
{
	AL_LFT,
	AL_MID,
	AL_RGT,
};

#define TB_TAB "\t"
#define LF_TAB "\tl"
#define MD_TAB "\tm"
#define RT_TAB "\tr"
#define TB_GAP ""

class TableRender
{
	int rowCur;
	size_t colCur;
	size_t rowMax, colMax;
	std::vector<size_t> lines;
	std::vector<std::string> heads;
	std::vector<std::vector<std::string>> table;
	std::vector<size_t> widths;
	std::vector<t_align> aligns;

	std::string geneField(std::string s, size_t width, t_align align);
	std::string geneLine();

public:
	enum op_type
	{
		ADD_HEAD,
		ADD_FIELD,
		SET_ALIGN
	};
	op_type op;

	TableRender();
	TableRender &reset();
	TableRender &setOp(op_type op);
	TableRender &setHead(std::string title);
	TableRender &setAlign(t_align align);
	TableRender &curAlign(t_align align);
	TableRender &addField(std::string field);
	TableRender &resetColCur();
	TableRender &resetRowCur();
	TableRender &nextRow();
	TableRender &setLine();
	std::string geneView();
	TableRender &operator|(std::string field);
	TableRender &operator|(t_align align);
};

extern TableRender _table;

#define set_col _table.resetColCur().setOp(TableRender::SET_ALIGN)

#define set_row _table.nextRow().setOp(TableRender::ADD_FIELD)

#define tb_line _table.setLine()

#define tb_head _table.reset().setOp(TableRender::ADD_HEAD)

#define tb_view _table.geneView()

#define tb_cont _table

#define set_cur_col(x) _table.curAlign(x)
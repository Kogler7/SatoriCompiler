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
	StrFormatter(std::string s)
	{
		std::stringstream ss(s);
		std::string seg;
		while (std::getline(ss, seg, '$'))
			segs.push(seg);
	}

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

	std::string str()
	{
		while (!segs.empty())
		{
			ss << segs.front();
			segs.pop();
		}
		return ss.str();
	}
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
	ss << lr[0];
	for (auto it = s.begin(); it != s.end(); it++)
	{
		if (it != s.begin())
			ss << sep;
		ss << *it;
	}
	ss << lr[1];
	return ss.str();
}

#define set2str(s) container2str(s, ", ", "{}")
#define vec2str(s) container2str(s, ", ", "[]")

enum t_align
{
	AL_LFT,
	AL_MID,
	AL_RGT,
};

class TableRender
{
	int rowCur = -1;
	size_t colCur = 0;
	size_t rowMax = 0;
	size_t colMax = 0;
	std::vector<size_t> lines;
	std::vector<std::string> heads;
	std::vector<std::vector<std::string>> table;
	std::vector<size_t> widths;
	std::vector<t_align> aligns;

	std::string geneField(std::string s, size_t width, t_align align)
	{
		std::stringstream ss;
		if (align == AL_MID)
		{
			size_t l = (width - s.length()) / 2;
			size_t r = width - s.length() - l;
			ss << "| " << std::setw(l) << std::setfill(' ') << "";
			ss << s;
			ss << std::setw(r) << std::setfill(' ') << "";
			ss << " ";
			return ss.str();
		}
		else
		{
			ss << "| " << std::setw(width) << std::setfill(' ');
			switch (align)
			{
			case AL_LFT:
				ss << std::left;
				break;
			case AL_RGT:
				ss << std::right;
				break;
			}
			ss << s << " ";
		}
		return ss.str();
	}

	std::string geneLine()
	{
		std::stringstream ss;
		for (size_t i = 0; i < colMax; i++)
			ss << "+-" << std::string(widths[i], '-') << "-";
		ss << "+" << std::endl;
		return ss.str();
	}

public:
	enum op_type
	{
		ADD_HEAD,
		ADD_FIELD,
		SET_ALIGN
	};
	op_type op;

	TableRender()
	{
		op = ADD_HEAD;
		reset();
	}
	TableRender &reset()
	{
		rowCur = -1;
		colCur = 0;
		rowMax = 1;
		colMax = 0;
		lines.clear();
		heads.clear();
		table.clear();
		table.push_back(std::vector<std::string>());
		widths.clear();
		aligns.clear();
		return *this;
	}
	TableRender &setOp(op_type op)
	{
		this->op = op;
		return *this;
	}
	TableRender &setHead(std::string title)
	{
		if (colCur >= colMax)
		{
			colMax = colCur + 1;
			heads.resize(colMax);
			widths.resize(colMax);
			aligns.resize(colMax, AL_LFT);
			for (auto &row : table)
				row.resize(colMax);
		}
		heads[colCur] = title;
		widths[colCur] = std::max(widths[colCur], title.size());
		colCur++;
		return *this;
	}
	TableRender &setAlign(t_align align)
	{
		assert(colCur < colMax, "Column index out of range.");
		aligns[colCur] = align;
		colCur++;
		return *this;
	}
	TableRender &addField(std::string field)
	{
		assert(colCur < colMax, "Column index out of range.");
		assert(rowCur >= 0 && rowCur < rowMax, "Row index out of range.");
		table[rowCur][colCur] = field;
		widths[colCur] = std::max(widths[colCur], field.size());
		colCur++;
		return *this;
	}
	TableRender &resetColCur()
	{
		colCur = 0;
		return *this;
	}
	TableRender &resetRowCur()
	{
		rowCur = 0;
		return *this;
	}
	TableRender &nextRow()
	{
		rowCur++;
		colCur = 0;
		if (rowCur >= rowMax)
		{
			rowMax = rowCur + 1;
			table.resize(rowMax);
			for (size_t i = 0; i < rowMax; i++)
				table[i].resize(colMax);
		}
		return *this;
	}
	TableRender &setLine()
	{
		assert(rowCur < rowMax, "Row index out of range.");
		lines.push_back(rowCur);
		return *this;
	}
	std::string geneView()
	{
		std::stringstream ss;
		ss << geneLine();
		for (size_t i = 0; i < colMax; i++)
		{
			ss << geneField(heads[i], widths[i], aligns[i]);
		}
		ss << "|" << std::endl;
		ss << geneLine();
		for (size_t i = 0; i < rowMax; i++)
		{
			for (size_t j = 0; j < colMax; j++)
			{
				ss << geneField(table[i][j], widths[j], aligns[j]);
			}
			ss << "|" << std::endl;
			if (std::find(lines.begin(), lines.end(), i) != lines.end())
				ss << geneLine();
		}
		ss << geneLine();
		return ss.str();
	}
	TableRender &operator<<(std::string field)
	{
		switch (op)
		{
		case ADD_HEAD:
			setHead(field);
			break;
		case ADD_FIELD:
			addField(field);
			break;
		default:
			warn << "Invalid operation type." << std::endl;
			break;
		}
		return *this;
	}
	TableRender &operator<<(t_align align)
	{
		switch (op)
		{
		case SET_ALIGN:
			setAlign(align);
			break;
		default:
			warn << "Invalid operation type." << std::endl;
			break;
		}
		return *this;
	}
};

extern TableRender _table;

#define t_col _table.resetColCur().setOp(TableRender::SET_ALIGN)

#define t_row _table.nextRow().setOp(TableRender::ADD_FIELD)

#define t_line _table.setLine()

#define t_head _table.reset().setOp(TableRender::ADD_HEAD)

#define t_view _table.geneView()
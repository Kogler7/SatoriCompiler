/**
 * @file viewer.cpp
 * @author Zhenjie Wei (2024108@bjtu.edu.cn)
 * @brief Lightweight String Viewer
 * @date 2023-04-22
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#pragma once
#include <string>
#include <vector>
#include <iomanip>
#include <iostream>

using namespace std;

/**
 * @brief lightweight string viewer
 */
class viewer
{
	string str;
	int pos = 0;

public:
	viewer(string str) : str(str) {}
	void operator=(viewer &v)
	{
		str = v.str;
		pos = v.pos;
	}
	bool operator>(viewer &v)
	{
		return pos > v.pos;
	}
	bool operator>=(viewer &v)
	{
		return pos >= v.pos;
	}
	// 获取第i个字符
	char operator[](int i)
	{
		if (i < 0 || i >= str.size())
		{
			return '\0';
		}
		return str[i];
	}
	// 获取字符串大小
	int size()
	{
		return str.size();
	}
	// 获取当前字符
	char peek(int i = 0)
	{
		return (*this)[pos + i];
	}
	// 获取当前字符并移动指针
	char step()
	{
		return (*this)[pos++];
	}
	// 移动指针并获取当前字符
	char next()
	{
		return (*this)[++pos];
	}
	// 判断是否到达末尾
	bool ends()
	{
		return pos >= str.size();
	}
	// 跳过i个字符（可正可负）
	void skip(int i = 1)
	{
		pos += i;
	}
	// 获取当前位置
	int getPos()
	{
		return pos;
	}
	// 获取字符串
	string getStr()
	{
		return str;
	}
};

class code_viewer : public viewer
{
	vector<int> lineNoVec;

public:
	void operator=(viewer &v)
	{
		viewer::operator=(v);
	}
	code_viewer(string str) : viewer(str)
	{
		auto pos = str.find('\n');
		while (pos != string::npos)
		{
			lineNoVec.push_back(pos);
			pos = str.find('\n', pos + 1);
		}
	}
	pair<int, int> getCurLineCol()
	{
		int ln = getLineNo();
		int col = getPos() - (ln > 1 ? lineNoVec[ln - 2] + 1 : 0);
		return make_pair(ln, col);
	}
	string getLine(int lineNo = -1)
	{
		if (lineNo == -1)
			lineNo = getLineNo();
		int start = lineNo > 1 ? lineNoVec[lineNo - 2] + 1 : 0;
		int end = lineNoVec[lineNo - 1];
		return getStr().substr(start, end - start);
	}
	int getLineNo()
	{
		int _pos = getPos();
		int lineNo = 1;
		for (auto i : lineNoVec)
		{
			if (_pos > i)
				lineNo++;
			else
				break;
		}
		return lineNo;
	}
	void printCodeCtx()
	{
		int lineNo = getLineNo();
		int startLine = lineNo - 1 > 0 ? lineNo - 1 : 1;
		int endLine = lineNo + 1 < size() ? lineNo + 1 : size();
		for (int i = startLine; i <= endLine; i++)
		{
			cout << setw(3) << i << ": ";
			if (i == lineNo)
				cout << ">>| ";
			else
				cout << "  | ";
			cout << getLine(i) << endl;
			if (i == lineNo)
				cout << setw(9 + getPos() - lineNoVec[lineNo - 2]) << "^" << endl;
		}
	}
	void skipToNextLine()
	{
		int lineNo = getLineNo();
		skip(lineNoVec[lineNo - 1] - getPos() + 1);
	}
};
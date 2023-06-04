/**
 * @file utils/viewer.h
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
#include <fstream>
#include <iostream>

using namespace std;

/**
 * @brief lightweight string viewer
 */
class Viewer
{
protected:
	string str;
	size_t pos = 0;

public:
	static Viewer fromFile(string filename)
	{
		ifstream ifs(filename);
		string str((istreambuf_iterator<char>(ifs)), istreambuf_iterator<char>());
		return Viewer(str);
	}
	Viewer(string &str) : str(str) {}
	Viewer(Viewer &v)
	{
		str = v.str;
		pos = v.pos;
	}
	void operator=(Viewer v)
	{
		str = v.str;
		pos = v.pos;
	}
	bool operator>(Viewer &v)
	{
		return pos > v.pos;
	}
	bool operator>=(Viewer &v)
	{
		return pos >= v.pos;
	}
	// 获取第i个字符
	char operator[](size_t i)
	{
		if (i >= str.size())
		{
			return '\0';
		}
		return str[i];
	}
	// 获取字符串大小
	size_t size()
	{
		return str.size();
	}
	// 获取当前字符
	char peek(size_t i = 0)
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
	// 跳转到指定位置
	void jump(size_t i)
	{
		pos = i;
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

/**
 * @file utils/context_viewer.h
 * @author Zhenjie Wei (2024108@bjtu.edu.cn)
 * @brief Lightweight Context Viewer
 * @date 2023-06-03
 *
 * @copyright Copyright (c) 2023
 *
 */

#pragma once

#include "viewer.h"

class ContextViewer : public Viewer
{
	vector<int> lineNoVec;

public:
	void operator=(Viewer &v)
	{
		Viewer::operator=(v);
	}
	ContextViewer(string str) : Viewer(str)
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
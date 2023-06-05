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
#include "utils/log.h"

using code_loc_t = pair<size_t, size_t>;

class ContextViewer : public Viewer
{
	vector<size_t> lineNoVec;
	void initialize()
	{
		size_t pos = str.find('\n');
		while (pos != string::npos)
		{
			lineNoVec.push_back(pos);
			pos = str.find('\n', pos + 1);
		}
		lineNoVec.push_back(str.size());
	}

public:
	void operator=(Viewer &v)
	{
		Viewer::operator=(v);
	}

	ContextViewer(string str) : Viewer(str)
	{
		initialize();
	}

	ContextViewer(const Viewer &v) : Viewer(v)
	{
		initialize();
	}

	pair<size_t, size_t> getCurLineCol() const
	{
		size_t ln = getLineNo();
		size_t col = getPos() - (ln > 1 ? lineNoVec[ln - 2] + 1 : 0);
		return make_pair(ln, col);
	}

	string getLine(size_t lineNo = -1) const
	{
		if (lineNo == -1)
			lineNo = getLineNo();
		size_t start = lineNo > 1 ? lineNoVec[lineNo - 2] + 1 : 0;
		size_t end = lineNoVec[lineNo - 1];
		return str.substr(start, end - start);
	}

	size_t getLineNo() const
	{
		size_t lineNo = 1;
		for (auto i : lineNoVec)
		{
			if (pos > i)
				lineNo++;
			else
				break;
		}
		return lineNo;
	}

	code_loc_t getLnAndCol() const
	{
		size_t lineNo = getLineNo();
		size_t start = lineNo > 1 ? lineNoVec[lineNo - 2] : 0;
		return make_pair(lineNo, pos - start);
	}

	void printContext() const
	{
		size_t ln, col;
		tie(ln, col) = getLnAndCol();
		printContext(ln, col);
	}

	void printContext(const size_t ln, const size_t col) const
	{
		size_t startLine = ln - 1 > 0 ? ln - 1 : 1;
		size_t lineMax = lineNoVec.size();
		size_t endLine = ln + 1 < lineMax ? ln + 1 : lineMax;
		for (size_t i = startLine; i <= endLine; i++)
		{
			cout << setw(3) << i << ": ";
			if (i == ln)
				cout << ">>| ";
			else
				cout << "  | ";
			cout << getLine(i) << endl;
			if (i == ln)
				cout << setw(9 + col) << "^" << endl;
		}
	}

	void skipToNextLine()
	{
		size_t lineNo = getLineNo();
		skip(lineNoVec[lineNo - 1] - getPos() + 1);
	}
};
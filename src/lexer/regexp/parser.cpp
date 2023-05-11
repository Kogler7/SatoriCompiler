/**
 * @file regexp/parser.cpp
 * @author Zhenjie Wei (2024108@bjtu.edu.cn)
 * @brief Regular Expression to Finite Automaton
 * @date 2023-04-22
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#include "parser.h"
#include "utils/log.h"
#include "define.h"

#define _find(x, y) (x.find(y) != x.end())

string RegexpParser::getStackDesc()
{
	stack<sub_nfa> tmpStack = nfaStack;
	vector<sub_nfa> tmpVec;
	while (!tmpStack.empty())
	{
		tmpVec.push_back(tmpStack.top());
		tmpStack.pop();
	}
	string desc = "state stack: ";
	// 从栈底开始打印
	for (auto it = tmpVec.rbegin(); it != tmpVec.rend(); it++)
	{
		sub_nfa state = *it;
		int st = state.first;
		int ed = state.second;
		bool stFinal = nfa.getStates()[st].isFinal;
		bool edFinal = nfa.getStates()[ed].isFinal;
		char buf[16];
		sprintf_s(buf, "(%d%s, %d%s) ", st, stFinal ? "*" : "", ed, edFinal ? "*" : "");
		desc += buf;
	}
	return desc;
}

FiniteAutomaton RegexpParser::parse()
{
	regexpPreproc();
	tmpReg2postfix();
	postfix2FA();
	return nfa;
}

void RegexpParser::parseSet(string setExp)
{
	debug(0) << "parse set: " << setExp << endl;
	Viewer tmpView(setExp);
	// 解析形如[a-zA-Z0-9_]的正则表达式
	int st = nfa.addState();
	int ed = nfa.addState(true);
	string pExpStr = "";
	bool reverse = false; // ^ 取反
	if (tmpView.peek() == '^')
	{
		reverse = true;
		tmpView.skip(1);
	}
	// 预处理
	while (!tmpView.ends())
	{
		char c = tmpView.step();
		if (c == '\\')
		{
			c = parseEscape(tmpView);
			pExpStr.push_back(c);
		}
		else
		{
			pExpStr.push_back(c);
		}
	}
	Viewer view(pExpStr);
	set<char> tmpSet;
	if (reverse)
	{
		tmpSet.insert(uni_set.begin(), uni_set.end()); // 插入全集
	}
	while (!view.ends())
	{
		char c = view.step();
		if (view.peek() == '-' && view.peek(1) >= c)
		{
			char e = view.peek(1);
			for (char t = c; t <= e; t++)
			{
				if (reverse)
					tmpSet.erase(t);
				else if (uni_set.find(t) != uni_set.end())
					tmpSet.insert(t);
				else
				{
					error << "invalid character in set: (int)" << int(t) << endl;
					exit(1);
				}
			}
			view.skip(2);
		}
		else if (c == WORD)
		{
			if (reverse)
				tmpSet.erase(word_set.begin(), word_set.end());
			else
				tmpSet.insert(word_set.begin(), word_set.end());
		}
		else if (c == ALPHA)
		{
			if (reverse)
				tmpSet.erase(alpha_set.begin(), alpha_set.end());
			else
				tmpSet.insert(alpha_set.begin(), alpha_set.end());
		}
		else if (c == DIGIT)
		{
			if (reverse)
				tmpSet.erase(digit_set.begin(), digit_set.end());
			else
				tmpSet.insert(digit_set.begin(), digit_set.end());
		}
		else if (c == SPACE)
		{
			if (reverse)
				tmpSet.erase(space_set.begin(), space_set.end());
			else
				tmpSet.insert(space_set.begin(), space_set.end());
		}
		else if (c == NON_SPACE)
		{
			if (reverse)
				tmpSet.erase(non_space_set.begin(), non_space_set.end());
			else
				tmpSet.insert(non_space_set.begin(), non_space_set.end());
		}
		else
		{
			if (reverse)
				tmpSet.erase(c);
			else if (uni_set.find(c) != uni_set.end())
				tmpSet.insert(c);
			else
			{
				error << "invalid character in set: (int)" << int(c) << endl;
				exit(1);
			}
		}
	}
	// 将tmpSet中的字符转换为转换函数
	for (const auto c : tmpSet)
	{
		nfa.addTransition(st, ed, c);
	}
	setStates.push_back(st);
	setStates.push_back(ed);
}

void RegexpParser::opSelect()
{
	// (st1, ed1) (st2, ed2) | => (nst, ned)
	sub_nfa sub2 = nfaStack.top();
	int ed2 = sub2.second;
	int st2 = sub2.first;
	nfa.setState(ed2, false);
	nfaStack.pop();
	sub_nfa sub1 = nfaStack.top();
	int ed1 = sub1.second;
	int st1 = sub1.first;
	nfa.setState(ed1, false);
	nfaStack.pop();
	int nst = nfa.addState();
	int ned = nfa.addState(true);
	nfa.addTransition(nst, st1, EPSILON);
	nfa.addTransition(nst, st2, EPSILON);
	nfa.addTransition(ed1, ned, EPSILON);
	nfa.addTransition(ed2, ned, EPSILON);
	nfaStack.push(make_pair(nst, ned));
}

void RegexpParser::opStar()
{
	// (st, ed) * => (nst, ned)
	sub_nfa sub = nfaStack.top();
	int ed = sub.second;
	int st = sub.first;
	nfa.setState(ed, false);
	nfaStack.pop();
	int nst = nfa.addState();
	int ned = nfa.addState(true);
	nfa.addTransition(nst, st, EPSILON);
	nfa.addTransition(nst, ned, EPSILON);
	nfa.addTransition(ed, st, EPSILON);
	nfa.addTransition(ed, ned, EPSILON);
	nfaStack.push(make_pair(nst, ned));
}

void RegexpParser::opPlus()
{
	// (st, ed) + => (nst, ned)
	sub_nfa sub = nfaStack.top();
	int ed = sub.second;
	int st = sub.first;
	nfa.setState(ed, false);
	nfaStack.pop();
	int nst = nfa.addState();
	int ned = nfa.addState(true);
	nfa.addTransition(ed, st, EPSILON);
	nfa.addTransition(nst, st, EPSILON);
	nfa.addTransition(ed, ned, EPSILON);
	nfaStack.push(make_pair(nst, ned));
}

void RegexpParser::opQues()
{
	// (st, ed) ? => (nst, ned)
	sub_nfa sub = nfaStack.top();
	int ed = sub.second;
	int st = sub.first;
	nfa.setState(ed, false);
	nfaStack.pop();
	int nst = nfa.addState();
	int ned = nfa.addState(true);
	nfa.addTransition(nst, st, EPSILON);
	nfa.addTransition(nst, ned, EPSILON);
	nfa.addTransition(ed, ned, EPSILON);
	nfaStack.push(make_pair(nst, ned));
}

void RegexpParser::opConcat()
{
	// (st1, ed1) (st2, ed2) ~ => (st1, ed2)
	sub_nfa sub2 = nfaStack.top();
	int ed2 = sub2.second;
	int st2 = sub2.first;
	nfaStack.pop();
	sub_nfa sub1 = nfaStack.top();
	int ed1 = sub1.second;
	int st1 = sub1.first;
	nfa.setState(ed1, false);
	nfaStack.pop();
	nfa.addTransition(ed1, st2, EPSILON);
	nfaStack.push(make_pair(st1, ed2));
}

void RegexpParser::regexpPreproc()
{
	info << "regexpPreproc: " << exp2str(rawReg) << endl;
	Viewer rawView(rawReg);
	string tmpStr;
	while (!rawView.ends())
	{
		char c = rawView.step();
		if (c == '\\')
		{
			c = parseEscape(rawView);
			tmpStr.push_back(c);
		}
		else if (c == '[')
		{
			int st = rawView.getPos();
			while (!rawView.ends() && (rawView.step() != ']' || rawView.peek(-2) == '\\'))
				;
			string setExp = rawReg.substr(st, rawView.getPos() - st - 1);
			parseSet(setExp);
			tmpStr.push_back(SET);
		}
		else if (chr2op.find(c) != chr2op.end()) // operator
		{
			tmpStr.push_back(chr2op[c]);
		}
		else // plain symbol
		{
			tmpStr.push_back(c);
		}
	}
	tmpReg.clear();
	Viewer tmpView(tmpStr);
	// 为了方便处理，将正则表达式中的所有非操作符字符之间插入连接符
	while (!tmpView.ends())
	{
		char c = tmpView.step();
		// 所有非操作符字符之间插入连接符
		if (!tmpView.ends() && !_find(opSet, c) && !_find(opSet, tmpView.peek()))
		{
			debug(1) << "insert concat between two symbols " << endl;
			tmpReg.push_back(c);
			tmpReg.push_back(CONCAT);
		}
		// + ? * ) 右边插入连接符
		else if (!tmpView.ends() && _find(leftOp, c) && !_find(leftOp, tmpView.peek()) && !_find(doubleOp, tmpView.peek()))
		{
			debug(1) << "insert concat after + ? * ) " << endl;
			tmpReg.push_back(c);
			tmpReg.push_back(CONCAT);
		}
		// ( 左边插入连接符
		else if (tmpView.getPos() > 1 && _find(rightOp, c) && !_find(rightOp, tmpView.peek(-2)) && !_find(doubleOp, tmpView.peek(-2)))
		{
			debug(1) << "insert concat before ( " << endl;
			if (tmpReg.back() != CONCAT)
			{
				tmpReg.push_back(CONCAT);
			}
			tmpReg.push_back(c);
		}
		else
		{
			tmpReg.push_back(c);
		}
	}
}

void RegexpParser::tmpReg2postfix()
{
	info << "tmpReg2postfix: " << exp2str(tmpReg) << endl;
	stack<char> opStack;
	try
	{
		for (int i = 0; i < tmpReg.size(); i++)
		{
			char c = tmpReg[i];
			if (op2str.find(c) != op2str.end() && c != UNI) // operator
			{
				if (c == STAR || c == PLUS || c == QUES) // single operand operators
				{
					postfix.push_back(c);
				}
				else if (c == PARENT_L)
				{
					opStack.push(c);
				}
				else if (c == PARENT_R)
				{
					while (opStack.top() != PARENT_L)
					{
						postfix.push_back(opStack.top());
						opStack.pop();
					}
					opStack.pop();
				}
				else // binary operand operators
				{
					while (!opStack.empty() && precedence[opStack.top()] >= precedence[c])
					{
						postfix.push_back(opStack.top());
						opStack.pop();
					}
					opStack.push(c);
				}
			}
			else // symbol
			{
				postfix.push_back(c);
			}
		}
		while (!opStack.empty())
		{
			postfix.push_back(opStack.top());
			opStack.pop();
		}
	}
	catch (const exception &e)
	{
		error << "tmpReg2postfix failed. Make sure the regexp is valid." << endl;
		throw e;
	}
}

void RegexpParser::postfix2FA()
{
	info << "postfix2FA: " << exp2str(postfix) << endl;
	int setIdx = 0;
	for (char c : postfix)
	{
		if (c == SET) // 处理插入的SET操作符
		{
			debug(0) << "postfix2FA: Push SET" << endl;
			if (setIdx + 1 >= setStates.size())
			{
				warn << "setIdx out of range" << endl;
				return;
			}
			int st = setStates[setIdx++];
			int ed = setStates[setIdx++];
			sub_nfa subNFA = make_pair(st, ed);
			nfaStack.push(subNFA);
		}
		else if (c == UNI) // 处理插入的UNI操作符
		{
			debug(0) << "postfix2FA: Executing UNI" << endl;
			int st = nfa.addState(false);
			int ed = nfa.addState(true);
			for (auto &c : uni_set)
			{
				nfa.addTransition(st, ed, c);
			}
			sub_nfa subNFA = make_pair(st, ed);
			nfaStack.push(subNFA);
		}
		else if (c == WORD)
		{
			debug(0) << "postfix2FA: Executing WORD" << endl;
			int st = nfa.addState(false);
			int ed = nfa.addState(true);
			for (auto &c : word_set)
			{
				nfa.addTransition(st, ed, c);
			}
			sub_nfa subNFA = make_pair(st, ed);
			nfaStack.push(subNFA);
		}
		else if (c == ALPHA)
		{
			debug(0) << "postfix2FA: Push ALPHA" << endl;
			int st = nfa.addState(false);
			int ed = nfa.addState(true);
			for (auto &c : alpha_set)
			{
				nfa.addTransition(st, ed, c);
			}
			sub_nfa subNFA = make_pair(st, ed);
			nfaStack.push(subNFA);
		}
		else if (c == DIGIT)
		{
			debug(0) << "postfix2FA: Push DIGIT" << endl;
			int st = nfa.addState(false);
			int ed = nfa.addState(true);
			for (auto &c : digit_set)
			{
				nfa.addTransition(st, ed, c);
			}
			sub_nfa subNFA = make_pair(st, ed);
			nfaStack.push(subNFA);
		}
		else if (c == SPACE)
		{
			debug(0) << "postfix2FA: Push SPACE" << endl;
			int st = nfa.addState(false);
			int ed = nfa.addState(true);
			for (auto &c : space_set)
			{
				nfa.addTransition(st, ed, c);
			}
			sub_nfa subNFA = make_pair(st, ed);
			nfaStack.push(subNFA);
		}
		else if (c == NON_SPACE)
		{
			debug(0) << "postfix2FA: Push NON_SPACE" << endl;
			int st = nfa.addState(false);
			int ed = nfa.addState(true);
			for (auto &c : non_space_set)
			{
				nfa.addTransition(st, ed, c);
			}
			sub_nfa subNFA = make_pair(st, ed);
			nfaStack.push(subNFA);
		}
		else if (op2str.find(c) != op2str.end() && c != UNI) // operator
		{
			debug(0) << "postfix2FA: Executing " << op2str[c] << endl;
			switch (c)
			{
			case CONCAT:
				opConcat();
				break;
			case QUES:
				opQues();
				break;
			case STAR:
				opStar();
				break;
			case PLUS:
				opPlus();
				break;
			case SELECT:
				opSelect();
				break;
			}
		}
		else // symbol
		{
			debug(0) << "postfix2FA: Push <" << c << '>' << endl;
			int st = nfa.addState(false);
			int ed = nfa.addState(true);
			nfa.addTransition(st, ed, c);
			sub_nfa subNFA = make_pair(st, ed);
			nfaStack.push(subNFA);
		}
		debug(0) << getStackDesc() << endl;
	}
	debug(0) << getStackDesc() << endl;
	sub_nfa subNFA = nfaStack.top();
	nfaStack.pop();
	nfa.setStartState(subNFA.first);
	if (nfaStack.empty())
		info << "postfix2FA: Success" << endl;
	else
	{
		error << "postfix2FA: Failed" << endl;
		exit(1);
	}
}
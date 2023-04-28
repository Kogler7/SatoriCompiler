/**
 * @file regexp.h
 * @author Zhenjie Wei (2024108@bjtu.edu.cn)
 * @brief Regular Expression to Finite Automaton
 * @date 2023-04-22
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#pragma once
#include <stack>
#include <string>
#include <vector>
#include "nfa.h"

class Regexp2FA
{
	string rawReg;
	string tmpReg;
	string postfix;
	stack<sub_nfa> nfaStack;
	vector<int> setStates;
	FiniteAutomaton nfa;

	void parseSet(string setExp);

	void opSelect();
	void opStar();
	void opPlus();
	void opQues();
	void opConcat();

public:
	Regexp2FA(string regexp) : rawReg(regexp) {}
	void regexpPreproc();
	void tmpReg2postfix();
	void postfix2FA();
	string getStackDesc();
	string getRawReg() { return rawReg; }
	string getTmpReg() { return tmpReg; }
	string getPostfix() { return postfix; }
	FiniteAutomaton getNFA() { return nfa; }
	FiniteAutomaton convert();
};
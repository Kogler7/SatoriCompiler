/**
 * @file parser.h
 * @author Zhenjie Wei (2024108@bjtu.edu.cn)
 * @brief Regular Expression to Finite Automaton
 * @date 2023-04-22
 *
 * @copyright Copyright (c) 2023
 *
 */

#pragma once

#include "lexer/nfa.h"

#include <stack>
#include <string>
#include <vector>

class RegexpParser
{
	std::string rawReg;
	std::string tmpReg;
	std::string postfix;
	std::stack<sub_nfa_t> nfaStack;
	std::vector<int> setStates;
	FiniteAutomaton nfa;

	void parseSet(std::string setExp);

	void opSelect();
	void opStar();
	void opPlus();
	void opQues();
	void opConcat();

public:
	RegexpParser(std::string regexp) : rawReg(regexp) {}
	void regexpPreproc();
	void tmpReg2postfix();
	void postfix2FA();
	std::string getStackDesc() const;
	std::string getRawReg() const { return rawReg; }
	std::string getTmpReg() const { return tmpReg; }
	std::string getPostfix() const { return postfix; }
	FiniteAutomaton getNFA() const { return nfa; }
	FiniteAutomaton parse();
};
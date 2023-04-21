#include "nfa.h"
#include "lexdef.h"
#include "../utils/log.h"

void FiniteAutomaton::setStartState(int id)
{
	startState = id;
}

/**
 * @brief 从指定状态开始匹配，递归地匹配每个字符，直到无法匹配为止
 *
 * @param view 		所需匹配的字符串视图，每次匹配一个字符，匹配成功后视图向后移动一位
 * @param result 	匹配成功后的结果，如果匹配失败则为""
 * @param start 	起始状态
 * @return true		匹配成功
 * @return false	匹配失败
 */
bool FiniteAutomaton::accepts(viewer &view, string &result, int start)
{
	if (start == -1) // 未指定起始状态，使用默认起始状态
		start = startState;
	if (states[start].isFinal) // 如果当前状态为终态，返回true
	{
		// cout << "reached final state: " << start << endl;
		return true;
	}
	viewer subView = view; // 保存当前视图，用于回溯
	if (transitions.find(start) != transitions.end())
	{
		char c = subView.step(); // 获取当前字符，视图向后移动一位
		transition_map available = transitions[start];
		bool resFlag = false;
		string tmpRes;			  // 保存当前匹配结果
		viewer tmpView = subView; // 保存当前视图，用于回溯
		if (available.find(c) != available.end())
		{
			set<int> nextSet = available[c];
			for (auto i : nextSet)
			{
				string nxtRes;
				viewer vNext = subView;
				if (accepts(vNext, nxtRes, i) && vNext >= tmpView)
				{
					resFlag = true;
					tmpRes = nxtRes;
					// cout << "<" << start << ">nxtRes: " << nxtRes << endl;
					tmpView = vNext;
				}
				// cout << "<" << start << ">accepts: " << c << " " << resFlag << endl;
			}
			if (resFlag)
			{
				result = c + tmpRes;
				// cout << "<" << start << ">concat: " << c << " " << tmpRes << endl;
				view = tmpView;
				return true;
			}
		}
		if (c != EPSILON && available.find(EPSILON) != available.end())
		{
			subView.skip(-1); // EPSILON转移不消耗字符，视图回退一位
			tmpView = subView;
			set<int> nextSet = available[EPSILON];
			for (auto i : nextSet)
			{
				string nxtRes;
				viewer vNext = subView;
				if (accepts(vNext, nxtRes, i) && vNext >= tmpView)
				{
					resFlag = true;
					tmpRes = nxtRes;
					// cout << "<" << start << ">EPSILONnxtRes: " << nxtRes << endl;
					tmpView = vNext;
				}
				// cout << "<" << start << ">view: " << vNext.getPos() << " " << tmpView.getPos() << " " << subView.getPos() << endl;
				// cout << "<" << start << ">accepts: "
				// 		  << "EPSILON " << resFlag << endl;
			}
			if (resFlag)
			{
				result = tmpRes;
				view = tmpView;
				return true;
			}
		}
		// if (states[start].isFinal) // 如果当前状态为终态，返回true
		// 	return true;
	}
	return false;
}

void FiniteAutomaton::setState(int id, bool isFinal)
{
	states[id].isFinal = isFinal;
}

void FiniteAutomaton::print()
{
	// 打印状态
	info << "States: ";
	for (const auto &state : states)
	{
		cout << state.id;
		if (state.id == startState) // 标记开始状态
		{
			cout << "^";
		}
		else if (state.isFinal) // 标记终态
		{
			cout << "*";
		}
		cout << " ";
	}
	cout << endl;

	// 打印转移关系
	info << "Transitions:" << endl;
	for (const auto &transition : transitions)
	{
		int from = transition.first;
		for (const auto &symbol : transition.second)
		{
			char c = symbol.first;
			const set<int> &to = symbol.second;
			for (const auto &t : to)
			{
				cout << "    " << from << " --" << char2str(c) << "(" << int(c) << ")--> " << t << endl;
			}
		}
	}
}

int FiniteAutomaton::addState(bool isFinal)
{
	int id = states.size();
	states.push_back(State(id, isFinal));
	debug(1) << "add state: " << id << endl;
	return id;
}

void FiniteAutomaton::addTransition(int from, int to, char symbol)
{
	debug(1) << "add transition: " << from << " --" << char2str(symbol) << "--> " << to << endl;
	transitions[from][symbol].insert(to);
}
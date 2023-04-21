#include <set>
#include <map>
#include <stack>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <unordered_map>
#include <string_view>
#include <iomanip>

using namespace std;

#define _find(x, y) (x.find(y) != x.end())

#define _red(x) "\033[31m" << x << "\033[0m"
#define _green(x) "\033[32m" << x << "\033[0m"
#define _yellow(x) "\033[33m" << x << "\033[0m"
#define _blue(x) "\033[34m" << x << "\033[0m"

#define info cout << _green("[info] ")
#define warn cout << _yellow("[warn] ")
#define error cout << _red("[error] ")

#define DEBUG_LEVEL 3
#define debug(level)          \
	if (level <= DEBUG_LEVEL) \
	cout << _blue("    <" #level ">: ")

typedef pair<int, int> sub_nfa;
typedef unordered_map<char, set<int>> transition_map;

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

enum Operator
{
	STAR = 0x01,	 // *
	PLUS = 0x02,	 // +
	QUES = 0x03,	 // ?
	SELECT = 0x04,	 // |
	CONCAT = 0x05,	 // ~
	PARENT_L = 0x06, // (
	PARENT_R = 0x07, // )
	UNI = 0x0e,		 // .
};

set<char> leftOp{
	STAR,
	PLUS,
	QUES,
	PARENT_R};

set<char> rightOp{
	PARENT_L};

set<char> doubleOp{
	SELECT,
	CONCAT,
};

set<char> opSet = {
	STAR,
	PLUS,
	QUES,
	SELECT,
	CONCAT,
	PARENT_L,
	PARENT_R,
};

enum Symbol
{
	EPSILON = 0x00,	  // \e
	SET = 0x10,		  // []
	ALPHA = 0x12,	  // \a
	DIGIT = 0x13,	  // \d
	WORD = 0x14,	  // \w
	SPACE = 0x15,	  // \s
	NON_SPACE = 0x16, // \S
};

map<char, char> chr2op{
	{'*', STAR},
	{'+', PLUS},
	{'?', QUES},
	{'|', SELECT},
	{'^', CONCAT},
	{'(', PARENT_L},
	{')', PARENT_R},
	{'.', UNI},
};

map<char, string> op2str = {
	{STAR, "STAR"},
	{PLUS, "PLUS"},
	{QUES, "QUES"},
	{SELECT, "SELECT"},
	{CONCAT, "CONCAT"},
	{PARENT_L, "PARENT_L"},
	{PARENT_R, "PARENT_R"},
	{UNI, "UNI"},
};

map<char, string> sym2str = {
	{EPSILON, "EPSILON"},
	{SET, "SET"},
	{ALPHA, "ALPHA"},
	{DIGIT, "DIGIT"},
	{WORD, "WORD"},
	{SPACE, "SPACE"},
	{NON_SPACE, "NON_SPACE"},
};

set<char> uni_set = []() -> set<char>
{
	set<char> s;
	for (char c = 0x09; c <= 0x0d; c++)
	{
		s.insert(c);
	}
	for (char c = 0x20; c <= 0x7e; c++)
	{
		s.insert(c);
	}
	return s;
}();

set<char> alpha_set = []() -> set<char>
{
	set<char> s;
	for (char c = 'a'; c <= 'z'; c++)
	{
		s.insert(c);
	}
	for (char c = 'A'; c <= 'Z'; c++)
	{
		s.insert(c);
	}
	return s;
}();

set<char> digit_set = {
	'0',
	'1',
	'2',
	'3',
	'4',
	'5',
	'6',
	'7',
	'8',
	'9',
};

set<char> word_set = []() -> set<char>
{
	set<char> s = alpha_set;
	for (char c : digit_set)
	{
		s.insert(c);
	}
	s.insert('_');
	return s;
}();

set<char> space_set = {
	'\t',
	'\n',
	'\r',
	'\f',
	'\v',
	' ',
};

set<char> non_space_set = []() -> set<char>
{
	set<char> s;
	s.insert(uni_set.begin(), uni_set.end());
	for (char c : space_set)
	{
		s.erase(c);
	}
	return s;
}();

string char2str(char c)
{
	if (op2str.find(c) != op2str.end())
	{
		return op2str[c];
	}
	else if (sym2str.find(c) != sym2str.end())
	{
		return sym2str[c];
	}
	else
	{
		return string(1, c);
	}
}

// 算符优先级
map<char, int> precedence = {
	{STAR, 3},
	{PLUS, 3},
	{QUES, 3},
	{CONCAT, 2},
	{SELECT, 1},
};

map<char, char> escape = {
	{'n', '\n'},
	{'t', '\t'},
	{'r', '\r'},
	{'f', '\f'},
	{'v', '\v'},
	{'\\', '\\'},
	{'.', '.'},
	{'^', '^'},
	{'?', '?'},
	{'*', '*'},
	{'+', '+'},
	{'|', '|'},
	{'(', '('},
	{')', ')'},
	{'[', '['},
	{']', ']'},
	{'e', EPSILON},
	{'a', ALPHA},
	{'d', DIGIT},
	{'w', WORD},
	{'s', SPACE},
	{'S', NON_SPACE},
};

/**
 * @brief 解析转义字符，返回对应的字符
 *
 * @param v viewer
 * @return char
 */
char parseEscape(viewer &v)
{
	char c = v.step();
	// 解析普通转义字符
	if (escape.find(c) != escape.end())
	{
		return escape[c];
	}
	// 解析16进制转义字符
	else if (c == 'x')
	{
		char c1 = v.step();
		char c2 = v.step();
		if (c1 >= '0' && c1 <= '9')
			c1 -= '0';
		else if (c1 >= 'a' && c1 <= 'f')
			c1 -= 'a' - 10;
		else if (c1 >= 'A' && c1 <= 'F')
			c1 -= 'A' - 10;
		else
			throw "Invalid escape sequence";
		if (c2 >= '0' && c2 <= '9')
			c2 -= '0';
		else if (c2 >= 'a' && c2 <= 'f')
			c2 -= 'a' - 10;
		else if (c2 >= 'A' && c2 <= 'F')
			c2 -= 'A' - 10;
		else
			throw "Invalid escape sequence";
		char result = c1 * 16 + c2;
		return result < 0x7f ? result : throw "Invalid escape sequence";
	}
	return c;
}

/**
 * @brief 将带有特殊不可见字符的表达式转换为可读的字符串
 *
 * @param v viewer
 * @return string
 */
string exp2str(string exp)
{
	string result;
	for (char c : exp)
	{
		if (op2str.find(c) != op2str.end())
		{
			result += "`" + op2str[c] + "`";
		}
		else if (sym2str.find(c) != sym2str.end())
		{
			result += "`" + sym2str[c] + "`";
		}
		else
		{
			result += string(1, c);
		}
	}
	return result;
}

/**
 * @brief 有限状态自动机
 */
class FiniteAutomaton
{
	struct State
	{
		int id;
		bool isFinal; // 是否为终态
		State(int id, bool isFinal) : id(id), isFinal(isFinal) {}
	};
	int startState = 0;								// 开始状态
	vector<State> states;							// 状态集合
	unordered_map<int, transition_map> transitions; // 转移函数

public:
	FiniteAutomaton() {}
	int addState(bool isFinal);							   // 添加状态
	void setStartState(int id);							   // 设置开始状态
	void setState(int id, bool isFinal);				   // 设置状态（是否为终态）
	void addTransition(int from, int to, char symbol);	   // 添加转移
	bool accepts(viewer &view, string &result, int start); // 从指定状态开始匹配，递归地匹配每个字符，直到无法匹配为止
	void print();										   // 打印自动机的状态和转移函数

	vector<State> getStates()
	{
		return states;
	}

	unordered_map<int, transition_map> getTransitions()
	{
		return transitions;
	}
};

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
bool FiniteAutomaton::accepts(viewer &view, string &result, int start = -1)
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

int FiniteAutomaton::addState(bool isFinal = false)
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

string Regexp2FA::getStackDesc()
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

FiniteAutomaton Regexp2FA::convert()
{
	regexpPreproc();
	tmpReg2postfix();
	postfix2FA();
	return nfa;
}

void Regexp2FA::parseSet(string setExp)
{
	debug(0) << "parse set: " << setExp << endl;
	viewer tmpView(setExp);
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
	viewer view(pExpStr);
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
					throw runtime_error("invalid character in set");
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
				throw runtime_error("invalid character in set");
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

void Regexp2FA::opSelect()
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

void Regexp2FA::opStar()
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

void Regexp2FA::opPlus()
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

void Regexp2FA::opQues()
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

void Regexp2FA::opConcat()
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

void Regexp2FA::regexpPreproc()
{
	info << "regexpPreproc: " << exp2str(rawReg) << endl;
	viewer rawView(rawReg);
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
	viewer tmpView(tmpStr);
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

void Regexp2FA::tmpReg2postfix()
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

void Regexp2FA::postfix2FA()
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
		throw runtime_error("postfix2FA: Failed");
	}
}

set<string> keywords = {
	"auto", "break", "case", "char", "const", "continue", "default",
	"do", "double", "else", "enum", "extern", "float", "for", "goto",
	"if", "int", "long", "register", "return", "short", "signed",
	"sizeof", "static", "struct", "switch", "typedef", "union",
	"unsigned", "void", "volatile", "while"};

set<string> separators = {
	"(", ")", "[", "]", "{", "}", ",", ";", ":", ".",
	"&", "*", "+", "-", "~", "!", "/", "%", "<", ">",
	"^", "|", "=", "?", "#", "\\", "\"", "\`"};

enum TokenType
{
	BLANK,
	IGNORE,
	MACRO,
	INCLUDE,
	IDENTIFIER,
	STRING,
	CHARACTER,
	REAL,
	INTEGER,
	SEPARATOR,
};

map<TokenType, string> tok2str = {
	{BLANK, "BLANK"},
	{IGNORE, "IGNORE"},
	{IDENTIFIER, "IDENTIFIER"},
	{SEPARATOR, "SEPARATOR"},
	{STRING, "STRING"},
	{CHARACTER, "CHARACTER"},
	{INTEGER, "INTEGER"},
	{REAL, "REAL"},
	{MACRO, "MACRO"},
	{INCLUDE, "INCLUDE"}};

map<string, TokenType> str2tok = {
	{"BLANK", BLANK},
	{"IGNORE", IGNORE},
	{"IDENTIFIER", IDENTIFIER},
	{"SEPARATOR", SEPARATOR},
	{"STRING", STRING},
	{"CHARACTER", CHARACTER},
	{"INTEGER", INTEGER},
	{"REAL", REAL},
	{"MACRO", MACRO},
	{"INCLUDE", INCLUDE}};

class Lexer
{
	map<string, unsigned int> rvIdMap;			   // 保留字种别码对照表
	map<TokenType, vector<FiniteAutomaton>> faMap; // 状态自动机对照表
	vector<pair<TokenType, string>> tokens;		   // 词法单元序列
	vector<string> literals;					   // 字符串字面量序列
	string code;								   // 代码文本
	void printToken(int idx);

public:
	Lexer() {}
	void addKeywordId(string keyword, unsigned int id);
	void addTokenType(TokenType type, string regExp);
	void readCodeFile(string filename);
	void readLexerDef(string filename);
	void tokenize(string codeSeg);
	void printTokens();
	void clear();
};

void Lexer::addKeywordId(string keyword, unsigned int id)
{
	rvIdMap[keyword] = id;
}

void Lexer::addTokenType(TokenType type, string regExp)
{
	Regexp2FA reg2FA(regExp);
	FiniteAutomaton nfa = reg2FA.convert();
	faMap[type].push_back(nfa);
}

void Lexer::tokenize(string codeSeg = "")
{
	if (codeSeg != "")
		this->code = codeSeg;
	info << "Tokenizing... " << endl;
	code_viewer vCode(code);
	while (!vCode.ends())
	{
		bool matched = false;
		string matchedToken;
		TokenType matchedType;
		viewer matchedView = vCode;
		for (auto tokFa : faMap) // 按序遍历所有的状态自动机
		{
			for (auto nfa : tokFa.second)
			{
				viewer vTmp = vCode;
				string matchResult;
				if (nfa.accepts(vTmp, matchResult))
				{
					if (matchResult.length() > matchedToken.length()) // 同类型的自动机取匹配的最长词法单元
					{
						matchedToken = matchResult;
						matchedType = tokFa.first;
						matchedView = vTmp;
						matched = true;
					}
				}
			}
			if (matched) // 按照顺序，一旦有某种类型的自动机匹配成功，就不再匹配其他类型的自动机
				break;
		}
		if (matched)
		{
			if (matchedType == IDENTIFIER || matchedType == SEPARATOR)
			{
				// 默认只认为标识符和分隔符是保留字，拥有种别码
				if (rvIdMap.find(matchedToken) != rvIdMap.end())
				{
					// 如果是保留字，就把它的种别码作为词法单元的种别码
					tokens.push_back(make_pair(matchedType, to_string(rvIdMap[matchedToken])));
				}
				else
				{
					// 否则就把它的字符串作为词法单元的种别码
					tokens.push_back(make_pair(matchedType, matchedToken));
				}
				literals.push_back(matchedToken); // 加入字面量序列，以变查看
			}
			else if (matchedType != IGNORE && matchedType != BLANK)
			{
				// 忽略空白和注释，其他的都作为词法单元
				tokens.push_back(make_pair(matchedType, matchedToken));
				literals.push_back(matchedToken); // 加入字面量序列，以变查看
			}
			vCode = matchedView;
			debug(0) << "Tokenize accepted: " << matchedToken << endl;
		}
		else
		{
			auto lc = vCode.getCurLineCol();
			error << "Tokenize failed at <" << lc.first << ", " << lc.second << ">" << endl;
			// 打印出错位置的上下文
			vCode.printCodeCtx();
			vCode.skipToNextLine();
		}
	}
}

void Lexer::printToken(int idx)
{
	pair<TokenType, string> token = tokens[idx];
	cout << "(" << setw(10) << right << tok2str[token.first] << ", ";
	cout << setw(16) << left << token.second << ")";
	cout << " : " << literals[idx];
	cout << endl;
}

void Lexer::printTokens()
{
	info << "Tokens: " << endl;
	for (int i = 0; i < tokens.size(); i++)
	{
		printToken(i);
	}
}

void Lexer::clear()
{
	tokens.clear();
}

void Lexer::readCodeFile(string fileName)
{
	ifstream ifs(fileName);
	string _code((istreambuf_iterator<char>(ifs)),
				 (istreambuf_iterator<char>()));
	code = _code;
	info << "Code: " << endl;
	cout << code << endl;
}

bool startWith(const string &s, const string &prefix)
{
	return (s.length() >= prefix.length()) && (s.substr(0, prefix.length()) == prefix);
}

vector<string> split(const string &s, char delimiter)
{
	vector<string> tokens;
	if (s.empty())
	{
		return tokens;
	}
	stringstream ss(s);
	string token;
	while (getline(ss, token, delimiter))
	{
		if (!token.empty())
		{
			tokens.push_back(move(token));
		}
	}
	if (s.back() == delimiter)
	{
		tokens.pop_back();
	}
	return move(tokens);
}

void Lexer::readLexerDef(string fileName)
{
	ifstream ifs(fileName);
	vector<string> tokens;
	// 读取关键字，使用cin循环读入，并存到vector中
	string token;
	while (ifs >> token)
	{
		tokens.push_back(token);
	}
	for (auto tok : tokens)
	{
		debug(1) << tok << endl;
	}
	// 解析PATTERN
	auto patternIt = find(tokens.begin(), tokens.end(), "PATTERN");
	if (patternIt != tokens.end())
	{
		auto ptnStart = find(patternIt, tokens.end(), "{");
		auto ptnEnd = find(patternIt, tokens.end(), "}");
		for (auto it = ptnStart + 1; it != ptnEnd; it += 2)
		{
			if (startWith(*it, "//"))
				continue;
			if (str2tok.find(*it) != str2tok.end())
			{
				addTokenType(str2tok[*it], *(it + 1));
			}
			else
			{
				error << "Unknown token type: " << *it << endl;
				throw runtime_error("Unknown token type");
			}
		}
	}
	// 解析RESERVED
	auto reservedIt = find(tokens.begin(), tokens.end(), "RESERVED");
	if (reservedIt != tokens.end())
	{
		auto rvStart = find(reservedIt, tokens.end(), "{");
		auto rvEnd = find(reservedIt, tokens.end(), "}");
		int id = 0;
		for (auto it = rvStart + 1; it != rvEnd;)
		{
			if (startWith(*it, "//"))
				continue;
			addKeywordId(*it, id++);
			it++;
			if (it != rvEnd)
				it++;
		}
	}
}

void lexerMain()
{
	Lexer lexer;
	lexer.readLexerDef("./assets/lexer.def");
	// lexer.readCodeFile("./assets/code.cpp");
	lexer.readCodeFile("./assets/error.cpp");
	lexer.tokenize();
	lexer.printTokens();
}

void lexerTest()
{
	Lexer lexer;
	int id = 0;
	for (auto &keyword : keywords)
	{
		lexer.addKeywordId(keyword, id++);
	}
	for (auto &sep : separators)
	{
		lexer.addKeywordId(sep, id++);
	}
	lexer.addTokenType(BLANK, "[\\t\\n\\r\\f\\v ]+");
	lexer.addTokenType(IGNORE, "\"([\\x0a-\\x0d\\x20-x21\\0x23-\\x7e]|\\\\\")*\"");
	lexer.addTokenType(IGNORE, "/\\\*([\\x0a-\\x0d\\x20-\\x29\\x2b-\\x7e]*|\\*[\\x0a-\\x0d\\x20-\\x2e\\x30-\\x7e]*)*\\*/");
	lexer.addTokenType(IDENTIFIER, "[a-zA-Z_][a-zA-Z0-9_]*");
	lexer.addTokenType(SEPARATOR, "[\\+\\-\\*\\\\%=\\(\\){}\\[\\]<>;,.\\|&^!]");
	lexer.addTokenType(SEPARATOR, ">=|<=|!=|==|\\+\\+|\\-\\-|\\|\\||&&|\\*=|/=|\\+=|\\-=|%=|<<|>>");
	lexer.addTokenType(STRING, "\"([\\x0a-\\x0d\\x20-x21\\0x23-\\x7e]|\\\")*\"");
	lexer.addTokenType(MACRO, "#[a-zA-Z][a-zA-Z0-9]*");
	lexer.addTokenType(INCLUDE, "\"[a-zA-Z0-9_.]+\"|<[a-zA-Z0-9_.]+>");
	lexer.addTokenType(INTEGER, "(\\-|\\+|\\e)[0-9]+");
	lexer.addTokenType(REAL, "(\\-|\\+|\\e)[0-9]+\\.[0-9]+");
	lexer.addTokenType(CHARACTER, "\'[\\x0a-\\x0d\\x20-x21\\0x23-\\x7e]*\'");
	lexer.tokenize("int main() { int a = 1; int b = 2; int c = a + b; return 0; }");
	lexer.printTokens();
}

void nfaTest() // 实验性质的测试函数
{
	string raw = "";
	cin >> raw;
	cout << "input raw: " << raw << endl;
	Regexp2FA convertor = Regexp2FA(raw);
	convertor.regexpPreproc();
	convertor.tmpReg2postfix();
	convertor.postfix2FA();
	FiniteAutomaton nfa = convertor.getNFA();
	nfa.print();
	string word;
	while (true)
	{
		cout << "input word: ";
		cin >> word;
		cout << "inputted word: " << word << endl;
		viewer vWord = viewer(word);
		string result;
		bool res = nfa.accepts(vWord, result);
		cout << "match result: " << (res ? "yes" : "no") << endl;
		cout << "matched word: " << result << endl;
	}
}

int main()
{
	// lexerTest();
	// nfaTest();
	lexerMain();
	return 0;
}
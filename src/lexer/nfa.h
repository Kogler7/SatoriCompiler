#pragma once
#include <set>
#include <vector>
#include <string>
#include <unordered_map>
#include "viewer.h"

typedef pair<int, int> sub_nfa;
typedef unordered_map<char, set<int>> transition_map;

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
    int startState = 0;                             // 开始状态
    vector<State> states;                           // 状态集合
    unordered_map<int, transition_map> transitions; // 转移函数

public:
    FiniteAutomaton() {}
    int addState(bool isFinal = false);                         // 添加状态
    void setStartState(int id);                                 // 设置开始状态
    void setState(int id, bool isFinal);                        // 设置状态（是否为终态）
    void addTransition(int from, int to, char symbol);          // 添加转移
    bool accepts(viewer &view, string &result, int start = -1); // 从指定状态开始匹配，递归地匹配每个字符，直到无法匹配为止
    void print();                                               // 打印自动机的状态和转移函数

    vector<State> getStates()
    {
        return states;
    }

    unordered_map<int, transition_map> getTransitions()
    {
        return transitions;
    }
};
/**
 * @file nfa.h
 * @author Zhenjie Wei (2024108@bjtu.edu.cn)
 * @brief Non-deterministic Finite Automaton
 * @date 2023-04-22
 *
 * @copyright Copyright (c) 2023
 *
 */

#pragma once
#include <set>
#include <vector>
#include <string>
#include <unordered_map>
#include "utils/viewer.h"

typedef int state_id;
typedef pair<state_id, state_id> sub_nfa;
typedef unordered_map<char, set<state_id>> transition_map;

/**
 * @brief 非确定的有限状态自动机
 */
class FiniteAutomaton
{
    struct State
    {
        state_id id;
        bool isFinal; // 是否为终态
        State(state_id id, bool isFinal) : id(id), isFinal(isFinal) {}
    };
    state_id startState = 0;                             // 开始状态
    vector<State> states;                                // 状态集合
    unordered_map<state_id, transition_map> transitions; // 转移函数

public:
    FiniteAutomaton() {}
    int addState(bool isFinal = false);                              // 添加状态
    void setStartState(state_id id);                                 // 设置开始状态
    void setState(state_id id, bool isFinal);                        // 设置状态（是否为终态）
    void addTransition(state_id from, state_id to, char symbol);     // 添加转移
    bool accepts(Viewer &view, string &result, state_id start = -1); // 从指定状态开始匹配，递归地匹配每个字符，直到无法匹配为止
    void print();                                                    // 打印自动机的状态和转移函数

    vector<State> getStates()
    {
        return states;
    }

    unordered_map<int, transition_map> getTransitions()
    {
        return transitions;
    }
};
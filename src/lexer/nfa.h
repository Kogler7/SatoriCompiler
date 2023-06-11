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
#include "utils/view/viewer.h"

typedef size_t state_id_t;
typedef pair<state_id_t, state_id_t> sub_nfa_t;
typedef unordered_map<char, set<state_id_t>> transition_map_t;

/**
 * @brief 非确定的有限状态自动机
 */
class FiniteAutomaton
{
    struct State
    {
        state_id_t id;
        bool isFinal; // 是否为终态
        State(state_id_t id, bool isFinal) : id(id), isFinal(isFinal) {}
    };
    state_id_t startState = 0;                               // 开始状态
    vector<State> states;                                    // 状态集合
    unordered_map<state_id_t, transition_map_t> transitions; // 转移函数

public:
    FiniteAutomaton() {}
    state_id_t addState(bool isFinal = false);                               // 添加状态
    void setStartState(state_id_t id);                                       // 设置开始状态
    void setState(state_id_t id, bool isFinal);                              // 设置状态（是否为终态）
    void addTransition(state_id_t from, state_id_t to, char symbol);         // 添加转移
    bool accepts(Viewer &view, string &result, state_id_t start = -1) const; // 从指定状态开始匹配，递归地匹配每个字符，直到无法匹配为止
    void printStates() const;                                                // 打印状态集合
    void printTransitions() const;                                           // 打印转移函数

    vector<State> getStates() const
    {
        return states;
    }

    unordered_map<size_t, transition_map_t> getTransitions()
    {
        return transitions;
    }
};
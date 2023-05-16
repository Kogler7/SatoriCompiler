/**
 * @file fa.h
 * @author Zhenjie Wei (2024108@bjtu.edu.cn)
 * @brief NFinite Automaton
 * @date 2023-05-16
 *
 * @copyright Copyright (c) 2023
 *
 */

#pragma once

#include <set>
#include <vector>
#include <unordered_map>

using namespace std;

typedef int state_id;
typedef pair<state_id, state_id> sub_fa;
typedef unordered_map<char, set<state_id>> transition_map;
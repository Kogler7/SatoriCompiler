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

typedef size_t state_id_t;
typedef pair<state_id_t, state_id_t> sub_fa_t;
typedef unordered_map<char, set<state_id_t>> transition_map_t;
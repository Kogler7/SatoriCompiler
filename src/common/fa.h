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

using state_id_t = size_t;
using sub_fa_t = std::pair<state_id_t, state_id_t>;
using transition_map_t = std::unordered_map<char, std::set<state_id_t>>;
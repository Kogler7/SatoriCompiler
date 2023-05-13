/**
 * @file utils/stl.h
 * @author Zhenjie Wei (2024108@bjtu.edu.cn)
 * @brief My Custom STL Utils
 * @date 2023-05-13
 *
 * @copyright Copyright (c) 2023
 *
 */

#pragma once

#define _find(x, y) (x.find(y) != x.end())

#include <string>

std::string trim(const std::string &str, const std::string &whitespace = " \t\0");
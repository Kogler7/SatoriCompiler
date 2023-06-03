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
#include <sstream>

std::string trim(const std::string &str, const std::string &whitespace = " \t\0");

template <typename T>
std::string container2str(T s, std::string sep = ", ", std::string lr = "{}")
{
    std::stringstream ss;
    if (lr.size() > 0)
        (ss << lr[0]);
    for (auto it = s.begin(); it != s.end(); it++)
    {
        if (it != s.begin())
            ss << sep;
        ss << *it;
    }
    if (lr.size() > 1)
        (ss << lr[1]);
    return ss.str();
}

#define compact(s) container2str(s, "", "")
#define set2str(s) container2str(s, ", ", "{}")
#define vec2str(s) container2str(s, ", ", "[]")
#define compact(s) container2str(s, " ", "")
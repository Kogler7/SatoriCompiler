/**
 * @file utils/str.h
 * @author Zhenjie Wei (2024108@bjtu.edu.cn)
 * @brief String Utils
 * @date 2023-04-22
 *
 * @copyright Copyright (c) 2023
 *
 */

#pragma once

#include <string>
#include <queue>
#include <sstream>
#include <iostream>

class StrFormatter
{
    std::stringstream ss;
    std::queue<std::string> segs;

public:
    StrFormatter(std::string s);

    StrFormatter(const char *s) : StrFormatter(std::string(s)) {}

    template <typename T>
    StrFormatter &operator,(const T &t)
    {
        if (segs.empty())
            return *this;
        ss << segs.front();
        segs.pop();
        ss << t;
        return *this;
    }

    std::string str();
};

#define format(fmt, ...) ((StrFormatter(fmt), ##__VA_ARGS__).str())

inline void print_ln()
{
}

inline void print_ln(std::string s)
{
    std::cout << s << std::endl;
}

template <typename... T>
std::string concat(T... ts)
{
    auto s = std::string();
    ((s += ts, s += " "), ...);
    return s;
}
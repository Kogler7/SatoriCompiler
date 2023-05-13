/**
 * @file utils/stl.cpp
 * @author Zhenjie Wei (2024108@bjtu.edu.cn)
 * @brief My Custom STL Utils
 * @date 2023-05-14
 *
 * @copyright Copyright (c) 2023
 *
 */

#include "stl.h"

std::string trim(const std::string &str, const std::string &whitespace)
{
    const auto strBegin = str.find_first_not_of(whitespace);
    if (strBegin == std::string::npos)
    {
        return "";
    }
    const auto strEnd = str.find_last_not_of(whitespace);
    const auto strRange = strEnd - strBegin + 1;
    return str.substr(strBegin, strRange);
}
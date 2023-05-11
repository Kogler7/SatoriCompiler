/**
 * @file log.cpp
 * @author Zhenjie Wei (2024108@bjtu.edu.cn)
 * @brief Level-based Log
 * @date 2023-04-22
 *
 * @copyright Copyright (c) 2023
 *
 */

#include "log.h"

StrFormatter::StrFormatter(std::string s)
{
    std::stringstream ss(s);
    std::string seg;
    while (std::getline(ss, seg, '$'))
        segs.push(seg);
}

std::string StrFormatter::str()
{
    while (!segs.empty())
    {
        ss << segs.front();
        segs.pop();
    }
    return ss.str();
}

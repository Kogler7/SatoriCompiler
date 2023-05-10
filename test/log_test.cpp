/**
 * @file log_test.cpp
 * @author Zhenjie Wei (2024108@bjtu.edu.cn)
 * @brief Test Logger
 * @date 2023-05-06
 *
 * @copyright Copyright (c) 2023
 *
 */

#include "utils/log.h"
#include "test.h"

void logTest()
{
    // info << "info" << std::endl;
    // warn << "warn" << std::endl;
    // error << "error" << std::endl;
    // std::cout << format("hello $ world $", std::string("hi"), 123) << std::endl;
    // assert(0 == 1, "assert");
    // assert(0 == 1, format("assert $ $", 1));
    t_head << "head1" << "head2" << "head3";
    t_col << AL_LFT << AL_RGT << AL_MID;
    t_row << "roaw1" << "rosefsefw3";
    t_row << "s" << "rsefow2" << "ef";
    t_line;
    t_row << "roaw1" << "s" << "rosefsefw3";
    std::cout << t_view;
}
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
#include "utils/table.h"
#include "test.h"

void logTest()
{
    // info << "info" << std::endl;
    // warn << "warn" << std::endl;
    // error << "error" << std::endl;
    // std::cout << format("hello $ world $", std::string("hi"), 123) << std::endl;
    // assert(0 == 1, "assert");
    // assert(0 == 1, format("assert $ $", 1));
    // tb_head | "head1" | "head2" | "head3" | "head4" | "he5";
    // set_col | AL_LFT | AL_RGT | AL_MID;
    // set_row | "roaw1" | TB_GAP | "rosefsdefw3";
    // tb_line;
    // set_row | MD_TAB | "s" | MD_TAB | "rowsefsefsefsef";
    // tb_line;
    // set_row | "roaw1" | "s" | "row" | RT_TAB | "rosefsefw3";
    // tb_line;
    // set_row | MD_TAB | "roadgrgdrgdrw1" | "row" | LF_TAB | "rosefsefw3";
    tb_head | "head1" | "head2";
    for (int i = 0; i < 10; i++)
    {
        set_row | "1" | "2";
    }
    std::cout << tb_view;
}
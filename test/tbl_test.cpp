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

using namespace table;

void tbl1()
{
    tb_head | "head1" | "head2" | "head3" | "head4" | "he5";
    set_col | AL_LFT | AL_RGT | AL_CTR;
    new_row | "roaw1" | TB_GAP | "rosefsdefw3";
    tb_line();
    new_row | MD_TAB | "s" | MD_TAB | "rowsefsefsefsef";
    tb_line();
    new_row | "roaw1" | "s" | "row" | RT_TAB | "rosefsefw3";
    tb_line();
    new_row | MD_TAB | "roadgrgdrgdrw1" | "row" | LF_TAB | "rosefsefw3";
    std::cout << tb_view();
}

void tbl2()
{
    tb_head | "head1" | "head2";
    for (int i = 0; i < 10; i++)
    {
        new_row | "1" | "2";
        tb_cont = FORE_BLU | BACK_GRE | FONT_BOL;
    }
    std::cout << tb_view();
}

void tblTest()
{
    tbl1();
    tbl2();
}
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
    tb_head | "head1" | Cell("head3") & BACK_GRE &AL_CTR | "head4";
    for (int i = 0; i < 4; i++)
    {
        new_row | "1" | Cell("asdfghjkl") & (FORE_GRE | BACK_YEL | FONT_BOL);
        new_row | "2" | Cell("asdkl") & AL_RGT | "3";
        tb_cont = FORE_BLU | BACK_RED | FONT_UND;
    }
    new_col | "head5" | "5" | "6";
    tb_line(-2);
    new_row | "7" | "8" | "9";
    tb_line();
    set_col | FONT_ITA | AL_RGT | AL_CTR;
    new_row | "0" | MD_TAB | "12";
    tb_move(0, 0);
    tb_cont = FORE_WHI | BACK_CYA | FONT_BOL;
    std::cout << tb_view(BDR_RUD);
}

void tblTest()
{
    tbl1();
    tbl2();
}
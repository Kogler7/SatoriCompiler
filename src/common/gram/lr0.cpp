/**
 * @file gram/lr0.cpp
 * @author Zhenjie Wei (2024108@bjtu.edu.cn)
 * @brief LR(0) Grammar
 * @date 2023-05-16
 *
 * @copyright Copyright (c) 2023
 *
 */

#include "lr0.h"
#include "utils/log.h"
#include "utils/stl.h"
#include "utils/table.h"

using namespace table;

void LR0Grammar::calcLR0Closure()
{
}

void LR0Grammar::calcLR0Goto()
{
}

void LR0Grammar::calcLR0Items()
{
    info << "Calculating LR(0) items..." << endl;
    for (auto &p : products)
    {
        for (size_t i = 0; i <= p.second.size(); i++)
        {
            lr0_item item(p, i);
            items.push_back(item);
        }
    }
}

void LR0Grammar::printItems()
{
    info << "LR(0) items:" << endl;
    tb_head | TB_TAB | TB_TAB | TB_TAB | MD_TAB | "Items";
    set_col | AL_RGT | AL_CTR | AL_RGT | AL_CTR | AL_LFT;
    for (auto &item : items)
    {
        symbol left = item.first.get().first;
        symstr right = item.first.get().second;
        size_t pos = item.second;
        symstr r1 = symstr(right.begin(), right.begin() + pos);
        symstr r2 = symstr(right.begin() + pos, right.end());
        new_row | left | "->" | compact(r1) | "." | compact(r2);
    }
    cout << tb_view(BDR_RUD);
}

void LR0Grammar::printGotoTable()
{
    info << "LR(0) goto table:" << endl;
}

void LR0Grammar::printActTable()
{
    info << "LR(0) action table:" << endl;
}

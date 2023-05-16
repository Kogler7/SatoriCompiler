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
}

void LR0Grammar::printGotoTable()
{
}

void LR0Grammar::printActTable()
{
}

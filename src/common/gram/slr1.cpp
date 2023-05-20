/**
 * @file gram/slr1.cpp
 * @author Zhenjie Wei (2024108@bjtu.edu.cn)
 * @brief SLR(1) Grammar
 * @date 2023-05-16
 *
 * @copyright Copyright (c) 2023
 *
 */

#include "slr1.h"
#include "utils/stl.h"
#include "utils/table.h"
#include <queue>

using namespace table;

inline symset_t intersects(const symset_t &s1, const symset_t &s2)
{
    symset_t inter;
    set_intersection(s1.begin(), s1.end(), s2.begin(), s2.end(), inserter(inter, inter.begin()));
    return inter;
}

bool SLR1Grammar::checkSLR1()
{
    info << "Checking SLR(1) grammar..." << endl;
    for (size_t i = 0; i < clusters.size(); i++)
    {
        symset_t shiftSet;
        set<symset_t> reduceSet;
        for (auto &item : clusters[i])
        {
            symbol_t left = item.first.get().first;
            symstr_t right = item.first.get().second;
            size_t pos = item.second;
            if (pos == right.size())
            {
                // 如果是规约项目
                if (left == symStart)
                    continue;
                reduceSet.insert(follow[left]);
            }
            else
            {
                // 如果是移进项目
                symbol_t next = right[pos];
                shiftSet.insert(next);
            }
        }
        // 检查Shift-reduce冲突
        for (auto &s : reduceSet)
        {
            if (intersects(s, shiftSet).size() > 0)
            {
                warn << "Shift-reduce conflict found in cluster " << i << "!" << endl;
                return false;
            }
        }
        // 检查Reduce-reduce冲突
        for (auto &s1 : reduceSet)
        {
            for (auto &s2 : reduceSet)
            {
                if (s1 == s2)
                    continue;
                if (intersects(s1, s2).size() > 0)
                {
                    warn << "Reduce-reduce conflict found in cluster " << i << "!" << endl;
                    return false;
                }
            }
        }
    }
    return true;
}

void SLR1Grammar::calcSLR1Table()
{
    info << "Calculating SLR(1) table..." << endl;
    // 填入移进动作
    for (auto &go : goTrans)
    {
        coord_t coord = go.first;
        state_id_t state = go.second;
        if (_find(slr1Table, coord))
        {
            assert(slr1Table[coord].index() == 2, "Conflict in SLR(1) table!");
            assert(get<2>(slr1Table[coord]) == state, "Conflict in SLR(1) table!");
        }
        else
        {
            action_t action(state);
            slr1Table[coord] = action;
        }
    }
    // 填入规约动作
    for (size_t i = 0; i < clusters.size(); i++)
    {
        for (auto &item : clusters[i])
        {
            symbol_t left = item.first.get().first;
            symstr_t right = item.first.get().second;
            size_t pos = item.second;
            if (pos == right.size())
            {
                if (left == symStart)
                {
                    coord_t coord(i, SYM_END);
                    action_t action(true);
                    slr1Table[coord] = action;
                }
                else
                {
                    for (auto &t : follow[left])
                    {
                        coord_t coord(i, t);
                        action_t action(item.first);
                        slr1Table[coord] = action;
                    }
                }
            }
        }
    }
}

void SLR1Grammar::printSLR1Table()
{
    info << "SLR1 table:" << endl;
    tb_head | "State" = AL_CTR;
    for (int i = 0; i < terminals.size() - 1; i++)
        tb_cont | TB_TAB;
    tb_cont | "Action" = AL_CTR;
    for (int i = 0; i < nonTerms.size() - 1; i++)
        tb_cont | TB_TAB;
    tb_cont | "Goto" = AL_CTR;
    new_row | "";
    for (auto &t : terminals)
        tb_cont | t = AL_CTR;
    for (auto &t : nonTerms)
        tb_cont | t = AL_CTR;
    for (int i = 0; i < clusters.size(); i++)
    {
        cluster_t &c = clusters[i];
        new_row | "S" + to_string(i);
        for (auto &t : terminals)
        {
            coord_t coord(i, t);
            if (_find(slr1Table, coord))
            {
                action_t action = slr1Table[coord];
                if (action.index() == 0)
                    tb_cont | Cell("ACC") & (FORE_GRE | FONT_BOL);
                else if (action.index() == 1)
                    tb_cont | product2str(get<1>(action));
                else
                    tb_cont | "S" + to_string(get<2>(action));
            }
            else
                tb_cont | "";
        }
        for (auto &t : nonTerms)
        {
            coord_t coord(i, t);
            if (_find(slr1Table, coord))
            {
                action_t action = slr1Table[coord];
                if (action.index() == 2)
                    tb_cont | to_string(get<2>(action));
                else
                    tb_cont | "";
            }
            else
                tb_cont | "";
        }
    }
    cout << tb_view(BDR_ALL);
}

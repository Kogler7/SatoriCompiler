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

#include <queue>

using namespace std;
using namespace table;

inline symset_t intersects(const symset_t &s1, const symset_t &s2)
{
    symset_t inter;
    set_intersection(s1.begin(), s1.end(), s2.begin(), s2.end(), inserter(inter, inter.begin()));
    return inter;
}

inline void reportConflict(const cluster_t &c, const symset_t &s1, const symset_t &s2, size_t i)
{
    info << "Related cluster: " << endl;
    printCluster(c, i);
    info << "Set (1): " << set2str(s1) << endl;
    info << "Set (2): " << set2str(s2) << endl;
}

bool SLR1Grammar::checkSLR1()
{
    bool flag = true;
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
                reportConflict(clusters[i], shiftSet, s, i);
                flag = false;
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
                    reportConflict(clusters[i], s1, s2, i);
                    flag = false;
                }
            }
        }
    }
    return flag;
}

void SLR1Grammar::calcSLR1Table()
{
    info << "Calculating SLR(1) table..." << endl;
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
    // 填入移进动作
    // 移进在规约之后，可以覆盖规约动作
    // 也就是说，如果有冲突，移进优先
    for (auto &go : goTrans)
    {
        coord_t coord = go.first;
        state_id_t state = go.second;
        if (_find(slr1Table, coord))
        {
            warn << "Conflict found in SLR(1) table! Shift action will be applied!" << endl;
        }
        action_t action(state);
        slr1Table[coord] = action;
    }
}

void SLR1Grammar::printSLR1TableField(coord_t<state_id_t, symbol_t> c) const
{
    tb_head | "State" | "Action/Goto";
    set_col | table::AL_CTR | table::AL_CTR;
    new_row | "S" + to_string(c.first);
    if (_find(slr1Table, c))
    {
        action_t action = slr1Table.at(c);
        if (action.index() == 0)
            tb_cont | Cell("ACC") & (FORE_GRE | FONT_BOL);
        else if (action.index() == 1)
            tb_cont | product2str(get<1>(action));
        else
            tb_cont | "S" + to_string(get<2>(action));
    }
    else
        tb_cont | "";
    cout << tb_view(BDR_ALL);
}

void SLR1Grammar::printSLR1TableOfState(state_id_t s) const
{
    info << "SLR1 table of state " << s << ":" << endl;
    tb_head | "Symbol" | "Action/Goto" = AL_CTR;
    tb_line();
    auto printST = [&](state_id_t s, symbol_t t)
    {
        coord_t c(s, t);
        new_row | t;
        if (_find(slr1Table, c))
        {
            action_t action = slr1Table.at(c);
            if (action.index() == 0)
                tb_cont | Cell("ACC") & (FORE_GRE | FONT_BOL);
            else if (action.index() == 1)
                tb_cont | product2str(get<1>(action));
            else
                tb_cont | "S" + to_string(get<2>(action));
        }
        else
            tb_cont | "";
    };
    new_row | MD_TAB | "Action";
    tb_line();
    for (auto &t : terminals)
    {
        printST(s, t);
    }
    tb_line();
    new_row | MD_TAB | "Goto";
    tb_line();
    for (auto &t : nonTerms)
    {
        printST(s, t);
    }
    cout << tb_view();
}

void SLR1Grammar::printSLR1Table() const
{
    info << "SLR1 table:" << endl;
    tb_head | "State" = AL_CTR;
    for (size_t i = 0; i < terminals.size() - 1; i++)
        tb_cont | TB_TAB;
    tb_cont | "Action" = AL_CTR;
    for (size_t i = 0; i < nonTerms.size() - 1; i++)
        tb_cont | TB_TAB;
    tb_cont | "Goto" = AL_CTR;
    new_row | "";
    for (auto &t : terminals)
        tb_cont | t = AL_CTR;
    for (auto &t : nonTerms)
        tb_cont | t = AL_CTR;
    for (size_t i = 0; i < clusters.size(); i++)
    {
        const cluster_t &c = clusters.at(i);
        new_row | "S" + to_string(i);
        for (auto &t : terminals)
        {
            coord_t coord(i, t);
            if (_find(slr1Table, coord))
            {
                action_t action = slr1Table.at(coord);
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
                action_t action = slr1Table.at(coord);
                if (action.index() == 2)
                    tb_cont | "S" + to_string(get<2>(action));
                else
                    tb_cont | "";
            }
            else
                tb_cont | "";
        }
    }
    cout << tb_view(BDR_ALL);
}

void SLR1Grammar::printSLR1Table(coord_t<state_id_t, symbol_t> c) const
{
    tb_head | "State" = AL_CTR;
}

void SLR1Grammar::printLargeSLR1Table() const
{
    info << "SLR1 table:" << endl;
}
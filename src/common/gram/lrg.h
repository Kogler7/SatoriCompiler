/**
 * @file gram/lrg.h
 * @author Zhenjie Wei (2024108@bjtu.edu.cn)
 * @brief LR Grammar
 * @date 2023-05-19
 *
 * @copyright Copyright (c) 2023
 *
 */

#pragma once

#include "common/fa.h"
#include "utils/stl.h"
#include "utils/log.h"
#include "utils/table.h"
#include "predict.h"

#include <variant>

using reduce_t = product_ref;
using shift_t = state_id_t;
using accept_t = bool;
using action_t = std::variant<accept_t, reduce_t, shift_t>;
using lr_item_t = std::pair<product_ref, size_t>;

inline std::string product2str(product_ref p)
{
    std::string s = p.get().first + "->";
    s += compact(p.get().second);
    return s;
}

inline std::string item2str(lr_item_t item)
{
    symbol_t left = item.first.get().first;
    symstr_t right = item.first.get().second;
    size_t pos = item.second;
    symstr_t r1 = symstr_t(right.begin(), right.begin() + pos);
    symstr_t r2 = symstr_t(right.begin() + pos, right.end());
    return left + "->" + compact(r1) + "." + compact(r2);
}

struct item_less
{
    bool operator()(const lr_item_t &a, const lr_item_t &b) const
    {
        symbol_t l1 = a.first.get().first;
        symstr_t &r1 = a.first.get().second;
        size_t p1 = a.second;
        symbol_t l2 = b.first.get().first;
        symstr_t &r2 = b.first.get().second;
        size_t p2 = b.second;
        if (l1 != l2)
        {
            return l1 < l2;
        }
        if (r1 != r2)
        {
            return r1 < r2;
        }
        if (p1 != p2)
        {
            return p1 < p2;
        }
        return false;
    }
};

using cluster_t = std::set<lr_item_t, item_less>;
using clusters_t = std::vector<cluster_t>;

inline std::string cluster2str(const cluster_t &c)
{
    std::string s = "{";
    for (int i = 0; i < c.size(); i++)
    {
        lr_item_t item = *std::next(c.begin(), i);
        s += item2str(item);
        if (i != c.size() - 1)
            s += ", ";
    }
    s += "}";
    return s;
}

inline void printCluster(const cluster_t &c, int i = 0)
{
    using namespace table;
    info << "Cluster " << i << ": " << std::endl;
    tb_head | TB_TAB | TB_TAB | TB_TAB | MD_TAB | "Cluster " + std::to_string(i);
    set_col | AL_RGT | AL_CTR | AL_RGT | AL_CTR | AL_LFT;
    for (auto &item : c)
    {
        symbol_t left = item.first.get().first;
        symstr_t right = item.first.get().second;
        size_t pos = item.second;
        symstr_t r1 = symstr_t(right.begin(), right.begin() + pos);
        symstr_t r2 = symstr_t(right.begin() + pos, right.end());
        new_row | left | "->" | compact(r1) | "." | compact(r2);
    }
    std::cout << tb_view(BDR_RUD);
}

class LRGrammar : public PredictiveGrammar
{
    void calcItems();
    void calcClosure(cluster_t &c);
    void calcClusters();

public:
    LRGrammar() : PredictiveGrammar(){};
    LRGrammar(const Grammar &g) : PredictiveGrammar(g)
    {
        calcItems();
        cluster_t c0;
        for (auto &p : products)
        {
            if (p.first != symStart)
                continue;
            lr_item_t item(p, 0);
            c0.insert(item);
        }
        calcClosure(c0);
        clusters.push_back(c0);
        calcClusters();
    }
    LRGrammar(const LRGrammar &g) : PredictiveGrammar(g)
    {
        items = g.items;
        clusters = g.clusters;
        goTrans = g.goTrans;
    }
    clusters_t clusters;
    std::vector<lr_item_t> items;
    table_t<state_id_t, symbol_t, state_id_t> goTrans;
    void printItems() const;
    void printClusters() const;
    void printGoTrans() const;
};
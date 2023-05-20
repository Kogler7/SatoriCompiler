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

#include <variant>
#include "common/fa.h"
#include "utils/stl.h"
#include "utils/log.h"
#include "predict.h"

using namespace std;

typedef reference_wrapper<product_t> product_ref;
typedef variant<bool, product_ref, state_id_t> action_t;
typedef pair<product_ref, size_t> lr_item_t;

inline string product2str(product_ref p)
{
    string s = p.get().first + "->";
    s += compact(p.get().second);
    return s;
}

inline string item2str(lr_item_t item)
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

typedef set<lr_item_t, item_less> cluster_t;
typedef vector<cluster_t> clusters_t;
typedef pair<state_id_t, symbol_t> coord_t;

inline string cluster2str(cluster_t c)
{
    string s = "{";
    for (int i = 0; i < c.size(); i++)
    {
        lr_item_t item = *next(c.begin(), i);
        s += item2str(item);
        if (i != c.size() - 1)
            s += ", ";
    }
    s += "}";
    return s;
}

class LRGrammar : public PredictiveGrammar
{
    void calcItems();
    void calcClosure(cluster_t &c);
    void calcClusters();

public:
    LRGrammar() : PredictiveGrammar(){};
    LRGrammar(const Grammar &g)
    {
        symStart = g.symStart;
        terminals = g.terminals;
        nonTerms = g.nonTerms;
        tok2sym = g.tok2sym;
        products = g.products;
        rules = g.rules;
        // 显式地只计算follow集，因为LR分析表只需要follow集
        // 同时，避免左递归对first集的影响
        calcFollow();
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
    LRGrammar(const LRGrammar &g)
    {
        symStart = g.symStart;
        terminals = g.terminals;
        nonTerms = g.nonTerms;
        tok2sym = g.tok2sym;
        products = g.products;
        rules = g.rules;
        first = g.first;
        firstS = g.firstS;
        follow = g.follow;
        select = g.select;
        items = g.items;
        clusters = g.clusters;
        goTrans = g.goTrans;
    }
    clusters_t clusters;
    vector<lr_item_t> items;
    map<coord_t, state_id_t> goTrans;
    void printItems();
    void printClusters();
    void printGoTrans();
};
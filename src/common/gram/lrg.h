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
#include "predict.h"

using namespace std;

typedef reference_wrapper<product> product_ref;
typedef variant<bool, product_ref, state_id> action_t;
typedef pair<product_ref, size_t> lr_item_t;

struct item_less
{
    bool operator()(const lr_item_t &a, const lr_item_t &b) const
    {
        if (a.first.get().first != b.first.get().first)
            return a.first.get().first < b.first.get().first;
        if (a.first.get().second.size() != b.first.get().second.size())
            return a.first.get().second.size() < b.first.get().second.size();
        if (a.second != b.second)
            return a.second < b.second;
        for (size_t i = 0; i < a.first.get().second.size(); i++)
        {
            if (a.first.get().second[i] != b.first.get().second[i])
                return a.first.get().second[i] < b.first.get().second[i];
        }
        return false;
    }
};

typedef set<lr_item_t, item_less> cluster_t;
typedef vector<cluster_t> clusters_t;
typedef pair<state_id, symbol> coord_t;

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
    vector<lr_item_t> items;
    map<coord_t, state_id> goTrans;
    void printItems();
    void printClusters();
    void printGoTrans();
};
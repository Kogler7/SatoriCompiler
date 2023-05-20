/**
 * @file gram/lrg.cpp
 * @author Zhenjie Wei (2024108@bjtu.edu.cn)
 * @brief LR Grammar
 * @date 2023-05-19
 *
 * @copyright Copyright (c) 2023
 *
 */

#include "lrg.h"
#include "utils/table.h"
#include <queue>

using namespace table;

#define DEBUG_LEVEL 0

#define _isVT(t) _find(terminals, t)
#define _isVN(t) _find(nonTerms, t)

size_t intersects(const cluster_t &c1, const cluster_t &c2)
{
    size_t cnt = 0;
    for (auto &item : c1)
    {
        if (c2.find(item) != c2.end())
        {
            cnt++;
        }
    }
    return cnt;
}

int findInClusters(const clusters_t &clusters, const cluster_t &c)
{
    for (size_t i = 0; i < clusters.size(); i++)
    {
        if (clusters[i].size() != c.size())
            continue;
        if (intersects(clusters[i], c) == c.size())
            return i;
    }
    return -1;
}

void LRGrammar::calcClosure(cluster_t &c)
{
    info << "Calculating LR closure..." << endl;
    while (true)
    {
        info << "new round" << endl;
        cluster_t c1;
        for (auto &item : c)
        {
            symbol_t left = item.first.get().first;
            symstr_t right = item.first.get().second;
            size_t pos = item.second;
            if (pos == right.size())
                continue;
            symbol_t next = right[pos];
            if (_isVT(next))
                continue;
            for (auto &p : products)
            {
                if (p.first != next)
                    continue;
                lr_item_t item(p, 0);
                debug(0) << "inserting " << item2str(item) << endl;
                if (c.find(item) != c.end())
                    continue;
                c1.insert(item);
            }
        }
        if (c1.size() == 0)
            break;
        c.insert(c1.begin(), c1.end());
    }
}

void LRGrammar::calcClusters()
{
    info << "Calculating LR clusters..." << endl;
    queue<cluster_t> q;
    q.push(clusters[0]);
    while (!q.empty())
    {
        cluster_t c = q.front();
        state_id_t cIdx = findInClusters(clusters, c);
        q.pop();
        for (auto &v : nonTerms)
        {
            cluster_t c1;
            for (auto &item : c)
            {
                symbol_t &left = item.first.get().first;
                symstr_t &right = item.first.get().second;
                size_t pos = item.second;
                if (pos == right.size())
                    continue;
                symbol_t &next = right[pos];
                if (next != v)
                    continue;
                lr_item_t item1(item.first, item.second + 1);
                debug(0) << "inserting(nt) " << item2str(item1) << endl;
                c1.insert(item1);
            }
            if (c1.size() == 0)
                continue;
            calcClosure(c1);
            debug(0) << "new cluster(nt): " << cluster2str(c1) << endl;
            int c1Idx = findInClusters(clusters, c1);
            debug(0) << "cluster(nt) index: " << c1Idx << endl;
            if (c1Idx == -1)
            {
                c1Idx = clusters.size();
                clusters.push_back(c1);
                debug(0) << "inserting cluster " << c1Idx << endl;
                q.push(c1);
            }
            coord_t crd(cIdx, v);
            if (_find(goTrans, crd))
            {
                assert(goTrans[crd] == c1Idx, "Unexpected go transition state.");
            }
            else
            {
                goTrans[crd] = c1Idx;
            }
        }
        for (auto &t : terminals)
        {
            cluster_t c1;
            for (auto &item : c)
            {
                symbol_t &left = item.first.get().first;
                symstr_t &right = item.first.get().second;
                size_t pos = item.second;
                if (pos == right.size())
                    continue;
                symbol_t &next = right[pos];
                if (next != t)
                    continue;
                lr_item_t item1(item.first, item.second + 1);
                debug(0) << "inserting(t) " << item2str(item1) << endl;
                c1.insert(item1);
            }
            if (c1.size() == 0)
                continue;
            calcClosure(c1);
            debug(0) << "new cluster(t): " << cluster2str(c1) << endl;
            int c1Idx = findInClusters(clusters, c1);
            debug(0) << "cluster(t) index: " << c1Idx << endl;
            if (c1Idx == -1)
            {
                c1Idx = clusters.size();
                clusters.push_back(c1);
                debug(0) << "inserting cluster " << c1Idx << endl;
                q.push(c1);
            }
            coord_t crd(cIdx, t);
            if (_find(goTrans, crd))
            {
                assert(goTrans[crd] == c1Idx, "Unexpected go transition state.");
            }
            else
            {
                goTrans[crd] = c1Idx;
            }
        }
    }
}

void LRGrammar::calcItems()
{
    info << "Calculating LR items..." << endl;
    for (auto &p : products)
    {
        for (size_t i = 0; i <= p.second.size(); i++)
        {
            lr_item_t item(p, i);
            items.push_back(item);
        }
    }
}

void LRGrammar::printItems()
{
    info << "LR items:" << endl;
    tb_head | TB_TAB | TB_TAB | TB_TAB | MD_TAB | "Items";
    set_col | AL_RGT | AL_CTR | AL_RGT | AL_CTR | AL_LFT;
    for (auto &item : items)
    {
        symbol_t left = item.first.get().first;
        symstr_t right = item.first.get().second;
        size_t pos = item.second;
        symstr_t r1 = symstr_t(right.begin(), right.begin() + pos);
        symstr_t r2 = symstr_t(right.begin() + pos, right.end());
        new_row | left | "->" | compact(r1) | "." | compact(r2);
    }
    cout << tb_view(BDR_RUD);
}

void LRGrammar::printClusters()
{
    info << "LR clusters:" << endl;
    for (size_t i = 0; i < clusters.size(); i++)
    {
        tb_head | TB_TAB | TB_TAB | TB_TAB | MD_TAB | "Cluster " + to_string(i);
        set_col | AL_RGT | AL_CTR | AL_RGT | AL_CTR | AL_LFT;
        for (auto &item : clusters[i])
        {
            symbol_t left = item.first.get().first;
            symstr_t right = item.first.get().second;
            size_t pos = item.second;
            symstr_t r1 = symstr_t(right.begin(), right.begin() + pos);
            symstr_t r2 = symstr_t(right.begin() + pos, right.end());
            new_row | left | "->" | compact(r1) | "." | compact(r2);
        }
        cout << tb_view(BDR_RUD);
    }
}

void LRGrammar::printGoTrans()
{
    info << "LR go transitions:" << endl;
    tb_head | "State";
    for (int i = 0; i < terminals.size() - 1; i++)
        tb_cont | MD_TAB;
    tb_cont | "Terminals";
    for (int i = 0; i < nonTerms.size() - 1; i++)
        tb_cont | MD_TAB;
    tb_cont | "Non-Terms";
    new_row | "" = AL_CTR;
    for (auto &t : terminals)
        tb_cont | t;
    for (auto &v : nonTerms)
        tb_cont | v;
    for (int i = 0; i < clusters.size(); i++)
    {
        new_row | "C" + to_string(i);
        for (auto &t : terminals)
        {
            coord_t crd(i, t);
            if (_find(goTrans, crd))
                tb_cont | to_string(goTrans[crd]);
            else
                tb_cont | "";
        }
        for (auto &v : nonTerms)
        {
            coord_t crd(i, v);
            if (_find(goTrans, crd))
                tb_cont | to_string(goTrans[crd]);
            else
                tb_cont | "";
        }
    }
    cout << tb_view(BDR_ALL);
}
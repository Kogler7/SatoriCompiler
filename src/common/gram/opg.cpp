/**
 * @file gram/opg.cpp
 * @author Zhenjie Wei (2024108@bjtu.edu.cn)
 * @brief Operator Precedence Grammar
 * @date 2023-05-12
 *
 * @copyright Copyright (c) 2023
 *
 */

#include "opg.h"
#include "utils/stl.h"
#include "utils/log.h"
#include "utils/table.h"

#define DEBUG_LEVEL 0

#define _isVT(t) _find(terminals, t)
#define _isVN(t) _find(nonTerms, t)

symset_t OperatorPrecedenceGrammar::calcFirstVTOf(symbol_t t)
{
    info << "calcFirstVTOf(" << t << ")" << endl;
    if (firstVT[t].size() > 0)
    {
        debug(0) << format(
            "FirstVT($) has been calculated before.\n",
            vec2str(t));
        return firstVT[t];
    }
    symset_t ret;
    for (auto &right : rules[t])
    {
        assert(!right.empty(), "OperatorPrecedenceGrammar: invalid null production.");
        if (_isVT(right[0]))
        {
            ret.insert(right[0]);
        }
        else
        {
            if (right[0] != t)
            {
                symset_t tmp = calcFirstVTOf(right[0]);
                ret.insert(tmp.begin(), tmp.end());
            }
            if (right.size() > 1 && _isVT(right[1]))
            {
                ret.insert(right[1]);
            }
        }
    }
    firstVT[t] = ret;
    return ret;
}

symset_t OperatorPrecedenceGrammar::calcLastVTOf(symbol_t t)
{
    info << "calcLastVTOf(" << t << ")" << endl;
    if (lastVT[t].size() > 0)
    {
        debug(0) << format(
            "LastVT($) has been calculated before.\n",
            vec2str(t));
        return lastVT[t];
    }
    symset_t ret;
    for (auto &right : rules[t])
    {
        assert(!right.empty(), "OperatorPrecedenceGrammar: invalid null production.");
        if (_isVT(right.back()))
        {
            ret.insert(right.back());
        }
        else
        {
            if (right.back() != t)
            {
                symset_t tmp = calcLastVTOf(right.back());
                ret.insert(tmp.begin(), tmp.end());
            }
            if (right.size() > 1 && _isVT(right[right.size() - 2]))
            {
                ret.insert(right[right.size() - 2]);
            }
        }
    }
    lastVT[t] = ret;
    return ret;
}

void OperatorPrecedenceGrammar::calcFirstVT()
{
    info << "calcFirstVT()" << endl;
    for (auto &t : nonTerms)
    {
        calcFirstVTOf(t);
    }
}

void OperatorPrecedenceGrammar::calcLastVT()
{
    info << "calcLastVT()" << endl;
    for (auto &t : nonTerms)
    {
        calcLastVTOf(t);
    }
}

inline void checkConflict(table_t<symbol_t, symbol_t, int> &opt, const coord_t<symbol_t, symbol_t> &crd, OP op)
{
    assert(
        opt[crd] == OP::NL || opt[crd] == op,
        format(
            "OperatorPrecedenceGrammar: Conflict found in opt[$][$]. (oOp = $, nOp = $)\n",
            crd.first, crd.second, opt[crd], op));
}

void OperatorPrecedenceGrammar::calcOPT()
{
    info << "calcOPT()" << endl;
    // 全部初始化为OP::NL
    for (auto &t : terminals)
    {
        for (auto &s : terminals)
        {
            opt[mkcrd(t, s)] = OP::NL;
        }
    }
    for (auto &t : nonTerms)
    {
        for (auto &right : rules[t])
        {
            for (int i = 0; i < right.size() - 1; i++)
            {
                if (_isVT(right[i]) && _isVT(right[i + 1]))
                {
                    auto crd = mkcrd(right[i], right[i + 1]);
                    checkConflict(opt, crd, OP::EQ);
                    opt[crd] = OP::EQ;
                }
                if (i < right.size() - 2 && _isVT(right[i]) && _isVN(right[i + 1]) && _isVT(right[i + 2]))
                {
                    auto crd = mkcrd(right[i], right[i + 2]);
                    checkConflict(opt, crd, OP::LT);
                    opt[crd] = 0;
                }
                if (_isVT(right[i]) && _isVN(right[i + 1]))
                {
                    symset_t tmp = calcFirstVTOf(right[i + 1]);
                    for (auto &s : tmp)
                    {
                        auto crd = mkcrd(right[i], s);
                        checkConflict(opt, crd, OP::LT);
                        opt[crd] = OP::LT;
                    }
                }
                if (_isVN(right[i]) && _isVT(right[i + 1]))
                {
                    symset_t tmp = calcLastVTOf(right[i]);
                    for (auto &s : tmp)
                    {
                        auto crd = mkcrd(s, right[i + 1]);
                        checkConflict(opt, crd, OP::GT);
                        opt[crd] = OP::GT;
                    }
                }
            }
        }
    }
}

void OperatorPrecedenceGrammar::printFirstVT() const
{
    info << "FirstVT: " << endl;
    tb_head | "Symbol" | "FirstVT" = table::AL_CTR;
    set_col | table::AL_CTR;
    for (auto &t : nonTerms)
    {
        new_row | t + ":" | set2str(firstVT.at(t));
        tb_cont = table::AL_LFT;
    }
    cout << tb_view(table::BDR_RUD);
}

void OperatorPrecedenceGrammar::printLastVT() const
{
    info << "LastVT: " << endl;
    tb_head | "Symbol" | "LastVT" = table::AL_CTR;
    set_col | table::AL_CTR;
    for (auto &t : nonTerms)
    {
        new_row | t + ":" | set2str(lastVT.at(t));
        tb_cont = table::AL_LFT;
    }
    cout << tb_view(table::BDR_RUD);
}

void OperatorPrecedenceGrammar::printOPT() const
{
    info << "Operator Precedence Table:" << endl;
    string ops[4] = {"", "<", "=", ">"};
    tb_head | "";
    for (auto &t : terminals)
    {
        tb_cont | t;
        tb_cont = table::AL_CTR;
    }
    for (auto &t : terminals)
    {
        new_row | t;
        for (auto &t2 : terminals)
        {
            int opt_val = opt.at(mkcrd(t, t2)) + 2;
            assert(
                opt_val >= 0 && opt_val <= 3,
                format("OperatorPrecedenceGrammar: Invalid opt_val: $.\n", opt_val));
            tb_cont | ops[opt_val];
        }
    }
    cout << tb_view(table::BDR_ALL);
}
/**
 * @file lab5.h
 * @author Zhenjie Wei (2024108@bjtu.edu.cn)
 * @brief Visit Lab5
 * @date 2023-06-27
 *
 * @copyright Copyright (c) 2023
 *
 */

#pragma once

#include "common/tree/pst.h"
#include "common/gram/basic.h"

#include <set>
#include <map>

class LAB5Visitor
{
    std::set<symbol_t> funcDef;
    size_t varCnt = 0;
    std::vector<std::string> quads;

public:
    symbol_t visitS(pst_node_ptr_t node);
    symbol_t visitA(pst_node_ptr_t node);
    symbol_t visitE(pst_node_ptr_t node);
    symbol_t visitT(pst_node_ptr_t node);
    symbol_t visitF(pst_node_ptr_t node);
    symbol_t visitP(pst_node_ptr_t node);

    void printQuads()
    {
        info << "LAB5Visitor::printQuads: " << std::endl;
        for (auto quad : quads)
        {
            std::cout << quad << std::endl;
        }
    }
};
/**
 * @file pta.h
 * @author Zhenjie Wei (2024108@bjtu.edu.cn)
 * @brief Predictive Table Analyzer
 * @date 2023-04-27
 *
 * @copyright Copyright (c) 2023
 *
 */

#include "common/gram.h"

class PredictiveTableAnalyzer
{
public:
    map<term, map<term, vector<term>>> predict;
    void calcPredictTable();
};
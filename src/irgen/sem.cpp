/**
 * @file sem.h
 * @author Zhenjie Wei (2024108@bjtu.edu.cn)
 * @brief Semantic Definition
 * @date 2023-06-01
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#include "sem.h"

std::map<std::string, std::function<std::shared_ptr<ASTNode>()>> astNodeFactories;
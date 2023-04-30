/**
 * @file terminal.cpp
 * @author Zhenjie Wei (2024108@bjtu.edu.cn)
 * @brief Terminal
 * @date 2023-04-27
 *
 * @copyright Copyright (c) 2023
 *
 */

#include "utils/tok_view.h"

using namespace std;

class TerminalChecker
{
    TokenViewer &tokens;
    int index;
};
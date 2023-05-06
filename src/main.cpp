/**
 * @file main.cpp
 * @author Zhenjie Wei (2024108@bjtu.edu.cn)
 * @brief Main
 * @date 2023-04-22
 *
 * @copyright Copyright (c) 2023
 *
 */

#define TEST_UNIT astTest

#ifndef TEST_UNIT

int main()
{
    return 0;
}

#else

#include "test.h"
#include "utils/log.h"

#define STR(x) #x

int main()
{
    info << "Testing " << STR(TEST_UNIT) << "..." << std::endl;
    TEST_UNIT();
    return 0;
}

#endif
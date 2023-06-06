/**
 * @file meta_test.cpp
 * @author Zhenjie Wei (2024108@bjtu.edu.cn)
 * @brief Test Meta Parser
 * @date 2023-06-04
 *
 * @copyright Copyright (c) 2023
 *
 */

#include "test.h"
#include "utils/meta.h"

void metaTest()
{
    Viewer v = Viewer::fromFile("./assets/tmp/test.meta");
    MetaParser parser(v);
    parser.printMetas();
}
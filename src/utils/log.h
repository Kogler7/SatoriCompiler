/**
 * @file log.h
 * @author Zhenjie Wei (2024108@bjtu.edu.cn)
 * @brief Level-based Log
 * @date 2023-04-22
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#include <iostream>

#define _red(x) "\033[31m" << x << "\033[0m"
#define _green(x) "\033[32m" << x << "\033[0m"
#define _yellow(x) "\033[33m" << x << "\033[0m"
#define _blue(x) "\033[34m" << x << "\033[0m"

#define info std::cout << _green("[info] ")
#define warn std::cout << _yellow("[warn] ")
#define error std::cout << _red("[error] ")

#define DEBUG_LEVEL -1
#define debug(level)          \
	if (level <= DEBUG_LEVEL) \
	std::cout << _blue("    <" #level ">: ")
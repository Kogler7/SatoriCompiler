/**
 * @file utils/log.h
 * @author Zhenjie Wei (2024108@bjtu.edu.cn)
 * @brief Level-based Log
 * @date 2023-04-22
 *
 * @copyright Copyright (c) 2023
 *
 */

#pragma once

#include "str.h"

#define _red(x) "\033[31m" << x << "\033[0m"
#define _blue(x) "\033[34m" << x << "\033[0m"
#define _green(x) "\033[32m" << x << "\033[0m"
#define _yellow(x) "\033[33m" << x << "\033[0m"

#define info std::cout << _green("[info] ")
#define warn std::cout << _yellow("[warn] ")
#define error std::cout << _red("[error] ")
#define fetal std::cout << _red("[fetal] ")

#define DEBUG_LEVEL -1

#define debug(level)          \
	if (level <= DEBUG_LEVEL) \
	std::cout << _blue("   [" #level "] ")

#define debug_u(level)        \
	if (level <= DEBUG_LEVEL) \
	std::cout


#define assert(x, msg)                                    \
	if (!(x))                                             \
	{                                                     \
		fetal << "Assertion \"" << #x << "\" FAILED at "; \
		std::cout << "\033[4m";                           \
		std::cout << __FILE__ << ":" << __LINE__;         \
		std::cout << "\033[0m" << std::endl;              \
		print_ln(msg);                                    \
		exit(1);                                          \
	}

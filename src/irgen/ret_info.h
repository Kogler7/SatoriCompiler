/**
 * @file ret_info.h
 * @author Zhenjie Wei (2024108@bjtu.edu.cn)
 * @brief AST Visitor Return Info
 * @date 2023-06-08
 *
 * @copyright Copyright (c) 2023
 *
 */

#pragma once

#include "instr.h"
#include "use.h"

#include <map>
#include <list>

using instr_list_t = std::list<user_ptr_t>;

class VisitorRetInfo
{
public:
	instr_list_t instrList;
	instr_list_t valueList;
	std::map<JumpReason, target_list_t> gotoListMap;

	user_ptr_t getValue() const;
	VisitorRetInfo &setValue(user_ptr_t value);

	VisitorRetInfo &addJmpTarget(jmp_ptr_t instr, JumpReason reason);
	VisitorRetInfo &addBrTarget(br_ptr_t instr);
	VisitorRetInfo &addInstr(user_ptr_t instr);
	VisitorRetInfo &addValue(user_ptr_t value);

	VisitorRetInfo &appendInstrList(instr_list_t &list);
	VisitorRetInfo &appendValueList(instr_list_t &list);
	VisitorRetInfo &appendTrueList(target_list_t &list);
	VisitorRetInfo &appendFalseList(target_list_t &list);
	VisitorRetInfo &appendJmpList(target_list_t &list, JumpReason reason);

	VisitorRetInfo &unionInfo(VisitorRetInfo &another);

	target_list_t &getTargetsOf(JumpReason reason);

	bool hasFallThrough() const;

	VisitorRetInfo & backpatch(JumpReason reason, user_ptr_t instr);
};

using ret_info_t = VisitorRetInfo;

template<typename T>
inline std::list<T> &list_concat(std::list<T> &list1, std::list<T> &list2)
{
	list1.splice(list1.end(), list2);
	return list1;
}
/**
 * @file ret_info.cpp
 * @author Zhenjie Wei (2024108@bjtu.edu.cn)
 * @brief AST Visitor Return Info
 * @date 2023-06-19
 *
 * @copyright Copyright (c) 2023
 *
 */

#include "ret_info.h"

bool VisitorRetInfo::hasValue() const
{
    return !valueList.empty();
}

user_ptr_t VisitorRetInfo::getValue() const
{
    assert(!valueList.empty(), "VisitorRetInfo::getValue: Value list is empty");
    assert(valueList.size() == 1, "VisitorRetInfo::getValue: Value list size is not 1");
    return valueList.front();
}

VisitorRetInfo &VisitorRetInfo::setValue(user_ptr_t value)
{
    valueList.clear();
    valueList.push_back(value);
    return *this;
}

VisitorRetInfo &VisitorRetInfo::addJmpTarget(jmp_ptr_t instr, JumpReason reason)
{
    gotoListMap[reason].push_back(instr->getTarget());
    return *this;
}

VisitorRetInfo &VisitorRetInfo::addBrTarget(br_ptr_t instr)
{
    target_ptr_t tc, fc;
    std::tie(tc, fc) = instr->getTargets();
    gotoListMap[JR_TRUE_EXIT].push_back(tc);
    gotoListMap[JR_FALSE_EXIT].push_back(fc);
    return *this;
}

VisitorRetInfo &VisitorRetInfo::addInstr(user_ptr_t instr)
{
    instrList.push_back(instr);
    return *this;
}

VisitorRetInfo &VisitorRetInfo::addValue(user_ptr_t value)
{
    valueList.push_back(value);
    return *this;
}

VisitorRetInfo &VisitorRetInfo::appendInstrList(instr_list_t &list)
{
    instrList.splice(instrList.end(), list);
    return *this;
}

VisitorRetInfo &VisitorRetInfo::appendValueList(instr_list_t &list)
{
    valueList.splice(valueList.end(), list);
    return *this;
}

VisitorRetInfo &VisitorRetInfo::appendTrueList(target_list_t &list)
{
    target_list_t &trueList = gotoListMap[JR_TRUE_EXIT];
    trueList.splice(trueList.end(), list);
    return *this;
}

VisitorRetInfo &VisitorRetInfo::appendFalseList(target_list_t &list)
{
    target_list_t &falseList = gotoListMap[JR_FALSE_EXIT];
    falseList.splice(falseList.end(), list);
    return *this;
}

VisitorRetInfo &VisitorRetInfo::appendJmpList(target_list_t &list, JumpReason reason)
{
    target_list_t &rList = gotoListMap[reason];
    rList.splice(rList.end(), list);
    return *this;
}

target_list_t &VisitorRetInfo::getTargetsOf(JumpReason reason)
{
    return gotoListMap[reason];
}

bool VisitorRetInfo::hasFallThrough() const
{
    if (gotoListMap.count(JR_FALL_THROUGH))
        return !gotoListMap.at(JR_FALL_THROUGH).empty();
    return false;
}

VisitorRetInfo &VisitorRetInfo::unionInfo(VisitorRetInfo &another)
{
    instrList.splice(instrList.end(), another.instrList);
    valueList = another.valueList; // 结果只保留最后一个
    for (auto &p : another.gotoListMap)
        gotoListMap[p.first].splice(gotoListMap[p.first].end(), p.second);
    return *this;
}

VisitorRetInfo &VisitorRetInfo::backpatch(JumpReason reason, user_ptr_t instr)
{
    target_list_t &list = getTargetsOf(reason);
    for (auto &target : list)
        target->patch(instr);
    list.clear();
    return *this;
}

VisitorRetInfo & VisitorRetInfo::shiftReason(JumpReason from, JumpReason to)
{
    target_list_t &list = getTargetsOf(from);
    gotoListMap[to].splice(gotoListMap[to].end(), list);
    return *this;
}